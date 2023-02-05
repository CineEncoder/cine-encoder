/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: constants.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QSettings>
#include <QDir>
#include <QVector>
#include <iostream>

#define Print(a) std::cout << a << std::endl
#define numToStr(num) QString::number(num)
#define Q_LOOP(i, start, end) for(int i = start; i < end; i++)
#define CHECKS(chk) data.checks[Data::chk]
#define FIELDS(fld) data.fields[Data::fld]
#define SETTINGSPATH QDir::homePath() + QString("/CineEncoder")
#define SETTINGSFILE SETTINGSPATH + QString("/settings.ini")
#define SETTINGS(settings) QSettings settings(SETTINGSFILE, \
                            QSettings::IniFormat);

// ******************* Constants *****************************//
#define PRESETS_VERSION 354
#define SETTINGS_VERSION 354

#define PARAMETERS_COUNT 34
#define NUMBER_PRESETS 31

#define CODEC_QSV_FIRST 6
#define CODEC_QSV_LAST 11
#define CODEC_VAAPI_FIRST 12
#define CODEC_VAAPI_LAST 12

#define DEFAULTPREFIX QString("output")
#define DEFAULTSUFFIX QString("_encoded_")

#define DOCKS_COUNT 8

#define AMOUNT_HDR_PARAMS 11
#define AMOUNT_VIDEO_METADATA 6
#define MAX_AUDIO_STREAMS 100
#define MAX_SUBTITLES 100

#define MAXIMUM_ALLOWED_TIME 359999.0f
#define FONTSIZE 8


namespace Constants {
    // ***************** Table Widget Columns *********************//
    enum ColumnIndex {
        FILENAME,   FORMAT,     RESOLUTION,    DURATION,     FPS,
        AR,         STATUS,     BITRATE,       SUBSAMPLING,  BITDEPTH,
        COLORSPACE, COLORRANGE, COLORPRIM,     COLORMATRIX,  TRANSFER,
        MAXLUM,     MINLUM,     MAXCLL,        MAXFALL,      MASTERDISPLAY,
        PATH,       T_DUR,      T_CHROMACOORD, T_WHITECOORD, T_STREAMSIZE,
        T_WIDTH,    T_HEIGHT,   T_STARTTIME,   T_ENDTIME,    T_ID
    };

    // **************** Geometry and Themes *********************//
    enum DockIndex {
        SOURCE_DOCK, PRESETS_DOCK,  OUTPUT_DOCK, STREAMS_DOCK,
        LOG_DOCK,    METADATA_DOCK, SPLIT_DOCK,  BROWSER_DOCK
    };

    enum Theme {
        LIGHT, DARK, WAVE, GRAY
    };

    enum PreviewDest {
        PREVIEW = 1, SPLITTER = 2
    };

    enum PreviewRes {
        RES_LOW, RES_HIGH
    };

    // ******************* Stream Data ***************************//
    enum class ContentType : uchar {
        Audio, Subtitle
    };

    struct StreamData {
        ContentType cont_type;
        QString input_file;
        QString output_file;
        float duration;
        int stream;
        StreamData() :
            cont_type(ContentType::Audio),
            input_file(QString()),
            output_file(QString()),
            duration(0.f),
            stream(-1)
        {}
    };

    // ********************* Report ******************************//
    struct ReportLog {
        enum class Icon : uchar {
            Info, Warning, Critical
        };
        Icon    reportIcon;
        QString reportTime,
                reportMsg;
        ReportLog() :
            reportIcon(Icon::Info),
            reportTime(QString()),
            reportMsg(QString())
        {}
    };

    // ******************** Metadata *****************************//
    struct Data {
        enum StreamCheck {
            audioChecks, externAudioChecks, subtChecks, externSubtChecks,
            audioDef,    externAudioDef,    subtDef,    externSubtDef
        };
        enum StreamField {
            audioFormats, audioChannels, audioChLayouts, audioDuration,
            audioLangs,   audioTitles,

            externAudioFormats,  externAudioChannels, externAudioChLayouts,
            externAudioDuration, externAudioLangs,    externAudioTitles,
            externAudioPath,

            subtFormats, subtDuration, subtLangs, subtTitles,

            externSubtFormats, externSubtDuration, externSubtLangs,
            externSubtTitles,  externSubtPath
        };

        static const int CHECKS_COUNT = 8;
        static const int FIELDS_COUNT = 22;

        QVector<QString> videoMetadata;
        QVector<bool>    checks[CHECKS_COUNT];
        QVector<QString> fields[FIELDS_COUNT];
        void clear() {
            videoMetadata.clear();
            Q_LOOP(i, 0, CHECKS_COUNT)
                checks[i].clear();
            Q_LOOP(i, 0, FIELDS_COUNT)
                fields[i].clear();
        }
    };

    enum VideoMetadataIndex {
        VIDEO_TITLE,  VIDEO_MOVIENAME, VIDEO_YEAR,
        VIDEO_AUTHOR, VIDEO_PERFORMER, VIDEO_DESCRIPTION
    };

    // ***************** Encoder Data **************************//
    enum CurParamIndex {
        OUTPUT_PARAM,   CODEC,          MODE,
        CONTAINER,      BQR,            MAXRATE,
        BUFSIZE,        FRAME_RATE,     BLENDING,
        WIDTH,          HEIGHT,         PASS,
        PRESET,         COLOR_RANGE,    MIN_LUM,
        MAX_LUM,        MAX_CLL,        MAX_FALL,
        MASTER_DISPLAY, CHROMA_COORD,   WHITE_COORD,
        AUDIO_CODEC,    AUDIO_BITRATE,  MINRATE,
        LEVEL,          ASAMPLE_RATE,   ACHANNELS,
        MATRIX,         PRIMARY,        TRC,
        PRESET_NAME,    REP_PRIM,       REP_MATRIX,
        REP_TRC
    };

    struct EncoderAudioParam {
        int AUDIO_CODEC;
        int AUDIO_BITRATE;
        int AUDIO_SAMPLING;
        int AUDIO_CHANNELS;
        QString AUDIO_CONTAINER;
    };

    struct EncoderSubtParam {
        int SUBT_CODEC;
        QString SUBT_CONTAINER;
    };

    // ******************* Presets *****************************//
    enum Profile {
        HIGH,           MAIN,           MAIN10,
        MAIN12,         PROFILE_0,      PROFILE_1,
        PROFILE_2,      PROFILE_3,      PROFILE_4,
        PROFILE_5,      DNXHR_LB,       DNXHR_SQ,
        DNXHR_HQ,       DNXHR_HQX,      DNXHR_444,
        PROFILE_AUTO
    };
    enum Pixformat {
        YUV420P12LE,    YUV444P10LE,    YUV422P10LE,
        YUV420P10LE,    YUV422p,        YUV420P,
        P010LE,         PIXFORMAT_AUTO
    };

    enum CurHDRIndex {
        CUR_COLOR_RANGE,    CUR_COLOR_PRIMARY,  CUR_COLOR_MATRIX,
        CUR_TRANSFER,       CUR_MAX_LUM,        CUR_MIN_LUM,
        CUR_MAX_CLL,        CUR_MAX_FALL,       CUR_MASTER_DISPLAY,
        CUR_CHROMA_COORD,   CUR_WHITE_COORD
    };

    enum MasterDisplay {
        SOURCE, DISPLAY_P3, DCI_P3, BT_2020, BT_709, CUSTOM
    };   

    enum EncodingStatus {
        START, PAUSE, RESUME
    };
}

#endif // CONSTANTS_H
