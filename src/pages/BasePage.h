#ifndef BASEPAGE_H
#define BASEPAGE_H

#include <QString>

#include "AppI18n.h"
#include "ShadScrollArea.h"

class QVBoxLayout;
class QWidget;
class ShadCard;

// 页面基类：标题 + 说明 + 若干卡片，内容超出时可滚动
// 标题与说明以返回文字的函数传入（函数内调用 tr()），切换语言时由基类重新调用并刷新
class BasePage : public ShadScrollArea
{
    Q_OBJECT

    public:
        BasePage(AppI18n::TextFunction title, AppI18n::TextFunction description, QWidget* parent = nullptr);
        ~BasePage() override;

        // 当前语言的页面标题
        QString getTitle() const;

    Q_SIGNALS:
        // 切换语言后标题变化时发出，主窗口据此刷新侧栏按钮与顶栏标题
        void titleChanged(const QString& title);

    protected:
        // 添加一张卡片，内容放入返回卡片的 contentLayout()；标题与说明随语言刷新
        ShadCard* addCard(AppI18n::TextFunction title, AppI18n::TextFunction description);

    private:
        QString _title;
        QVBoxLayout* _pageLayout{nullptr};
};

#endif // BASEPAGE_H
