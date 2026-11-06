#include "MainWindow.h"

#include <QCoreApplication>
#include <QFont>
#include <QHBoxLayout>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCore/qnamespace.h>

#include "AboutPage.h"
#include "AppI18n.h"
#include "BasePage.h"
#include "HomePage.h"
#include "SettingsDialog.h"
#include "ShadButton.h"
#include "ShadDef.h"
#include "ShadLabel.h"
#include "ShadSeparator.h"
#include "ShadSidebar.h"
#include "ShadSidebarGroup.h"
#include "ShadSidebarLayout.h"
#include "ShadSidebarMenuButton.h"
#include "ShadSidebarTrigger.h"
#include "ShadStackedWidget.h"
#include "ShadTheme.h"
#include "ShadToaster.h"

namespace
{
    constexpr int kSidebarWidth = 240;
    constexpr int kHeaderHeight = 56;
    constexpr int kMinimumWidth = 800;
    constexpr int kMinimumHeight = 600;
    // 与组件库可聚焦组件四周预留的焦点环边距一致，用于对齐可视边缘
    constexpr int kFocusRingWidth = 3;
} // namespace

MainWindow::MainWindow(QWidget* parent)
    : ShadWindow(parent)
{
    setMinimumSize(kMinimumWidth, kMinimumHeight);
    resize(1100, 760);
    setWindowTitle(QCoreApplication::applicationName());

    // 标题栏右侧：切换明暗主题
    _themeButton = new ShadButton(QString(), ShadButtonType::Ghost, this);
    _themeButton->setButtonSize(ShadButtonType::IconSmall);
    setTitleBarWidget(ShadWindowType::RightArea, _themeButton);

    ShadSidebarLayout* sidebarLayout = new ShadSidebarLayout(this);
    _sidebar = new ShadSidebar(sidebarLayout);
    _sidebar->setExpandedWidth(kSidebarWidth);
    _sidebar->setCollapsible(ShadSidebarType::Icon);
    // 品牌按钮：点击回到首页，不占用选中项
    ShadSidebarMenuButton* brandButton = new ShadSidebarMenuButton(ShadIconType::Component, QCoreApplication::applicationName(), _sidebar);
    brandButton->setButtonSize(ShadSidebarMenuButtonType::Large);
    brandButton->setDescription(QStringLiteral("v%1").arg(QCoreApplication::applicationVersion()));
    brandButton->setSelectable(false);
    _sidebar->headerLayout()->addWidget(brandButton);
    // 设置按钮：只打开对话框，不改变选中页面
    ShadSidebarMenuButton* settingsButton = new ShadSidebarMenuButton(ShadIconType::Settings, QString(), _sidebar);
    settingsButton->setSelectable(false);
    _sidebar->footerLayout()->addWidget(settingsButton);
    sidebarLayout->setSidebar(_sidebar);

    QWidget* mainArea = new QWidget(sidebarLayout);
    QVBoxLayout* mainAreaLayout = new QVBoxLayout(mainArea);
    mainAreaLayout->setContentsMargins(0, 0, 0, 0);
    mainAreaLayout->setSpacing(0);
    QWidget* header = new QWidget(mainArea);
    header->setFixedHeight(kHeaderHeight);
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    // 控件四周带焦点环边距，左右边距相应扣除
    headerLayout->setContentsMargins(16 - kFocusRingWidth, 0, 24 - kFocusRingWidth, 0);
    headerLayout->setSpacing(8);
    ShadSidebarTrigger* sidebarTrigger = new ShadSidebarTrigger(_sidebar, header);
    ShadSeparator* headerSeparator = new ShadSeparator(Qt::Vertical, header);
    headerSeparator->setFixedHeight(16);
    _headerTitle = new ShadLabel(header);
    QFont headerFont = _headerTitle->font();
    headerFont.setWeight(QFont::Medium);
    _headerTitle->setFont(headerFont);
    headerLayout->addWidget(sidebarTrigger);
    headerLayout->addWidget(headerSeparator);
    headerLayout->addWidget(_headerTitle);
    headerLayout->addStretch(1);
    _pageStack = new ShadStackedWidget(mainArea);
    mainAreaLayout->addWidget(header);
    mainAreaLayout->addWidget(new ShadSeparator(Qt::Horizontal, mainArea));
    mainAreaLayout->addWidget(_pageStack, 1);
    sidebarLayout->setContentWidget(mainArea);

    QWidget* centralArea = new QWidget(this);
    QVBoxLayout* centralLayout = new QVBoxLayout(centralArea);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralLayout->addWidget(new ShadSeparator(Qt::Horizontal, centralArea));
    centralLayout->addWidget(sidebarLayout, 1);
    setCentralWidget(centralArea);

    // 主窗口的 toaster，需在使用它的页面之前创建
    _toaster = ShadToaster::forWindow(this);

    // 新增页面：创建页面后调用 addPage 加入侧栏；需要多个分组时再调用 _sidebar->addGroup()
    ShadSidebarGroup* mainGroup = _sidebar->addGroup();
    HomePage* homePage = new HomePage(_toaster, _pageStack);
    ShadSidebarMenuButton* homeButton = addPage(mainGroup, ShadIconType::House, homePage);
    addPage(mainGroup, ShadIconType::Info, new AboutPage(_pageStack));

    AppI18n::bind(this, [this, sidebarTrigger, settingsButton]()
    {
        _themeButton->setToolTip(tr("切换明暗主题"));
        sidebarTrigger->setToolTip(tr("切换侧栏（Ctrl+B / Cmd+B）"));
        settingsButton->setText(tr("设置"));
    });

    connect(brandButton, &ShadSidebarMenuButton::clicked, this, [this, homeButton]()
    {
        _sidebar->setActiveButton(homeButton);
        showPage(homeButton);
    });
    connect(settingsButton, &ShadSidebarMenuButton::clicked, this, &MainWindow::openSettings);
    connect(homePage, &HomePage::settingsRequested, this, &MainWindow::openSettings);
    connect(_sidebar, &ShadSidebar::itemActivated, this, &MainWindow::showPage);
    connect(_themeButton, &ShadButton::clicked, this, []()
    {
        shadTheme->setThemeMode(shadTheme->isDark() ? ShadThemeType::Light : ShadThemeType::Dark);
    });
    connect(shadTheme, &ShadTheme::themeChanged, this, &MainWindow::updateThemeButton);

    _sidebar->setActiveButton(homeButton);
    showPage(homeButton);
    updateThemeButton();
}

MainWindow::~MainWindow()
{
}

ShadSidebarMenuButton* MainWindow::addPage(ShadSidebarGroup* group, ShadIconType::IconName icon, BasePage* page)
{
    const int index = _pageStack->addWidget(page);
    ShadSidebarMenuButton* button = group->addItem(icon, page->getTitle());
    _pageIndexes.insert(button, index);
    // 页面标题随语言刷新后同步侧栏按钮与顶栏标题
    connect(page, &BasePage::titleChanged, button, &ShadSidebarMenuButton::setText);
    connect(page, &BasePage::titleChanged, this, &MainWindow::updateHeaderTitle);
    return button;
}

void MainWindow::showPage(ShadSidebarMenuButton* button)
{
    const int index = _pageIndexes.value(button, -1);
    if (index < 0)
    {
        return;
    }
    _pageStack->setCurrentIndex(index);
    updateHeaderTitle();
}

void MainWindow::openSettings()
{
    SettingsDialog* dialog = new SettingsDialog(this);
    // 每次打开都新建，关闭后销毁
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->open();
}

void MainWindow::updateHeaderTitle()
{
    if (BasePage* page = qobject_cast<BasePage*>(_pageStack->currentWidget()))
    {
        _headerTitle->setText(page->getTitle());
    }
}

void MainWindow::updateThemeButton()
{
    _themeButton->setShadIcon(shadTheme->isDark() ? ShadIconType::Sun : ShadIconType::Moon);
}
