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
#include "tables.h"


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
                      const QString  _hdr[],
                      Data           data,
                      int            *_fr_count,
                      int            streamCutting,
                      const QString  &subtitle_font,
                      int            subtitle_font_size,
                      const QString  &subtitle_font_color,
                      const bool     burn_background,
                      const QString  &subtitle_background_color,
                      int            subtitle_location,
                      int threads
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

    int     *fr_count,
            _threads;

    time_t  _loop_start;

    QStringList _extAudioPaths,
                _extSubPaths;
    QString _temp_file,
            _input_file,
            _output_file,
            _preset_0,
            _preset_mkvmerge,
            _message,
            _error_message,
            _encoding_mode;

    QStringList _preset, _preset_pass1, _sub_mux_param;

    QProcess *processEncoding;

private slots:
    void encode();
    void add_metadata();
    void progress_1();
    void progress_2();
    void completed(int exit_code);
    void abort();

    void
    resizeVF(const QString &_width, const QString &_height, int _CODEC, int _WIDTH, int _HEIGHT, Tables &t,
             QString &resize_vf) const;
    void
    fpsVF(const QString &_fps, int _CODEC, int _FRAME_RATE, int _BLENDING, Tables &t, QString &fps_vf,
          double &fps_dest) const;

    void subtVF(const QString &input_file, const QString &subtitle_font, int subtitle_font_size,
                const QString &subtitle_font_color, const bool burn_background,
                const QString &subtitle_background_color,
                int subtitle_location, Data &data, QString &burn_subt_vf);

    QStringList
    audioModule(const Tables &t, int _CODEC, int _AUDIO_CODEC, int _AUDIO_BITRATE, int _AUDIO_SAMPLING,
                int _AUDIO_CHANNELS) const;

    void passModule(const Tables &t, int _CODEC, int _PASS, QStringList &pass, QStringList &pass1);

    void codecModule(const Tables &t, int _CODEC, QString &hwaccel, QString &hwaccel_filter_vf);

    void split(const double &_startTime, const double &_endTime, const double &_dur, int streamCutting, const Tables &t,
               int _CODEC, double fps_dest, double minExtTime, QStringList &_splitStartParam,
               QStringList &_splitParam) const;

    QString getLog() const;

    void hdrColorRange(const QString _hdr[], int _COLOR_RANGE, QStringList &color_range) const;

    void hdrLum(const QString _hdr[], const QString &_MIN_LUM, const QString &_MAX_LUM, const QString &_MAX_CLL,
                const QString &_MAX_FALL, QStringList &max_lum, QStringList &min_lum, QStringList &max_cll,
                QStringList &max_fall) const;

    void
    hdrDisplay(const QString _hdr[], int _MASTER_DISPLAY, const QString &_CHROMA_COORD,
               const QString &_WHITE_COORD,
               QStringList &chroma_coord, QStringList &white_coord);

    QStringList modeModule(const Tables &t, int _CODEC, int _MODE, const QString &_BQR, const QString &_MINRATE,
                           const QString &_MAXRATE, const QString &_BUFSIZE) const;

    QStringList subModule(const QString &container);

    void colorPrimaries(const QString _hdr[], int _PRIMARY, int _REP_PRIM, QStringList &colorprim,
                        QStringList &colorprim_vf);

    int extAudio(Data &data, QStringList &_audioMapParam, QStringList &_audioMetadataParam, int audioNum);

    void audio(Data &data, QStringList &_audioMapParam, QStringList &_audioMetadataParam, int &audioNum) const;

    QStringList presetModule(const Tables &t, int _CODEC, int _PRESET) const;

    void extSub(Data &data, int extTrackNum, QStringList &_subtitleMapParam, QStringList &_subtitleMetadataParam,
                int subtNum);

    void
    colorTransfer(const QString _hdr[], int _TRC, int _REP_TRC, QStringList &transfer, QStringList &transfer_vf);

    void colorMatrix(const QString hdr[], int _MATRIX, int _REP_MATRIX, QStringList &colormatrix,
                     QStringList &colormatrix_vf);

    QStringList levelModule(const Tables &t, int _CODEC, int _LEVEL) const;

    void subtitles(const QString &input_file, const QString &subtitle_font, int subtitle_font_size,
                   const QString &subtitle_font_color, const bool burn_background,
                   const QString &subtitle_background_color,
                   int subtitle_location, Data &data, QString &burn_subt_vf, QStringList &_subtitleMapParam,
                   QStringList &_subtitleMetadataParam, int &subtNum);

    QStringList getCodec(const Tables &t, int _CODEC, const QString &resize_vf, const QString &fps_vf,
                         const QStringList &_videoMetadataParam, const QStringList &_audioMapParam,
                         const QStringList &_audioMetadataParam, const QString &burn_subt_vf,
                         const QStringList &_subtitleMapParam, const QStringList &_subtitleMetadataParam,
                         const QString &hwaccel_filter_vf, const QStringList &colorprim_vf,
                         const QStringList &colormatrix_vf, const QStringList &transfer_vf) const;

    void getPresets(const QStringList &_splitStartParam, const QStringList &_splitParam, const QString &hwaccel,
                    const QStringList &level, const QStringList &mode, const QStringList &preset,
                    const QStringList &pass,
                    const QStringList &pass1, const QStringList &audio_param, const QStringList &sub_param,
                    const QStringList &colorprim, const QStringList &colormatrix, const QStringList &transfer,
                    const QStringList &codec, const QStringList &color_range, const QStringList &max_lum,
                    const QStringList &min_lum, const QStringList &max_cll, const QStringList &max_fall,
                    const QStringList &chroma_coord, const QStringList &white_coord);

    Data &video(QString &globalTitle, Data &data, QVector<QString> &videoMetadata,
                QStringList &_videoMetadataParam) const;

    void initVariables(const QString &temp_file, const QString &input_file, const QString &output_file,
                       QVector<QString> &_cur_param, int *_fr_count, Tables &t, int &_CODEC, int &_MODE,
                       QString &_BQR, QString &_MINRATE, QString &_MAXRATE, QString &_BUFSIZE, int &_LEVEL,
                       int &_FRAME_RATE, int &_BLENDING, int &_WIDTH, int &_HEIGHT, int &_PASS, int &_PRESET,
                       int &_COLOR_RANGE, int &_MATRIX, int &_PRIMARY, int &_TRC, QString &_MIN_LUM, QString &_MAX_LUM,
                       QString &_MAX_CLL, QString &_MAX_FALL, int &_MASTER_DISPLAY, QString &_CHROMA_COORD,
                       QString &_WHITE_COORD, int &_AUDIO_CODEC, int &_AUDIO_BITRATE, int &_AUDIO_SAMPLING,
                       int &_AUDIO_CHANNELS, int &_REP_PRIM, int &_REP_MATRIX, int &_REP_TRC);
};

#endif // ENCODER_H
