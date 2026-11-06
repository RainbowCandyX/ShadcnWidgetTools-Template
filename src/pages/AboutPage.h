#ifndef ABOUTPAGE_H
#define ABOUTPAGE_H

#include "BasePage.h"

class QWidget;

// 关于页：应用与 Qt 版本、相关链接
class AboutPage : public BasePage
{
    Q_OBJECT

    public:
        explicit AboutPage(QWidget* parent = nullptr);
        ~AboutPage() override;
};

#endif // ABOUTPAGE_H
