#include "AboutPage.h"

#include <QCoreApplication>
#include <QDesktopServices>
#include <QString>
#include <QUrl>
#include <QVBoxLayout>
#include <QtCore/qnamespace.h>
#include <QtGlobal>

#include "AppI18n.h"
#include "ShadButton.h"
#include "ShadCard.h"
#include "ShadDef.h"
#include "ShadLabel.h"

AboutPage::AboutPage(QWidget* parent)
    : BasePage([] { return tr("关于"); }, [] { return tr("应用与依赖的版本信息。"); }, parent)
{
    ShadCard* infoCard = addCard([] { return tr("应用信息"); }, [] { return tr("当前运行的应用与 Qt 版本。"); });
    ShadLabel* nameLabel = new ShadLabel(infoCard);
    ShadLabel* versionLabel = new ShadLabel(infoCard);
    ShadLabel* qtLabel = new ShadLabel(infoCard);
    AppI18n::bind(infoCard, [nameLabel, versionLabel, qtLabel]()
    {
        nameLabel->setText(tr("名称：%1").arg(QCoreApplication::applicationName()));
        versionLabel->setText(tr("版本：%1").arg(QCoreApplication::applicationVersion()));
        qtLabel->setText(tr("Qt 版本：%1").arg(QString::fromLatin1(qVersion())));
    });
    infoCard->contentLayout()->addWidget(nameLabel);
    infoCard->contentLayout()->addWidget(versionLabel);
    infoCard->contentLayout()->addWidget(qtLabel);

    ShadCard* linkCard = addCard([] { return tr("相关链接"); }, [] { return tr("组件文档与完整示例。"); });
    ShadButton* repoButton = new ShadButton(QStringLiteral("ShadcnWidgetTools"), ShadButtonType::Link, linkCard);
    // 未指定对齐时 QBoxLayout 会把按钮拉伸到整行宽度
    linkCard->contentLayout()->addWidget(repoButton, 0, Qt::AlignLeft);
    connect(repoButton, &ShadButton::clicked, this, []()
    {
        QDesktopServices::openUrl(QUrl(QStringLiteral("https://github.com/RainbowCandyX/ShadcnWidgetTools")));
    });
}

AboutPage::~AboutPage()
{
}
