/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: streamconverter.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef STREAMCONVERTER_H
#define STREAMCONVERTER_H

#include <QtGlobal>
#include "encoderstream.h"
#include "constants.h"
#ifdef WM_CUSTOM
    #include "basewindow_cwm.h"
#else
    #ifdef Q_OS_WIN
        #include "platform_win/basewindow.h"
    #else
        #include "platform_unix/basewindow.h"
    #endif
#endif

using namespace Constants;

namespace Ui
{
    class StreamConverter;
}

class StreamConverter : public BaseWindow
{
    Q_OBJECT
public:
    enum class Mode : uchar {
        Extract, Convert
    };
    explicit StreamConverter(QWidget *parent,
                             Mode mode,
                             StreamData *data,
                             int theme);
    ~StreamConverter();

private:
    void onCloseWindow();
    void lockSignals(bool status);
    virtual void showEvent(QShowEvent*) final;
    virtual void closeEvent(QCloseEvent*);
    virtual bool eventFilter(QObject*, QEvent*) final;
    void setWidgetsEnabled(bool);
    void onComboBox_audio_codec_textChanged(const QString&);
    void onComboBox_subt_codec_textChanged(const QString&);
    bool showDialogMessage(const QString &message);
    void showInfoMessage(const QString &message);

    Ui::StreamConverter *ui;
    int m_status_pause_btn,
        m_theme;
    bool  m_windowActivated;
    Mode  m_mode;
    EncoderStream *m_pEncoder;
    StreamData *m_pData;
    EncoderAudioParam m_aParam;
    EncoderSubtParam m_sParam;

private slots:
    void onStart();
    void onStop();
    void initEncoding();
    void onEncodingStarted();
    void onEncodingInitError(const QString &_message);
    void onEncodingProgress(const int percent, const float rem_time);
    void onEncodingLog(const QString &log);
    void onEncodingCompleted();
    void onEncodingAborted();
    void onEncodingError(const QString &error_message, bool popup = false);
    void pause();
    void resume();
};

#endif // STREAMCONVERTER_H
