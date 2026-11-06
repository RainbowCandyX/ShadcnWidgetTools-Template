#include "SettingsDialog.h"

#include <QHBoxLayout>
#include <QList>
#include <QLocale>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

#include "AppI18n.h"
#include "ShadButton.h"
#include "ShadComboBox.h"
#include "ShadDef.h"
#include "ShadField.h"
#include "ShadFieldGroup.h"
#include "ShadI18n.h"
#include "ShadTheme.h"
#include "ShadToggle.h"
#include "ShadToggleGroup.h"

namespace
{
    constexpr int kDialogWidth = 480;
    constexpr int kFieldSpacing = 12;
    const QList<int> kRadiusOptions{0, 6, 10, 14};
    // 顺序与 ShadThemeType::BaseColor 一致
    const QStringList kBaseColorNames{QStringLiteral("neutral"), QStringLiteral("zinc"), QStringLiteral("slate"), QStringLiteral("stone"), QStringLiteral("gray")};

    // 单选按钮组：不可取消选中；选项文字随语言刷新
    template <typename OnSelected>
    ShadToggleGroup* createChoiceGroup(const QList<AppI18n::TextFunction>& texts, int currentIndex, QWidget* parent, OnSelected onSelected)
    {
        ShadToggleGroup* group = new ShadToggleGroup(ShadToggleGroupType::Single, parent);
        group->setVariant(ShadToggleType::Outline);
        for (int i = 0; i < texts.size(); ++i)
        {
            group->addItem(QString());
        }
        AppI18n::bind(group, [group, texts]()
        {
            for (int i = 0; i < texts.size(); ++i)
            {
                group->itemAt(i)->setText(texts.at(i)());
            }
        });
        group->setCurrentIndex(currentIndex);
        // 再次点击已选中项会取消选中，此时恢复原选中项
        QObject::connect(group, &ShadToggleGroup::currentIndexChanged, group, [group, onSelected, selectedIndex = currentIndex](int index) mutable
        {
            if (index < 0)
            {
                if (selectedIndex >= 0)
                {
                    group->setCurrentIndex(selectedIndex);
                }
                return;
            }
            selectedIndex = index;
            onSelected(index);
        });
        return group;
    }

    ShadComboBox* createComboBox(const QStringList& texts, int currentIndex, QWidget* parent)
    {
        ShadComboBox* comboBox = new ShadComboBox(parent);
        comboBox->addItems(texts);
        comboBox->setCurrentIndex(currentIndex);
        return comboBox;
    }

    ShadField* createField(AppI18n::TextFunction label, QWidget* control, QWidget* parent)
    {
        ShadField* field = new ShadField(QString(), control, parent);
        field->setOrientation(ShadFieldType::Horizontal);
        AppI18n::bind(field, [field, label]() { field->setLabel(label()); });
        return field;
    }

    int themeIndex()
    {
        if (shadTheme->isFollowSystemTheme())
        {
            return 2;
        }
        return shadTheme->isDark() ? 1 : 0;
    }
} // namespace

SettingsDialog::SettingsDialog(QWidget* parent)
    : ShadDialog(parent)
{
    resize(kDialogWidth, height());
    AppI18n::bind(this, [this]()
    {
        setTitle(tr("设置"));
        setDescription(tr("修改后立即生效。"));
    });

    // 语言名称使用各自的语言显示，不翻译；新增语言时在这里与 CMakeLists.txt 的 TS_FILES 中同时添加
    const QList<QLocale> languages{QLocale(QLocale::Chinese, QLocale::SimplifiedHanScript, QLocale::China), QLocale(QLocale::English, QLocale::UnitedStates)};
    const QLocale currentLanguage = shadI18n->getLanguage();
    int languageIndex = 0;
    for (int index = 0; index < languages.size(); ++index)
    {
        if (languages.at(index).language() == currentLanguage.language())
        {
            languageIndex = index;
            break;
        }
    }
    ShadComboBox* languageComboBox = createComboBox({QStringLiteral("简体中文"), QStringLiteral("English")}, languageIndex, this);
    connect(languageComboBox, &ShadComboBox::currentIndexChanged, this, [languages](int index)
    {
        if (index < 0 || index >= languages.size())
        {
            return;
        }
        shadI18n->setLanguage(languages.at(index));
    });

    ShadToggleGroup* themeGroup = createChoiceGroup({[] { return tr("浅色"); }, [] { return tr("深色"); }, [] { return tr("跟随系统"); }}, themeIndex(), this, [](int index)
    {
        if (index == 2)
        {
            shadTheme->setFollowSystemTheme(true);
            return;
        }
        shadTheme->setThemeMode(index == 1 ? ShadThemeType::Dark : ShadThemeType::Light);
    });

    ShadComboBox* baseColorComboBox = createComboBox(kBaseColorNames, static_cast<int>(shadTheme->getBaseColor()), this);
    connect(baseColorComboBox, &ShadComboBox::currentIndexChanged, this, [](int index)
    {
        if (index >= 0)
        {
            shadTheme->setBaseColor(static_cast<ShadThemeType::BaseColor>(index));
        }
    });

    QList<AppI18n::TextFunction> radiusNames;
    for (int radius : kRadiusOptions)
    {
        radiusNames.append([radius]() { return QString::number(radius); });
    }
    ShadToggleGroup* radiusGroup = createChoiceGroup(radiusNames, kRadiusOptions.indexOf(shadTheme->getBaseRadius()), this, [](int index)
    {
        shadTheme->setBaseRadius(kRadiusOptions.at(index));
    });

    ShadFieldGroup* fieldGroup = new ShadFieldGroup(this);
    fieldGroup->setSpacing(kFieldSpacing);
    fieldGroup->addWidget(createField([] { return tr("语言"); }, languageComboBox, this));
    fieldGroup->addWidget(createField([] { return tr("主题"); }, themeGroup, this));
    fieldGroup->addWidget(createField([] { return tr("基色"); }, baseColorComboBox, this));
    fieldGroup->addWidget(createField([] { return tr("圆角"); }, radiusGroup, this));
    contentLayout()->addWidget(fieldGroup);

    ShadButton* doneButton = new ShadButton(this);
    AppI18n::bind(doneButton, [doneButton]() { doneButton->setText(tr("完成")); });
    doneButton->setDefault(true);
    footerLayout()->addWidget(doneButton);
    connect(doneButton, &ShadButton::clicked, this, &ShadDialog::accept);
}

SettingsDialog::~SettingsDialog()
{
}
