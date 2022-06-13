/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: preset.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef PRESET_H
#define PRESET_H

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


namespace Ui
{
    class Preset;
}

class Preset : public BaseWindow
{
    Q_OBJECT
public:
    explicit Preset(QWidget *parent, QVector<QString> *pOld_param);
    ~Preset();

private:
    void onCloseWindow();
    void onButtonApply();
    void lockSignals(bool status);
    virtual void showEvent(QShowEvent*) final;
    virtual bool eventFilter(QObject*, QEvent*) final;
    // Transform
    void repeat_handler();
    void change_preset_name();
    void onComboBoxAspectRatio_indexChanged(int);
    void onComboBox_width_textChanged(const QString&);
    void onComboBox_height_textChanged(const QString&);
    void onComboBoxFrameRate_indexChanged(int);
    void calculateDAR(QString, QString);
    // Video
    void disableHDR();
    void onComboBox_codec_textChanged(const QString&);
    void onComboBox_mode_textChanged(const QString&);
    void onComboBox_preset_indexChanged(int);
    void onComboBox_pass_indexChanged(int);
    void onComboBox_container_textChanged();
    void onLineEdit_bitrate_editingFinished();
    // Audio
    void onComboBox_audio_codec_textChanged(const QString&);
    void onComboBox_audio_bitrate_textChanged();
    // Metadata
    void onComboBox_master_disp_textChanged(const QString&);

    Ui::Preset *ui;
    QVector<QString> *m_pNew_param;
    float m_aspectRatio;
    int   m_repeat;
    bool  m_windowActivated;
};

#endif // PRESET_H
