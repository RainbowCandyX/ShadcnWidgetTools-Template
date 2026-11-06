#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "ShadDialog.h"

class QWidget;

// 设置：语言、主题、基色与圆角，修改立即生效，不保存
class SettingsDialog : public ShadDialog
{
    Q_OBJECT

    public:
        explicit SettingsDialog(QWidget* parent = nullptr);
        ~SettingsDialog() override;
};

#endif // SETTINGSDIALOG_H
