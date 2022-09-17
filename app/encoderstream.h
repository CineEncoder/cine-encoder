/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: encoderstream.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef ENCODERSTREAM_H
#define ENCODERSTREAM_H

#include <QObject>
#include <QProcess>
#include <QVector>
#include "constants.h"


#if defined (__linux__)
    #include <unistd.h>
    #include <signal.h>
#elif defined(__WIN64__)
    #include <windows.h>
#endif

using namespace Constants;

class EncoderStream : public QObject
{
    Q_OBJECT
public:
    explicit EncoderStream(QObject *parent = nullptr);
    ~EncoderStream();

    void initEncoding(StreamData *data,
                      EncoderAudioParam *aParam,
                      EncoderSubtParam *sParam);

    QProcess::ProcessState getEncodingState();
    void pauseEncoding();
    void resumeEncoding();
    void stopEncoding();
    void killEncoding();

    signals:
    void onEncodingStarted();
    void onEncodingInitError(const QString &_message);
    void onEncodingProgress(const int percent, const float rem_time);
    void onEncodingLog(const QString &log);
    void onEncodingAborted();
    void onEncodingError(const QString &_error_message, bool popup = false);
    void onEncodingCompleted();

private:
    time_t  m_loop_start;
    QString m_preset_0,
            m_preset,
            m_output_file,
            m_message,
            m_error_message;

    StreamData *m_pData;
    QProcess   *m_pProcessEncoding;

private slots:
    void encode();
    void progress();
    void completed(int);
    void abort();
};

#endif // ENCODERSTREAM_H
