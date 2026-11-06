#include "HomePage.h"

#include <QHBoxLayout>
#include <QList>
#include <QString>
#include <QVBoxLayout>

#include "AppI18n.h"
#include "ShadButton.h"
#include "ShadCard.h"
#include "ShadDef.h"
#include "ShadLabel.h"
#include "ShadToaster.h"

HomePage::HomePage(ShadToaster* toaster, QWidget* parent)
    : BasePage([] { return tr("首页"); }, [] { return tr("从这里开始构建你的应用。"); }, parent)
{
    ShadCard* startCard = addCard([] { return tr("快速开始"); }, [] { return tr("新增一个页面只需要三步。"); });
    const QList<AppI18n::TextFunction> steps{
        [] { return tr("1. 在 src/pages 中新建页面类，继承 BasePage。"); },
        [] { return tr("2. 在 MainWindow 构造函数中调用 addPage，把页面加入侧栏。"); },
        [] { return tr("3. 在 CMakeLists.txt 的 APP_SOURCES 中加入新文件。"); },
    };
    for (const AppI18n::TextFunction& step : steps)
    {
        ShadLabel* stepLabel = new ShadLabel(startCard);
        stepLabel->setWordWrap(true);
        AppI18n::bind(stepLabel, [stepLabel, step]() { stepLabel->setText(step()); });
        startCard->contentLayout()->addWidget(stepLabel);
    }

    ShadCard* demoCard = addCard([] { return tr("试一试"); }, [] { return tr("组件库提供 100 多个组件，完整演示见 ShadcnWidgetTools 示例程序。"); });
    ShadButton* toastButton = new ShadButton(demoCard);
    ShadButton* settingsButton = new ShadButton(QString(), ShadButtonType::Outline, demoCard);
    AppI18n::bind(demoCard, [toastButton, settingsButton]()
    {
        toastButton->setText(tr("显示提示"));
        settingsButton->setText(tr("打开设置"));
    });
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(8);
    buttonLayout->addWidget(toastButton);
    buttonLayout->addWidget(settingsButton);
    buttonLayout->addStretch(1);
    demoCard->contentLayout()->addLayout(buttonLayout);

    connect(toastButton, &ShadButton::clicked, this, [toaster]()
    {
        toaster->add(tr("你好"), tr("这是一条来自 ShadToaster 的提示。"));
    });
    connect(settingsButton, &ShadButton::clicked, this, &HomePage::settingsRequested);
}

HomePage::~HomePage()
{
}
