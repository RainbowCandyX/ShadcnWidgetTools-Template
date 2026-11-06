#include <QApplication>
#include <QFont>
#include <QLocale>
#include <QString>
#include <QStringList>
#include <QtGlobal>

#include "MainWindow.h"
#include "ShadI18n.h"
#include "ShadTooltip.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    // APP_NAME 与 APP_VERSION 由 CMakeLists.txt 定义
    QApplication::setApplicationName(QStringLiteral(APP_NAME));
    QApplication::setApplicationVersion(QStringLiteral(APP_VERSION));
#ifdef Q_OS_WIN
    // 西文使用 Segoe UI，中文回退到微软雅黑
    QFont appFont = app.font();
    appFont.setFamilies(QStringList{QStringLiteral("Segoe UI"), QStringLiteral("Microsoft YaHei UI")});
    app.setFont(appFont);
#endif
    // 界面原文为简体中文：日历的月名、星期名与日期格式默认使用简体中文；需在创建组件之前设置，切换语言时由 ShadI18n 更新
    QLocale::setDefault(QLocale(QLocale::Chinese, QLocale::China));
    // 应用自己的翻译由 qt_add_translations 嵌入资源 :/i18n（app_<语言>.qm）
    shadI18n->addTranslations(QStringLiteral(":/i18n"), QStringLiteral("app"));
    // 所有组件的 toolTip 以 shadcn 样式显示
    shadTooltip->install();
    MainWindow window;
    window.show();
    return app.exec();
}
