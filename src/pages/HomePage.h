#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "BasePage.h"

class QWidget;
class ShadToaster;

// 首页：新增页面的步骤与组件示例
class HomePage : public BasePage
{
    Q_OBJECT

    public:
        explicit HomePage(ShadToaster* toaster, QWidget* parent = nullptr);
        ~HomePage() override;

    Q_SIGNALS:
        // 点击"打开设置"时发出，由主窗口打开设置对话框
        void settingsRequested();
};

#endif // HOMEPAGE_H
