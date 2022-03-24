/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: constants.cpp
 MODIFIED: February, 2022
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef CONSTANTS_H
#define CONSTANTS_H


// ******************* Constants *****************************//

#define PRESETS_VERSION 352

#define SETTINGS_VERSION 352

#define PARAMETERS_COUNT 34

#define NUMBER_PRESETS 30

#define CODEC_QSV_FIRST 6
#define CODEC_QSV_LAST 11
#define CODEC_VAAPI_FIRST 12
#define CODEC_VAAPI_LAST 12

#define DOCKS_COUNT 8

#define AMOUNT_AUDIO_STREAMS 9

#define AMOUNT_SUBTITLES 9

#define MAXIMUM_ALLOWED_TIME 359999.0f

// ***************** Table Widget Columns *********************//

enum ColumnIndex {
    FILENAME,       FORMAT,         RESOLUTION,     DURATION,           FPS,
    AR,             STATUS,         BITRATE,        SUBSAMPLING,        BITDEPTH,
    COLORSPACE,     COLORRANGE,     COLORPRIM,      COLORMATRIX,        TRANSFER,
    MAXLUM,         MINLUM,         MAXCLL,         MAXFALL,            MASTERDISPLAY,
    PATH,           T_DUR,          T_CHROMACOORD,  T_WHITECOORD,       T_STREAMSIZE,
    T_WIDTH,        T_HEIGHT,       T_VIDEOTITLE,   T_VIDEOMOVIENAME,   T_VIDEOYEAR,
    T_VIDEOAUTHOR,  T_VIDEOPERF,    T_VIDEODESCR,   T_AUDIO_1,          T_AUDIO_2,
    T_AUDIO_3,      T_AUDIO_4,      T_AUDIO_5,      T_AUDIO_6,          T_AUDIO_7,
    T_AUDIO_8,      T_AUDIO_9,      T_AUDIOLANG_1,  T_AUDIOLANG_2,      T_AUDIOLANG_3,
    T_AUDIOLANG_4,  T_AUDIOLANG_5,  T_AUDIOLANG_6,  T_AUDIOLANG_7,      T_AUDIOLANG_8,
    T_AUDIOLANG_9,  T_AUDIOTITLE_1, T_AUDIOTITLE_2, T_AUDIOTITLE_3,     T_AUDIOTITLE_4,
    T_AUDIOTITLE_5, T_AUDIOTITLE_6, T_AUDIOTITLE_7, T_AUDIOTITLE_8,     T_AUDIOTITLE_9,
    T_AUDIOCHECK_1, T_AUDIOCHECK_2, T_AUDIOCHECK_3, T_AUDIOCHECK_4,     T_AUDIOCHECK_5,
    T_AUDIOCHECK_6, T_AUDIOCHECK_7, T_AUDIOCHECK_8, T_AUDIOCHECK_9,     T_SUBTITLE_1,
    T_SUBTITLE_2,   T_SUBTITLE_3,   T_SUBTITLE_4,   T_SUBTITLE_5,       T_SUBTITLE_6,
    T_SUBTITLE_7,   T_SUBTITLE_8,   T_SUBTITLE_9,   T_SUBLANG_1,        T_SUBLANG_2,
    T_SUBLANG_3,    T_SUBLANG_4,    T_SUBLANG_5,    T_SUBLANG_6,        T_SUBLANG_7,
    T_SUBLANG_8,    T_SUBLANG_9,    T_TITLESUB_1,   T_TITLESUB_2,       T_TITLESUB_3,
    T_TITLESUB_4,   T_TITLESUB_5,   T_TITLESUB_6,   T_TITLESUB_7,       T_TITLESUB_8,
    T_TITLESUB_9,   T_SUBCHECK_1,   T_SUBCHECK_2,   T_SUBCHECK_3,       T_SUBCHECK_4,
    T_SUBCHECK_5,   T_SUBCHECK_6,   T_SUBCHECK_7,   T_SUBCHECK_8,       T_SUBCHECK_9,
    T_STARTTIME,    T_ENDTIME
};

// **************** Geometry and Themes *********************//

enum DockIndex {
    PRESETS_DOCK, PREVIEW_DOCK, SOURCE_DOCK, OUTPUT_DOCK,
    STREAMS_DOCK, LOG_DOCK, METADATA_DOCK, SPLIT_DOCK
};

enum Resize {
    LEFT,   LEFT_TOP,   LEFT_BOTTOM,   TOP,
    RIGHT,  RIGHT_TOP,  RIGHT_BOTTOM,  BOTTOM
};

enum Theme {
    GRAY, DARK, WAVE, DEFAULT
};

enum PreviewDest {
    PREVIEW = 1, SPLITTER = 2
};

enum PreviewRes {
    RES_LOW, RES_HIGH
};

// ******************** Metadata *****************************//

enum VideoMetadataIndex {
    VIDEO_TITLE,        VIDEO_AUTHOR,       VIDEO_YEAR,
    VIDEO_PERFORMER,    VIDEO_DESCRIPTION,  VIDEO_MOVIENAME, ENUM_END_LABEL
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

enum EncodingStatus {
    START, PAUSE, RESUME
};


#endif // CONSTANTS_H
