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
#ifdef WM_CUSTOM
    #include "basewindow_cwm.h"
#else
    #ifdef Q_OS_WIN
        #include "platform_win/basewindow.h"
    #else
        #include "platform_unix/basewindow.h"
    #endif
#endif
#include "helper.h"


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

    void setParameters(QString    *pOutputFolder,
                       QString    *pTempFolder,
                       bool       *pProtectFlag,
                       bool       *pMultiInstances,
                       bool       *pShowHdrFlag,
                       int        *pTimerInterval,
                       int        *pTheme,
                       QString    *pPrefixName,
                       QString    *pSuffixName,
                       int        *pPrefxType,
                       int        *pSuffixType,
                       bool       *pHideInTrayFlag,
                       QString    *pLanguage,
                       int        *pFontSize,
                       QString    *pFont,
                       int        *pSubtitlesFontSize,
                       QString    *pSubtitlesFont,
                       bool       *pSubtitlesBackground);

private:
    void onCloseWindow();
    void onButtonApply();
    void onButtonReset();
    void showEvent(QShowEvent*);
    virtual bool eventFilter(QObject*, QEvent*) final;

    void onButtonOutputPath();
    void onButtonTempPath();
    void onCheckBoxProtectFlag_clicked();
    void onComboBoxPrefixType_indexChanged(int);
    void onComboBoxSuffixType_indexChanged(int);
    void onComboBoxFont_indexChanged(int);
    void onComboBoxSubtitlesFont_indexChanged(int);

    Ui::Settings *ui;

    QString *m_pOutputFolder,
            *m_pTempFolder,
            *m_pPrefixName,
            *m_pSuffixName,
            *m_pLanguage,
            *m_pFont,
            *m_pSubtitlesFont;

    int     *m_pFontSize,
            *m_pSubtitlesFontSize,
            *m_pPrefxType,
            *m_pSuffixType,
            *m_pTimerInterval,
            *m_pTheme;

    bool    *m_pShowHdrFlag,
            *m_pProtectFlag,
            *m_pMultiInstances,
            *m_pHideInTrayFlag,
            m_windowActivated,
            *m_pSubtitlesBackground;
};

#endif // SETTINGS_H
