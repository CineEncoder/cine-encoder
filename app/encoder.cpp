/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: encoder.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "encoder.h"
#include "tables.h"
#include <QDir>
#include <QMap>
#include <iostream>
#include <math.h>

#define rnd(num) static_cast<int>(round(num))


Encoder::Encoder(QObject *parent) :
    QObject(parent)
{
    processEncoding = new QProcess(this);
    processEncoding->setProcessChannelMode(QProcess::MergedChannels);
    processEncoding->setWorkingDirectory(QDir::homePath());
}

Encoder::~Encoder()
{

}

/************************************************
** Encoder
************************************************/

void Encoder::initEncoding(const QString  &temp_file,
                           const QString  &input_file,
                           const QString  &output_file,
                           const QString  &_width,
                           const QString  &_height,
                           const QString  &_fps,
                           const double   &_startTime,
                           const double   &_endTime,
                           const double   &_dur,
                           const QString  &container,
                           QString  globalTitle,
                           QVector<QString> _cur_param,
                           QString  *_hdr,
                           QString  *_videoMetadata,
                           QString  *_audioLang,
                           QString  *_audioTitle,
                           QString  *_subtitleLang,
                           QString  *_subtitleTitle,
                           int      *_audioStreamCheckState,
                           int      *_subtitleCheckState,
                           int      *_fr_count)
{
    Dump("Make preset...");
    Tables t;
    _temp_file = temp_file;
    _input_file = input_file;
    _output_file = output_file;
    fr_count = _fr_count;
    int _CODEC = _cur_param[CurParamIndex::CODEC].toInt();
    int _MODE = _cur_param[CurParamIndex::MODE].toInt();
    //int _CONTAINER = _cur_param[CurParamIndex::CONTAINER].toInt();
    QString _BQR = _cur_param[CurParamIndex::BQR];
    QString _MINRATE = _cur_param[CurParamIndex::MINRATE];
    QString _MAXRATE = _cur_param[CurParamIndex::MAXRATE];
    QString _BUFSIZE = _cur_param[CurParamIndex::BUFSIZE];
    int _LEVEL = _cur_param[CurParamIndex::LEVEL].toInt();
    int _FRAME_RATE = _cur_param[CurParamIndex::FRAME_RATE].toInt();
    int _BLENDING = _cur_param[CurParamIndex::BLENDING].toInt();
    int _WIDTH = _cur_param[CurParamIndex::WIDTH].toInt();
    int _HEIGHT = _cur_param[CurParamIndex::HEIGHT].toInt();
    int _PASS = _cur_param[CurParamIndex::PASS].toInt();
    int _PRESET = _cur_param[CurParamIndex::PRESET].toInt();
    int _COLOR_RANGE = _cur_param[CurParamIndex::COLOR_RANGE].toInt();
    int _MATRIX = _cur_param[CurParamIndex::MATRIX].toInt();
    int _PRIMARY = _cur_param[CurParamIndex::PRIMARY].toInt();
    int _TRC = _cur_param[CurParamIndex::TRC].toInt();
    QString _MIN_LUM = _cur_param[CurParamIndex::MIN_LUM].replace(",", ".");
    QString _MAX_LUM = _cur_param[CurParamIndex::MAX_LUM].replace(",", ".");
    QString _MAX_CLL = _cur_param[CurParamIndex::MAX_CLL].replace(",", ".");
    QString _MAX_FALL = _cur_param[CurParamIndex::MAX_FALL].replace(",", ".");
    int _MASTER_DISPLAY = _cur_param[CurParamIndex::MASTER_DISPLAY].toInt();
    QString _CHROMA_COORD = _cur_param[CurParamIndex::CHROMA_COORD];
    QString _WHITE_COORD = _cur_param[CurParamIndex::WHITE_COORD];
    int _AUDIO_CODEC = _cur_param[CurParamIndex::AUDIO_CODEC].toInt();
    int _AUDIO_BITRATE = _cur_param[CurParamIndex::AUDIO_BITRATE].toInt();
    int _AUDIO_SAMPLING = _cur_param[CurParamIndex::ASAMPLE_RATE].toInt();
    int _AUDIO_CHANNELS = _cur_param[CurParamIndex::ACHANNELS].toInt();
    int _REP_PRIM = _cur_param[CurParamIndex::REP_PRIM].toInt();
    int _REP_MATRIX = _cur_param[CurParamIndex::REP_MATRIX].toInt();
    int _REP_TRC = _cur_param[CurParamIndex::REP_TRC].toInt();

    _preset_0 = "";
    _preset_pass1 = "";
    _preset = "";
    _preset_mkvmerge = "";
    _sub_mux_param = "";
    _error_message = "";
    _flag_two_pass = false;
    _flag_hdr = false;
    _mux_mode = false;
    *fr_count = 0;

    /****************************************** Resize ****************************************/   
    QString resize_vf = "";
    const QString new_width = (t.arr_width[_WIDTH] != "Source") ? t.arr_width[_WIDTH] : _width;
    const QString new_height = (t.arr_height[_HEIGHT] != "Source") ? t.arr_height[_HEIGHT] : _height;
    if ((t.arr_width[_WIDTH] != "Source") || (t.arr_height[_HEIGHT] != "Source")) {
        if (_CODEC >= CODEC_QSV_FIRST && _CODEC <= CODEC_QSV_LAST) { // QSV
            resize_vf = QString("scale_qsv=w=%1:h=%2,setsar=1:1").arg(new_width, new_height);
        }
        else
        if (_CODEC >= CODEC_VAAPI_FIRST && _CODEC <= CODEC_VAAPI_LAST) { // VAAPI
            resize_vf = QString("scale_vaapi=w=%1:h=%2,setsar=1:1").arg(new_width, new_height);
        }
        else {
            resize_vf = QString("scale=%1:%2,setsar=1:1").arg(new_width, new_height);
        }
    }

    /******************************************* FPS *****************************************/

    QString fps_vf = "";
    double fps_dest;

    if (t.frame_rate[_FRAME_RATE] != "Source") {
        fps_dest = t.frame_rate[_FRAME_RATE].toDouble();
        if (t.blending[_BLENDING] == "Simple") {
            if (_CODEC >= CODEC_QSV_FIRST && _CODEC <= CODEC_QSV_LAST) { // QSV
                fps_vf = QString("vpp_qsv=framerate=%1").arg(t.frame_rate[_FRAME_RATE]);
            }
            else
            if (_CODEC >= CODEC_VAAPI_FIRST && _CODEC <= CODEC_VAAPI_LAST) { // VAAPI
                fps_vf = QString("fps=fps=%1").arg(t.frame_rate[_FRAME_RATE]);
            }
            else {
                fps_vf = QString("fps=fps=%1").arg(t.frame_rate[_FRAME_RATE]);
            }
        }
        else
        if (t.blending[_BLENDING] == "Interpolated") {
            fps_vf = QString("framerate=fps=%1").arg(t.frame_rate[_FRAME_RATE]);
        }
        else
        if (t.blending[_BLENDING] == "MCI") {
            fps_vf = QString("minterpolate=fps=%1:mi_mode=mci:mc_mode=aobmc:me_mode=bidir:vsbmc=1")
                    .arg(t.frame_rate[_FRAME_RATE]);
        }
        else
        if (t.blending[_BLENDING] == "Blend") {
            fps_vf = QString("minterpolate=fps=%1:mi_mode=blend").arg(t.frame_rate[_FRAME_RATE]);
        }
    } else {
        fps_dest = _fps.toDouble();
    }

    /****************************************** Split ****************************************/

    QString _splitStartParam = "";
    QString _splitParam = "";

    if (_endTime > 0 && _startTime < _endTime) {
        double duration = _endTime - _startTime;
        *fr_count = rnd(duration * fps_dest);
        int startFrame = rnd(_startTime * fps_dest);
        int endFrame = rnd(_endTime * fps_dest);
        int amountFrames = endFrame - startFrame;
        _splitStartParam = QString(" -ss %1").arg(QString::number(_startTime, 'f', 3));
        _splitParam = QString("-vframes %1 ").arg(QString::number(amountFrames));
    } else {
        *fr_count = rnd(_dur * fps_dest);
    }

    /************************************** Video metadata ************************************/

    QString videoMetadata[6] = {"", "", "", "", "", ""};
    QString _videoMetadataParam = "";

    if (globalTitle != "") {
        videoMetadata[0] = QString("-metadata:s:v:0 title=%1 ").arg(globalTitle.replace(" ", "\u00A0"));
    } else {
        if (_videoMetadata[VIDEO_TITLE] != "") {
            videoMetadata[0] = QString("-metadata:s:v:0 title=%1 ").arg(_videoMetadata[VIDEO_TITLE]
                                                                        .replace(" ", "\u00A0"));
        } else {
            videoMetadata[0] = QString("-map_metadata:s:v:0 -1 ");
        }
    }
    if (_videoMetadata[VIDEO_MOVIENAME] != "") {
        videoMetadata[1] = QString("-metadata title=%1 ").arg(_videoMetadata[VIDEO_MOVIENAME]
                                                              .replace(" ", "\u00A0"));
    }
    if (_videoMetadata[VIDEO_AUTHOR] != "") {
        videoMetadata[2] = QString("-metadata author=%1 ").arg(_videoMetadata[VIDEO_AUTHOR]
                                                               .replace(" ", "\u00A0"));
    }
    if (_videoMetadata[VIDEO_DESCRIPTION] != "") {
        videoMetadata[3] = QString("-metadata description=%1 ").arg(_videoMetadata[VIDEO_DESCRIPTION]
                                                                    .replace(" ", "\u00A0"));
    }
    if (_videoMetadata[VIDEO_YEAR] != "") {
        videoMetadata[4] = QString("-metadata year=%1 ").arg(_videoMetadata[VIDEO_YEAR].replace(" ", ""));
    }
    if (_videoMetadata[VIDEO_PERFORMER] != "") {
        videoMetadata[5] = QString("-metadata author=%1 ").arg(_videoMetadata[VIDEO_PERFORMER]
                                                               .replace(" ", "\u00A0"));
    }
    for (int i = 0; i < 6; i++) {
        _videoMetadataParam += videoMetadata[i];
    }

    /************************************** Audio streams ************************************/

    QString audioLang[AMOUNT_AUDIO_STREAMS] = {"", "", "", "", "", "", "", "", ""};
    QString audioTitle[AMOUNT_AUDIO_STREAMS] = {"", "", "", "", "", "", "", "", ""};
    QString audioMap[AMOUNT_AUDIO_STREAMS] = {"", "", "", "", "", "", "", "", ""};
    QString _audioMapParam = "";
    QString _audioMetadataParam = "";
    int countDestAudioStream = 0;

    for (int k = 0; k < AMOUNT_AUDIO_STREAMS; k++) {
        if (_audioStreamCheckState[k] == 1) {
            audioMap[k] = QString("-map 0:a:%1? ").arg(QString::number(k));
            audioLang[k] = QString("-metadata:s:a:%1 language=%2 ")
                           .arg(QString::number(countDestAudioStream), _audioLang[k].replace(" ", "\u00A0"));
            audioTitle[k] = QString("-metadata:s:a:%1 title=%2 ")
                            .arg(QString::number(countDestAudioStream), _audioTitle[k].replace(" ", "\u00A0"));
            countDestAudioStream++;
        }
        _audioMapParam += audioMap[k];
        _audioMetadataParam += audioLang[k] + audioTitle[k];
    }

    /**************************************** Subtitles **************************************/

    QString subtitleLang[AMOUNT_SUBTITLES] = {"", "", "", "", "", "", "", "", ""};
    QString subtitleTitle[AMOUNT_SUBTITLES] = {"", "", "", "", "", "", "", "", ""};
    QString subtitleMap[AMOUNT_SUBTITLES] = {"", "", "", "", "", "", "", "", ""};
    QString _subtitleMapParam = "";
    QString _subtitleMetadataParam = "";
    int countDestSubtitleStream = 0;

    for (int k = 0; k < AMOUNT_SUBTITLES; k++) {
        if (_subtitleCheckState[k] == 1) {
            subtitleMap[k] = QString("-map 0:s:%1? ").arg(QString::number(k));
            subtitleLang[k] = QString("-metadata:s:s:%1 language=%2 ")
                           .arg(QString::number(countDestSubtitleStream), _subtitleLang[k].replace(" ", "\u00A0"));
            subtitleTitle[k] = QString("-metadata:s:s:%1 title=%2 ")
                            .arg(QString::number(countDestSubtitleStream), _subtitleTitle[k].replace(" ", "\u00A0"));
            countDestSubtitleStream++;
        }
        _subtitleMapParam += subtitleMap[k];
        _subtitleMetadataParam += subtitleLang[k] + subtitleTitle[k];
    }

    /************************************* Codec module ***************************************/

    const QString hwaccel = t.arr_params[_CODEC][1];
    const QString hwaccel_filter_vf = t.arr_params[_CODEC][3];
    _flag_hdr = static_cast<bool>(t.arr_params[_CODEC][2].toInt());

    /************************************* Level module **************************************/

    const QString selected_level = t.arr_level[_CODEC][_LEVEL];
    const QString level = (selected_level != "" && selected_level != tr("Auto")) ?
                QString("-level:v %1 ").arg(selected_level) : "";

    /************************************* Mode module ***************************************/

    QString mode = "";
    const QString bitrate = QString::number(1000000.0*_BQR.toDouble(), 'f', 0);
    const QString minrate = QString::number(1000000.0*_MINRATE.toDouble(), 'f', 0);
    const QString maxrate = QString::number(1000000.0*_MAXRATE.toDouble(), 'f', 0);
    const QString bufsize = QString::number(1000000.0*_BUFSIZE.toDouble(), 'f', 0);
    const QString selected_mode = t.arr_mode[_CODEC][_MODE];

    if (selected_mode == "CBR") {
        mode = QString("-b:v %1 -minrate %1 -maxrate %1 -bufsize %2 ").arg(bitrate, bufsize);
    }
    else
    if (selected_mode == "ABR") {
        mode = QString("-b:v %1 ").arg(bitrate);
    }
    else
    if (selected_mode == "VBR") {
        mode = QString("-b:v %1 -minrate %2 -maxrate %3 -bufsize %4 ").arg(bitrate, minrate, maxrate, bufsize);
    }
    else
    if (selected_mode == "VBR_NV") {
        mode = QString("-b:v %1 -minrate %2 -maxrate %3 -bufsize %4 -rc vbr ").arg(bitrate, minrate, maxrate, bufsize);
    }
    else
    if (selected_mode == "CRF") {
        mode = QString("-crf %1 ").arg(_BQR);
    }
    else
    if (selected_mode == "CQP") {
        mode = QString("-b:v 0 -cq %1 -qmin %1 -qmax %1 ").arg(_BQR);
    }
    else
    if (selected_mode == "CQP_QS") {
        mode = QString("-global_quality %1 -look_ahead 1 ").arg(_BQR);
    }
    else
    if (selected_mode == "CQP_VA") {
        mode = QString("-qp %1 -rc_mode 4 ").arg(_BQR);
    }

    /************************************* Preset module ***************************************/

    QString preset = "";
    const QString selected_preset = t.getCurrentPreset(_CODEC, _PRESET);
    if (selected_preset != "" && selected_preset != tr("None")) {
        preset = QString("-preset ") + selected_preset.toLower() + QString(" ");
    }

    /************************************* Pass module ***************************************/

    const QString selected_pass = t.arr_pass[_CODEC][_PASS];
    QString pass = "";
    QString pass1 = "";
    if (selected_pass == tr("2 Pass_x265")) {
        pass = "-x265-params pass=2 ";
        pass1 = "-x265-params pass=1 ";
        _flag_two_pass = true;
    }
    else
    if (selected_pass == tr("2 Pass")) {
        pass = "-pass 2 ";
        pass1 = "-pass 1 ";
        _flag_two_pass = true;
    }
    else
    if (selected_pass == tr("2 Pass Optimisation")) {
        pass = "-2pass 1 ";
    }

    /************************************* Audio module ***************************************/

    QString acodec = "";
    const QString selected_acodec = t.arr_acodec[_CODEC][_AUDIO_CODEC];
    QString selected_bitrate = "";

    QString sampling = "";
    const QString selected_sampling = t.arr_sampling[_AUDIO_SAMPLING];
    if (selected_sampling != "Source") {
        sampling = QString("-af aresample=%1:resampler=soxr ").arg(selected_sampling);
    }

    QString channels = "";
    const QString selected_channels = t.arr_channels[_AUDIO_CHANNELS];
    if (selected_channels != "Source") {
        channels = QString(" -ac %1").arg(selected_channels);
    }

    if (selected_acodec == "AAC") {
        selected_bitrate = t.arr_bitrate[0][_AUDIO_BITRATE];
        acodec = QString("-c:a aac -b:a %1").arg(selected_bitrate);
    }
    else
    if (selected_acodec == "AC3") {
        selected_bitrate = t.arr_bitrate[1][_AUDIO_BITRATE];
        acodec = QString("-c:a ac3 -b:a %1").arg(selected_bitrate);
    }
    else
    if (selected_acodec == "DTS") {
        selected_bitrate = t.arr_bitrate[2][_AUDIO_BITRATE];
        acodec = QString("-strict -2 -c:a dca -b:a %1").arg(selected_bitrate);
    }
    else
    if (selected_acodec == "Vorbis") {
        selected_bitrate = t.arr_bitrate[3][_AUDIO_BITRATE];
        acodec = QString("-c:a libvorbis -b:a %1").arg(selected_bitrate);
    }
    else
    if (selected_acodec == "Opus") {
        selected_bitrate = t.arr_bitrate[4][_AUDIO_BITRATE];
        acodec = QString("-c:a libopus -b:a %1").arg(selected_bitrate);
    }
    else
    if (selected_acodec == "PCM 16 bit") {
        acodec = "-c:a pcm_s16le";
    }
    else
    if (selected_acodec == "PCM 24 bit") {
        acodec = "-c:a pcm_s24le";
    }
    else
    if (selected_acodec == "PCM 32 bit") {
        acodec = "-c:a pcm_s32le";
    }
    else
    if (selected_acodec == tr("Source")) {
        acodec = "-c:a copy";
    }
    const QString audio_param = sampling + acodec + channels;

    /************************************ Subtitle module *************************************/

    QString sub_param("");
    if (container == "MKV") {
        _sub_mux_param = QString("-c:s copy");
    }
    else
    if (container == "WebM") {
        _sub_mux_param = QString("-c:s webvtt");
    }
    else
    if (container == "MP4" || container == "MOV") {
        _sub_mux_param = QString("-c:s mov_text");
    }
    else {
        _sub_mux_param = QString("-sn");
    }

    if (_flag_hdr) {
        sub_param = QString(" -c:s copy");
    }
    else {
        sub_param = QString(" ") + _sub_mux_param;
    }

    /************************************* Color module ***************************************/

    // color primaries

    const QString arr_colorprim[11] = {
        "Source",    "bt470m",   "bt470bg",  "bt709",    "bt2020", "smpte170m",
        "smpte240m", "smpte428", "smpte431", "smpte432", "film"
    };
    const QMap<QString, QString> curr_colorprim = {
        {"BT709",           "bt709"},
        {"BT2020",          "bt2020"},
        {"BT601 NTSC",      "smpte170m"},
        {"BT601 PAL",       "bt470bg"},
        {"BT470 System M",  "bt470m"},
        {"SMPTE 240M",      "smpte240m"},
        {"Generic film",    "film"},
        {"DCI P3",          "smpte431"},
        {"XYZ",             "smpte428"},
        {"Display P3",      "smpte432"},
        {"",                ""}
    };

    QString colorprim = "";
    QString colorprim_vf = "";
    const QString selected_colorprim = arr_colorprim[_PRIMARY];
    if (!curr_colorprim.contains(_hdr[CUR_COLOR_PRIMARY])) {
        _message = tr("Can\'t find color primaries %1 in source map.").arg(_hdr[CUR_COLOR_PRIMARY]);
        emit onEncodingInitError(_message);
        return;
    }
    if (selected_colorprim == "Source") {
        if (_hdr[CUR_COLOR_PRIMARY] != "") {
            colorprim = QString("-color_primaries %1 ").arg(curr_colorprim[_hdr[CUR_COLOR_PRIMARY]]);
        }
    }
    else {
        colorprim = QString("-color_primaries %1 ").arg(selected_colorprim);
        if (_REP_PRIM == 2) {
            colorprim_vf = QString("zscale=p=%1").arg(selected_colorprim);
        } else {

        }
    }

    // color matrix

    const QString arr_colormatrix[14] = {
        "Source", "bt470bg", "bt709", "bt2020nc", "bt2020c", "smpte170m", "smpte240m",
        "smpte2085", "chroma-derived-nc", "chroma-derived-c", "fcc", "GBR", "ICtCp", "YCgCo"
    };
    const QMap<QString, QString> curr_colormatrix = {
        {"BT709",                   "bt709"},
        {"BT2020nc",                "bt2020nc"},
        {"BT2020c",                 "bt2020c"},
        {"FCC73682",                "fcc"},
        {"BT470SystemB/G",          "bt470bg"},
        {"SMPTE240M",               "smpte240m"},
        {"YCgCo",                   "YCgCo"},
        {"Y'D'zD'x",                "smpte2085"},
        {"Chromaticity-derivednc",  "chroma-derived-nc"},
        {"Chromaticity-derivedc",   "chroma-derived-c"},
        {"ICtCp",                   "ICtCp"},
        {"BT601",                   "smpte170m"},
        {"Identity",                "GBR"},
        {"",                        ""}
    };

    QString colormatrix = "";
    QString colormatrix_vf = "";
    const QString selected_colormatrix = arr_colormatrix[_MATRIX];
    if (!curr_colormatrix.contains(_hdr[CUR_COLOR_MATRIX])) {
        _message = tr("Can\'t find color matrix %1 in source map.").arg(_hdr[CUR_COLOR_MATRIX]);
        emit onEncodingInitError(_message);
        return;
    }
    if (selected_colormatrix == "Source") {
        if (_hdr[CUR_COLOR_MATRIX] != "") {
            colormatrix = QString("-colorspace %1 ").arg(curr_colormatrix[_hdr[CUR_COLOR_MATRIX]]);
        }
    }
    else {
        colormatrix = QString("-colorspace %1 ").arg(selected_colormatrix);
        if (_REP_MATRIX == 2) {
            colormatrix_vf = QString("zscale=m=%1").arg(selected_colormatrix);
        } else {

        }
    }

    // transfer characteristics

    const QString arr_trc[17] = {
        "Source", "bt470m", "bt470bg", "bt709", "bt1361e", "bt2020-10", "bt2020-12", "smpte170m",
        "smpte240m", "smpte428", "smpte2084", "arib-std-b67", "linear", "log100", "log316",
        "iec61966-2-1", "iec61966-2-4"
    };
    const QMap<QString, QString> curr_transfer = {
        {"BT709",                    "bt709"},
        {"PQ",                       "smpte2084"},
        {"HLG",                      "arib-std-b67"},
        {"BT2020 (10-bit)",          "bt2020-10"},
        {"BT2020 (12-bit)",          "bt2020-12"},
        {"BT470 System M",           "bt470m"},
        {"BT470 System B/G",         "bt470bg"},
        {"SMPTE 240M",               "smpte240m"},
        {"Linear",                   "linear"},
        {"Logarithmic (100:1)",      "log100"},
        {"Logarithmic (31622777:1)", "log316"},
        {"xvYCC",                    "iec61966-2-4"},
        {"BT1361",                   "bt1361e"},
        {"sRGB/sYCC",                "iec61966-2-1"},
        {"SMPTE 428M",               "smpte428"},
        {"BT601",                    "smpte170m"},
        {"",                         ""}
    };

    QString transfer = "";
    QString transfer_vf = "";
    const QString selected_transfer = arr_trc[_TRC];
    if (!curr_transfer.contains(_hdr[CUR_TRANSFER])) {
        _message = tr("Can\'t find transfer characteristics %1 in source map.").arg(_hdr[CUR_TRANSFER]);
        emit onEncodingInitError(_message);
        return;
    }
    if (selected_transfer == "Source") {
        if (_hdr[CUR_TRANSFER] != "") {
            transfer = QString("-color_trc %1 ").arg(curr_transfer[_hdr[CUR_TRANSFER]]);
        }
    }
    else {
        transfer = QString("-color_trc %1 ").arg(selected_transfer);
        if (_REP_TRC == 2) {
            transfer_vf = QString("zscale=t=%1").arg(selected_transfer);
        } else {

        }
    }

    const int vf_size = 6;
    const QString vf_transform_arr[vf_size] = {
        hwaccel_filter_vf,
        fps_vf,
        resize_vf,
        colorprim_vf,
        colormatrix_vf,
        transfer_vf,
    };

    QString vf = "";
    int pos = 0;
    for (int n = 0; n < vf_size; n ++) {
        if (vf_transform_arr[n] != "") {
            pos++;
            if (pos == 1) {
                vf += vf_transform_arr[n];
            } else {
                vf += "," + vf_transform_arr[n];
            }
        }
    }

    QString transform = "";
    if (vf != "") {
        transform = QString("-vf %1 ").arg(vf);
    }

    const QString codec = QString("-map 0:v:0? ") + _audioMapParam + _subtitleMapParam +
                    QString("-map_metadata -1 ") + _videoMetadataParam + _audioMetadataParam +
                    _subtitleMetadataParam + transform + t.arr_params[_CODEC][0];

    /************************************* HDR module ***************************************/

    QString color_range = "";
    QString max_lum = "";
    QString min_lum = "";
    QString max_cll = "";
    QString max_fall = "";
    QString chroma_coord = "";
    QString white_coord = "";
    if (_flag_hdr == true) {

        /********************************* Color range module **********************************/

        if (_COLOR_RANGE == 0) {                             // color range
            if (_hdr[CUR_COLOR_RANGE] == "Limited")
                color_range = "-color_range tv ";
            else
            if (_hdr[CUR_COLOR_RANGE] == "Full")
                color_range = "-color_range pc ";
        }
        else
        if (_COLOR_RANGE == 1) {
            color_range = "-color_range pc ";
        }
        else
        if (_COLOR_RANGE == 2) {
            color_range = "-color_range tv ";
        }

        /************************************* Lum module ***************************************/

        if (_MAX_LUM != "") {                           // max lum
            max_lum = QString("-s max-luminance=%1 ").arg(_MAX_LUM);
        } else {
            if (_hdr[CUR_MAX_LUM] != "") {
                max_lum = QString("-s max-luminance=%1 ").arg(_hdr[CUR_MAX_LUM]);
            } else {
                max_lum = "-d max-luminance ";
            }
        }

        if (_MIN_LUM != "") {                           // min lum
            min_lum = QString("-s min-luminance=%1 ").arg(_MIN_LUM);
        } else {
            if (_hdr[CUR_MIN_LUM] != "") {
                min_lum = QString("-s min-luminance=%1 ").arg(_hdr[CUR_MIN_LUM]);
            } else {
                min_lum = "-d min-luminance ";
            }
        }

        if (_MAX_CLL != "") {                           // max cll
            max_cll = QString("-s max-content-light=%1 ").arg(_MAX_CLL);
        } else {
            if (_hdr[CUR_MAX_CLL] != "") {
                max_cll = QString("-s max-content-light=%1 ").arg(_hdr[CUR_MAX_CLL]);
            } else {
                max_cll = "-d max-content-light ";
            }
        }

        if (_MAX_FALL != "") {                           // max fall
            max_fall = QString("-s max-frame-light=%1 ").arg(_MAX_FALL);
        } else {
            if (_hdr[CUR_MAX_FALL] != "") {
                max_fall = QString("-s max-frame-light=%1 ").arg(_hdr[CUR_MAX_FALL]);
            } else {
                max_fall = "-d max-frame-light ";
            }
        }

        /************************************* Display module ***************************************/

        enum Display {Display_P3, Dci_P3, Bt_2020, Bt_709};
        enum Coord {red_x, red_y, green_x, green_y, blue_x, blue_y, white_x, white_y};
        const QString arr_coord[4][8] = {
            {"0.680", "0.320", "0.265", "0.690", "0.150", "0.060", "0.3127", "0.3290"}, // Display_P3
            {"0.680", "0.320", "0.265", "0.690", "0.150", "0.060", "0.314",  "0.3510"}, // DCI_P3
            {"0.708", "0.292", "0.170", "0.797", "0.131", "0.046", "0.3127", "0.3290"}, // BT.2020
            {"0.640", "0.330", "0.30",  "0.60",  "0.150", "0.060", "0.3127", "0.3290"}  // BT.709
        };
        QString current_coord[8] = {"", "", "", "", "", "", "", ""};

        auto fill_coord = [&current_coord, &arr_coord](int display){
            for (int i = red_x; i <= white_y; i++) {
                current_coord[i] = arr_coord[display][i];
            }
        };
        if (_MASTER_DISPLAY == MasterDisplay::SOURCE) {     // From source
            if (_hdr[CUR_MASTER_DISPLAY] == "Display P3") {
                fill_coord(Display::Display_P3);
            } else
            if (_hdr[CUR_MASTER_DISPLAY] == "DCI P3") {
                fill_coord(Display::Dci_P3);
            } else
            if (_hdr[CUR_MASTER_DISPLAY] == "BT.2020") {
                fill_coord(Display::Bt_2020);
            } else
            if (_hdr[CUR_MASTER_DISPLAY] == "BT.709") {
                fill_coord(Display::Bt_709);
            } else
            if (_hdr[CUR_MASTER_DISPLAY] == "Undefined") {
                const QStringList chr = _hdr[CUR_CHROMA_COORD].split(",");
                if (chr.size() == 6) {
                    for (int i = red_x; i <= blue_y; i++) {
                        current_coord[i] = chr[i];
                    }
                } else {
                    _message = tr("Incorrect master display chroma coordinates source parameters!");
                    emit onEncodingInitError(_message);
                    return;
                }
                const QStringList wht = _hdr[CUR_WHITE_COORD].split(",");
                if (wht.size() == 2) {
                    current_coord[white_x] = wht[0];
                    current_coord[white_y] = wht[1];
                } else {
                    _message = tr("Incorrect master display white point coordinates source parameters!");
                    emit onEncodingInitError(_message);
                    return;
                }
            }
        }
        if (_MASTER_DISPLAY == MasterDisplay::DISPLAY_P3) {     // Display P3
            fill_coord(Display::Display_P3);
        } else
        if (_MASTER_DISPLAY == MasterDisplay::DCI_P3) {     // DCI P3
            fill_coord(Display::Dci_P3);
        } else
        if (_MASTER_DISPLAY == MasterDisplay::BT_2020) {     // BT.2020
            fill_coord(Display::Bt_2020);
        } else
        if (_MASTER_DISPLAY == MasterDisplay::BT_709) {     // BT.709
            fill_coord(Display::Bt_709);
        } else
        if (_MASTER_DISPLAY == MasterDisplay::CUSTOM) {     // Custom
            QStringList chr = _CHROMA_COORD.split(",");
            if (chr.size() == 6) {
                for (int i = red_x; i <= blue_y; i++) {
                    current_coord[i] = chr[i];
                }
            }
            QStringList wht = _WHITE_COORD.split(",");
            if (wht.size() == 2) {
                current_coord[white_x] = wht[0];
                current_coord[white_y] = wht[1];
            }
        }

        if (current_coord[red_x] == "") {
            chroma_coord = "-d chromaticity-coordinates-red-x -d chromaticity-coordinates-red-y "
                           "-d chromaticity-coordinates-green-x -d chromaticity-coordinates-green-y "
                           "-d chromaticity-coordinates-blue-x -d chromaticity-coordinates-blue-y ";
        } else {
            chroma_coord = QString("-s chromaticity-coordinates-red-x=%1 -s chromaticity-coordinates-red-y=%2 "
                                   "-s chromaticity-coordinates-green-x=%3 -s chromaticity-coordinates-green-y=%4 "
                                   "-s chromaticity-coordinates-blue-x=%5 -s chromaticity-coordinates-blue-y=%6 ")
                                   .arg(current_coord[red_x], current_coord[red_y], current_coord[green_x],
                                        current_coord[green_y], current_coord[blue_x], current_coord[blue_y]);
        }
        if (current_coord[white_x] == "") {
            white_coord = "-d white-coordinates-x -d white-coordinates-y ";
        } else {
            white_coord = QString("-s white-coordinates-x=%1 -s white-coordinates-y=%2 ").arg(current_coord[white_x], current_coord[white_y]);
        }
    }

    /************************************* Result module ***************************************/

    _preset_0 = "-hide_banner" + hwaccel + _splitStartParam;
    _preset_pass1 = _splitParam + codec + level + preset + mode + pass1 + color_range
            + colorprim + colormatrix + transfer + "-an -sn -f null /dev/null";
    _preset = _splitParam + codec + level + preset + mode + pass + color_range
            + colorprim + colormatrix + transfer + audio_param + sub_param;
    _preset_mkvmerge = QString("%1%2%3%4%5%6 ").arg(max_cll, max_fall, max_lum, min_lum, chroma_coord, white_coord);
    Dump("Flag two-pass: " << _flag_two_pass);
    Dump("Flag HDR: " << _flag_hdr);
    Dump("preset_0: " << _preset_0.toStdString());

    QString log("");
    if (_flag_two_pass && _flag_hdr) {
        Dump("preset_pass1: " << _preset_pass1.toStdString());
        Dump("preset: " << _preset.toStdString());
        Dump("preset_mkvpropedit: " << _preset_mkvmerge.toStdString());
        log = QString("Preset pass 1: %1 -i <input file> %2\n"
                      "Preset pass 2: %3 -i <input file> %4 -y <output file>\n"
                      "Preset mkvpropedit: %5\n")
                .arg(_preset_0, _preset_pass1, _preset_0, _preset, _preset_mkvmerge);
    }
    else
    if (_flag_two_pass && !_flag_hdr) {
        Dump("preset_pass1: " << _preset_pass1.toStdString());
        Dump("preset: " << _preset.toStdString());
        log = QString("Preset pass 1: %1 -i <input file> %2\n"
                      "Preset pass 2: %3 -i <input file> %4 -y <output file>\n")
                .arg(_preset_0, _preset_pass1, _preset_0, _preset);
    }
    else
    if (!_flag_two_pass && _flag_hdr) {
        Dump("preset: " << _preset.toStdString());
        Dump("preset_mkvpropedit: " << _preset_mkvmerge.toStdString());
        log = QString("Preset: %1 -i <input file> %2 -y <output file>\n"
                      "Preset mkvpropedit: %3\n")
                .arg(_preset_0, _preset, _preset_mkvmerge);
    }
    else
    if (!_flag_two_pass && !_flag_hdr) {
        Dump("preset: " << _preset.toStdString());
        log = QString("Preset: %1 -i <input file> %2 -y <output file>\n")
                .arg(_preset_0, _preset);
    }
    emit onEncodingLog(log);
    encode();
}

void Encoder::encode()   // Encode
{
    Dump("Encode ...");
    QStringList arguments;
    _calling_pr_1 = true;
    processEncoding->disconnect();
    connect(processEncoding, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_1()));
    connect(processEncoding, SIGNAL(finished(int)), this, SLOT(error()));
    emit onEncodingProgress(0, 0.0f);
    if (_mux_mode) {
        Dump("Muxing mode ...");
        _encoding_mode = tr("Muxing:");
        emit onEncodingMode(_encoding_mode);
        arguments << "-hide_banner" << "-i" << _temp_file << "-map" << "0:v:0?" << "-map" << "0:a?"
                  << "-map" << "0:s?" << "-movflags" << "+write_colr"
                  << "-c:v" << "copy" << "-c:a" << "copy" << _sub_mux_param.split(" ") << "-y" << _output_file;
    } else {
        if (*fr_count == 0) {
            _message = tr("The file does not contain FPS information!\nSelect the correct input file!");
            emit onEncodingInitError(_message);
            return;
        }
        emit onEncodingStarted();

        _loop_start = time(nullptr);
        if (!_flag_two_pass && !_flag_hdr) {
            Dump("Encode non HDR...");
            _encoding_mode = tr("Encoding:");
            emit onEncodingMode(_encoding_mode);
            arguments << _preset_0.split(" ") << "-i" << _input_file << _preset.split(" ") << "-y" << _output_file;
        }
        else
        if (!_flag_two_pass && _flag_hdr) {
            Dump("Encode HDR...");
            _encoding_mode = tr("Encoding:");
            emit onEncodingMode(_encoding_mode);
            arguments << _preset_0.split(" ") << "-i" << _input_file << _preset.split(" ") << "-y" << _temp_file;
        }
        else
        if (_flag_two_pass) {
            Dump("Encode 1-st pass...");
            _encoding_mode = tr("1-st pass:");
            emit onEncodingMode(_encoding_mode);
            arguments << _preset_0.split(" ") << "-y" << "-i" << _input_file << _preset_pass1.split(" ");
        }
    }
    //qDebug() << arguments;
    processEncoding->start("ffmpeg", arguments);
    if (!processEncoding->waitForStarted()) {
        Dump("cmd command not found!!!");
        processEncoding->disconnect();
        _message = tr("An unknown error occurred!\n Possible FFMPEG not installed.\n");
        emit onEncodingInitError(_message);
    }
}

void Encoder::add_metadata() /*** Add metedata ***/
{
    Dump("Add metadata ...");
    _calling_pr_1 = true;
    processEncoding->disconnect();
    connect(processEncoding, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_2()));
    connect(processEncoding, SIGNAL(finished(int)), this, SLOT(error()));
    _encoding_mode = tr("Add data:");
    emit onEncodingMode(_encoding_mode);
    emit onEncodingProgress(0, 0.0f);
    QStringList arguments;
    arguments << "--edit" << "track:1" << _preset_mkvmerge.split(" ") << _temp_file;
    processEncoding->start("mkvpropedit", arguments);
    if (!processEncoding->waitForStarted()) {
        Dump("cmd command not found!!!");
        processEncoding->disconnect();
        _message = tr("An unknown error occured!\n Possible mkvtoolnix not installed.\n");
        emit onEncodingInitError(_message);
    }
}

void Encoder::progress_1()   // Progress
{
    QString line = QString(processEncoding->readAllStandardOutput());
    const QString line_mod6 = line.replace("   ", " ").replace("  ", " ").replace("  ", " ").replace("= ", "=");
    emit onEncodingLog(line_mod6);
    const int pos_err_1 = line_mod6.indexOf("[error]:");
    const int pos_err_2 = line_mod6.indexOf("Error");
    const int pos_err_3 = line_mod6.indexOf(" @ ");
    if (pos_err_1 != -1) {
        const QStringList error = line_mod6.split(":");
        if (error.size() >= 2)
            _error_message = error[1];
    }
    if (pos_err_2 != -1) {
        _error_message = line_mod6;
    }
    if (pos_err_3 != -1) {
        const QStringList error = line_mod6.split("]");
        if (error.size() >= 2)
            _error_message = error[1];
    }
    const int pos_st = line_mod6.indexOf("frame=");
    if (pos_st == 0) {
        QStringList data = line_mod6.split(" ");
        const QString frame_qstr = data[0].replace("frame=", "");
        int frame = frame_qstr.toInt();
        if (frame == 0)
            frame = 1;
        const time_t iter_start = time(nullptr);
        const int timer = static_cast<int>(iter_start - _loop_start);
        const float full_time = static_cast<float>(timer * (*fr_count)) / (frame);
        float rem_time = full_time - static_cast<float>(timer);
        if (rem_time < 0.0f)
            rem_time = 0.0f;
        if (rem_time > MAXIMUM_ALLOWED_TIME)
            rem_time = MAXIMUM_ALLOWED_TIME;

        float percent = static_cast<float>(frame * 100) / (*fr_count);
        int percent_int = rnd(percent);
        if (percent_int > 100)
            percent_int = 100;
        emit onEncodingProgress(percent_int, rem_time);

        if ((percent_int >= 95) && _calling_pr_1) {
             disconnect(processEncoding, SIGNAL(finished(int)), this, SLOT(error()));
             if (_mux_mode) {
                 connect(processEncoding, SIGNAL(finished(int)), this, SLOT(completed()));
             } else {
                 if (!_flag_two_pass && _flag_hdr) {
                     disconnect(processEncoding, SIGNAL(finished(int)), this, SLOT(encode()));
                     connect(processEncoding, SIGNAL(finished(int)), this, SLOT(add_metadata()));
                 } else
                 if (!_flag_two_pass && !_flag_hdr) {
                     disconnect(processEncoding, SIGNAL(finished(int)), this, SLOT(encode()));
                     connect(processEncoding, SIGNAL(finished(int)), this, SLOT(completed()));
                 } else
                 if (_flag_two_pass) {
                     connect(processEncoding, SIGNAL(finished(int)), this, SLOT(encode()));
                     _flag_two_pass = false;
                 }
             }
             _calling_pr_1 = false;
        }
    }
}

void Encoder::progress_2()   // Progress mkvpropedit
{
    const QString line = QString(processEncoding->readAllStandardOutput());
    emit onEncodingLog(line);
    const int pos_st = line.indexOf("Done.");
    const int pos_nf = line.indexOf("Nothing to do.");
    if ((pos_st != -1) || (pos_nf != -1)) {
        int percent = 100;
        emit onEncodingProgress(percent, 0.0f);
        if (_calling_pr_1) {
            disconnect(processEncoding, SIGNAL(finished(int)), this, SLOT(error()));
            _mux_mode = true;
            _loop_start = time(nullptr);
            _calling_pr_1 = false;
            connect(processEncoding, SIGNAL(finished(int)), this, SLOT(encode()));
        }
    }
}

QProcess::ProcessState Encoder::getEncodingState()
{
    return processEncoding->state();
}

void Encoder::pauseEncoding()
{
#ifdef Q_OS_WIN
    _PROCESS_INFORMATION *pi = processEncoding->pid();
    SuspendThread(pi->hThread);  // pause for Windows
#else
    kill(pid_t(processEncoding->processId()), SIGSTOP);  // pause for Unix
#endif
}

void Encoder::resumeEncoding()
{
#ifdef Q_OS_WIN
    _PROCESS_INFORMATION *pi = processEncoding->pid();
    ResumeThread(pi->hThread);  // resume for Windows
#else
    kill(pid_t(processEncoding->processId()), SIGCONT); // resume for Unix
#endif
}

void Encoder::stopEncoding()
{
    processEncoding->disconnect();
    connect(processEncoding, SIGNAL(finished(int)), this, SLOT(abort()));
    processEncoding->kill();
}

void Encoder::killEncoding()
{
    processEncoding->kill();
}

void Encoder::completed()
{
    processEncoding->disconnect();
    if (_flag_hdr)
        QDir().remove(_temp_file);
    emit onEncodingCompleted();
}

void Encoder::abort()
{
    processEncoding->disconnect();
    if (_flag_hdr)
        QDir().remove(_temp_file);
    emit onEncodingAborted();
}

void Encoder::error()
{
    processEncoding->disconnect();
    if (_flag_hdr)
        QDir().remove(_temp_file);
    emit onEncodingError(_error_message);
}
