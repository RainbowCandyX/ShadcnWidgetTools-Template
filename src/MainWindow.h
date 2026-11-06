#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QHash>

#include "ShadIconType.h"
#include "ShadWindow.h"

class BasePage;
class QWidget;
class ShadButton;
class ShadLabel;
class ShadSidebar;
class ShadSidebarGroup;
class ShadSidebarMenuButton;
class ShadStackedWidget;
class ShadToaster;

// 主窗口：左侧侧栏导航，右侧为顶栏与页面
class MainWindow : public ShadWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget* parent = nullptr);
        ~MainWindow() override;

    private:
        // 把页面加入页面栈，并在侧栏分组中添加对应按钮
        ShadSidebarMenuButton* addPage(ShadSidebarGroup* group, ShadIconType::IconName icon, BasePage* page);
        void showPage(ShadSidebarMenuButton* button);
        void openSettings();
        // 按当前页面刷新顶栏标题，切换语言后也调用
        void updateHeaderTitle();
        void updateThemeButton();

        ShadSidebar* _sidebar{nullptr};
        ShadStackedWidget* _pageStack{nullptr};
        ShadLabel* _headerTitle{nullptr};
        ShadButton* _themeButton{nullptr};
        ShadToaster* _toaster{nullptr};
        QHash<ShadSidebarMenuButton*, int> _pageIndexes;
};

#endif // MAINWINDOW_H
