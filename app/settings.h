/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: settings.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QtGlobal>
#ifdef Q_OS_WIN
    #include "platform_win/basewindow.h"
#else
    #include "platform_unix/basewindow.h"
#endif


namespace Ui
{
    class Settings;
}

class Settings : public BaseWindow
{
    Q_OBJECT
public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    void setParameters(QString    *ptr_output_folder,
                       QString    *ptr_temp_folder,
                       bool       *ptr_protection,
                       bool       *ptr_showHDR_mode,
                       int        *ptr_timer_interval,
                       int        *ptr_theme,
                       QString    *ptr_prefixName,
                       QString    *ptr_suffixName,
                       int        *ptr_prefxType,
                       int        *ptr_suffixType,
                       bool       *ptr_hideInTrayFlag,
                       QString    *ptr_language,
                       const QString &_desktopEnv,
                       int        *ptr_fontSize,
                       QString    *ptr_font);

private:
    void onCloseWindow();
    void onButtonApply();
    void onButtonReset();
    void onButtonTab_1();
    void onButtonTab_2();
    virtual bool eventFilter(QObject*, QEvent*) final;

    QString callFileDialog(const QString&);
    void onButtonOutputPath();
    void onButtonTempPath();
    void onCheckBox_protection_clicked();
    void onComboBoxPrefixType_indexChanged(int);
    void onComboBoxSuffixType_indexChanged(int);
    void onComboBox_font_indexChanged(int);

    Ui::Settings *ui;
    QString desktopEnv;

    QString *_ptr_output_folder,
            *_ptr_temp_folder,
            *_ptr_prefixName,
            *_ptr_suffixName,
            *_ptr_language,
            *_ptr_font;

    int     *_ptr_fontSize,
            *_ptr_prefxType,
            *_ptr_suffixType,
            *_ptr_timer_interval,
            *_ptr_theme;

    bool    *_ptr_showHDR_mode,
            *_ptr_protection,
            *_ptr_hideInTrayFlag;
};

#endif // SETTINGS_H
