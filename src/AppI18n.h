#ifndef APPI18N_H
#define APPI18N_H

#include <QObject>
#include <QString>

#include <functional>

#include "ShadI18n.h"

// 界面文字绑定：切换语言时原地刷新文字，组件不重建，选中、输入等状态都保留
namespace AppI18n
{
    // 返回当前语言文字的函数，内部调用 tr()
    using TextFunction = std::function<QString()>;

    // 立即执行一次 apply，之后每次切换语言（ShadI18n::languageChanged）再执行一次；owner 销毁后自动断开
    // apply 中用 tr() 设置文字，不要在 apply 中改动用户可编辑的内容
    template <typename Function>
    void bind(QObject* owner, Function apply)
    {
        apply();
        QObject::connect(shadI18n, &ShadI18n::languageChanged, owner, [apply]() mutable { apply(); });
    }
} // namespace AppI18n

#endif // APPI18N_H
