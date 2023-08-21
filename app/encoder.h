/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: encoder.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef ENCODER_H
#define ENCODER_H

#include <QObject>
#include <QProcess>
#include <QVector>
#include "constants.h"


#if defined (Q_OS_UNIX)
    #include <unistd.h>
    #include <signal.h>
#elif defined(Q_OS_WIN64)
    #include <windows.h>
#endif

using namespace Constants;

class Encoder : public QObject
{
    Q_OBJECT
public:
    explicit Encoder(QObject *parent = nullptr);
    ~Encoder();

    void initEncoding(const QString  &temp_file,
                      const QString  &input_file,
                      const QString  &output_file,
                      const QString  &_width,
                      const QString  &_height,
                      const QString  &_fps,
                      const double   &_startTime,
                      const double   &_endTime,
                      const double   &_dur,
                      const QString  &container,
                      QString        globalTitle,
                      QVector<QString>  _cur_param,
                      const QString  (&_hdr)[11],
                      Data           data,
                      int            *_fr_count,
                      int            streamCutting,
                      const QString &subtitle_font,
                      int subtitle_font_size,
                      const QString &subtitle_font_color,
                      const QString &subtitle_background_color
                      );

    QProcess::ProcessState getEncodingState();
    void pauseEncoding();
    void resumeEncoding();
    void stopEncoding();
    void killEncoding();

    signals:
    void onEncodingMode(const QString &mode);
    void onEncodingStarted();
    void onEncodingInitError(const QString &_message);
    void onEncodingProgress(const int percent, const float rem_time);
    void onEncodingLog(const QString &log);
    void onEncodingAborted();
    void onEncodingError(const QString &_error_message, bool popup = false);
    void onEncodingCompleted();

private:
    bool    _flag_two_pass,
            _flag_hdr,
            _burn_subtitle,
            _mux_mode;

    int     *fr_count;

    time_t  _loop_start;

    QStringList _extAudioPaths,
                _extSubPaths;
    QString _temp_file,
            _input_file,
            _output_file,
            _preset_0,
            _preset_pass1,
            _preset,
            _preset_mkvmerge,
            _sub_mux_param,
            _message,
            _error_message,
            _encoding_mode;

    QProcess *processEncoding;

private slots:
    void encode();
    void add_metadata();
    void progress_1();
    void progress_2();
    void completed(int);
    void abort();
};

#endif // ENCODER_H
