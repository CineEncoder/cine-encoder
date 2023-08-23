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
#include <ctime>
#include <algorithm>


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
                           const QString  (&_hdr)[11],
                           Data           data,
                           int            *_fr_count,
                           int            streamCutting,
                           const QString &subtitle_font,
                           int subtitle_font_size,
                           const QString &subtitle_font_color,
                           const bool burn_background,
                           const QString &subtitle_background_color
                           )
{
    Print("Make preset...");
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

    _extAudioPaths.clear();
    _extSubPaths.clear();
    _preset_0 = "";
    _preset_pass1.clear();
    _preset.clear();
    _preset_mkvmerge = "";
    _sub_mux_param.clear();
    _error_message = "";
    _flag_two_pass = false;
    _flag_hdr = false;
    _burn_subtitle = false;
    _mux_mode = false;
    *fr_count = 0;

    /****************************************** Resize ****************************************/
    QString resize_vf = "";
    const QString new_width = (t.arr_width[_WIDTH] != "Source") ? t.arr_width[_WIDTH] : _width;
    const QString new_height = (t.arr_height[_HEIGHT] != "Source") ? t.arr_height[_HEIGHT] : _height;
    if ((t.arr_width[_WIDTH] != "Source") || (t.arr_height[_HEIGHT] != "Source")) {
        if (_CODEC >= CODEC_QSV_FIRST && _CODEC <= CODEC_QSV_LAST) // QSV
            resize_vf = QString("scale_qsv=w=%1:h=%2,setsar=1:1").arg(new_width, new_height);
        else
        if (_CODEC >= CODEC_VAAPI_FIRST && _CODEC <= CODEC_VAAPI_LAST) // VAAPI
            resize_vf = QString("scale_vaapi=w=%1:h=%2,setsar=1:1").arg(new_width, new_height);
        else
            resize_vf = QString("scale=%1:%2,setsar=1:1").arg(new_width, new_height);
    }

    /******************************************* FPS *****************************************/

    QString fps_vf = "";
    double fps_dest;

    if (t.frame_rate[_FRAME_RATE] != "Source") {
        fps_dest = t.frame_rate[_FRAME_RATE].toDouble();
        if (_BLENDING == t.Blending::Simple) {
            if (_CODEC >= CODEC_QSV_FIRST && _CODEC <= CODEC_QSV_LAST) // QSV
                fps_vf = QString("vpp_qsv=framerate=%1").arg(t.frame_rate[_FRAME_RATE]);
            else
            if (_CODEC >= CODEC_VAAPI_FIRST && _CODEC <= CODEC_VAAPI_LAST) // VAAPI
                fps_vf = QString("fps=fps=%1").arg(t.frame_rate[_FRAME_RATE]);
            else
                fps_vf = QString("fps=fps=%1").arg(t.frame_rate[_FRAME_RATE]);
        }
        else
        if (_BLENDING == t.Blending::Interpolated)
            fps_vf = QString("framerate=fps=%1").arg(t.frame_rate[_FRAME_RATE]);
        else
        if (_BLENDING == t.Blending::MCI)
            fps_vf = QString("minterpolate=fps=%1:mi_mode=mci:mc_mode=aobmc:me_mode=bidir:vsbmc=1")
                    .arg(t.frame_rate[_FRAME_RATE]);
        else
        if (_BLENDING == t.Blending::Blend)
            fps_vf = QString("minterpolate=fps=%1:mi_mode=blend").arg(t.frame_rate[_FRAME_RATE]);
    } else
        fps_dest = _fps.toDouble();

    /****************************************** Split ****************************************/

    QVector<double> extDurVect;
    foreach (auto dur, FIELDS(externAudioDuration))
        extDurVect.push_back(0.001*dur.toDouble());
    double minExtTime = *std::min_element(std::begin(extDurVect), std::end(extDurVect));
    Print("Min external time: " << minExtTime);

    double duration = _endTime - _startTime;
    if (streamCutting == 1) {
        if (minExtTime > 0) {
            if (minExtTime < duration || minExtTime < _dur)
                duration = minExtTime;
            else
            if (_dur < minExtTime)
                duration = _dur;
        }
    }
    Print("Cut duration: " << duration);
    QStringList _splitStartParam;
    QStringList _splitParam;
    if (duration > 0) {
        *fr_count = rnd(duration * fps_dest);
        _splitStartParam.append(t.arr_codec[_CODEC][0] == tr("Source") ? QStringList{"-copyts"} : QStringList{"-copytb","0"}); //-copytb 0
        _splitStartParam.append(QStringList{"-ss", QString::number(_startTime, 'f', 3)});
        _splitParam.append(QStringList{"-vframes", numToStr(*fr_count)});
    } else {
        *fr_count = rnd(_dur * fps_dest);
    }

    /************************************** Video metadata ************************************/

    QVector<QString> videoMetadata(data.videoMetadata.size(), "");
    QStringList _videoMetadataParam;

    if (globalTitle != "") {
        videoMetadata[0] = QString("-metadata:s:v:0 title=%1 ").arg(globalTitle.replace(" ", "\u00A0"));
        _videoMetadataParam.append({"-metadata:s:v:0",QString("title="+globalTitle)});
    } else {
        if (data.videoMetadata[VIDEO_TITLE] != "") {
            videoMetadata[0] = QString("-metadata:s:v:0 title=%1 ").arg(data.videoMetadata[VIDEO_TITLE]
                                                                                .replace(" ", "\u00A0"));
            _videoMetadataParam.append({"-metadata:s:v:0",QString("title=" + data.videoMetadata[VIDEO_TITLE])});
        } else {
            videoMetadata[0] = QString("-map_metadata:s:v:0 -1 ");
            _videoMetadataParam.append({"-map_metadata:s:v:0","-1"});
        }
    }
    if (data.videoMetadata[VIDEO_MOVIENAME] != "") {
        videoMetadata[1] = QString("-metadata title=%1 ").arg(data.videoMetadata[VIDEO_MOVIENAME]
                                                              .replace(" ", "\u00A0"));
        _videoMetadataParam.append({"-metadata",QString("title="+data.videoMetadata[VIDEO_MOVIENAME])});
    }
    if (data.videoMetadata[VIDEO_AUTHOR] != "") {
        videoMetadata[2] = QString("-metadata author=%1 ").arg(data.videoMetadata[VIDEO_AUTHOR]
                                                               .replace(" ", "\u00A0"));
        _videoMetadataParam.append({"-metadata",QString("author="+data.videoMetadata[VIDEO_AUTHOR])});
    }
    if (data.videoMetadata[VIDEO_DESCRIPTION] != "") {
        videoMetadata[3] = QString("-metadata description=%1 ").arg(data.videoMetadata[VIDEO_DESCRIPTION]
                                                                    .replace(" ", "\u00A0"));
        _videoMetadataParam.append({"-metadata",QString("description="+data.videoMetadata[VIDEO_DESCRIPTION])});
    }
    if (data.videoMetadata[VIDEO_YEAR] != "") {
        videoMetadata[4] = QString("-metadata year=%1 ").arg(data.videoMetadata[VIDEO_YEAR].replace(" ", ""));
        _videoMetadataParam.append({"-metadata",QString("year="+data.videoMetadata[VIDEO_YEAR])});
    }
    if (data.videoMetadata[VIDEO_PERFORMER] != "") {
        videoMetadata[5] = QString("-metadata author=%1 ").arg(data.videoMetadata[VIDEO_PERFORMER]
                                                               .replace(" ", "\u00A0"));
        _videoMetadataParam.append({"-metadata",QString("author="+data.videoMetadata[VIDEO_PERFORMER])});
    }

    /************************************** Audio streams ************************************/

    QVector<QString> audioLang(CHECKS(audioChecks).size(), ""),
                     audioTitle(CHECKS(audioChecks).size(), ""),
                     audioMap(CHECKS(audioChecks).size(), ""),
                     audioDef(CHECKS(audioChecks).size(), "");
    QStringList _audioMapParam,
            _audioMetadataParam;
    int     audioNum = 0;

    Q_LOOP(k, 0, CHECKS(audioChecks).size()) {
        if (CHECKS(audioChecks)[k] == true) {
            audioMap[k] = QString("-map 0:a:%1? ").arg(numToStr(k));
            _audioMapParam.append({"-map", "0:a:"+numToStr(k)+"?" });
            audioLang[k] = QString("-metadata:s:a:%1 language=%2 ")
                           .arg(numToStr(audioNum), FIELDS(audioLangs)[k].replace(" ", "\u00A0"));
            _audioMetadataParam.append({"-metadata:s:a:"+numToStr(audioNum),"language="+FIELDS(audioLangs)[k]});
            audioTitle[k] = QString("-metadata:s:a:%1 title=%2 ")
                            .arg(numToStr(audioNum), FIELDS(audioTitles)[k].replace(" ", "\u00A0"));
            _audioMetadataParam.append({"-metadata:s:a:"+numToStr(audioNum),"title="+FIELDS(audioTitles)[k]});
            audioDef[k] = QString("-disposition:a:%1 %2 ")
                           .arg(numToStr(audioNum), CHECKS(audioDef)[k] ? "default" : "0");
            _audioMetadataParam.append({"-disposition:a:"+numToStr(audioNum),CHECKS(audioDef)[k] ? "default" : "0"});
            audioNum++;
        }
    }

    /********************************* External Audio streams ************************************/

    QVector<QString> extAudioLang(CHECKS(externAudioChecks).size(), ""),
                     extAudioTitle(CHECKS(externAudioChecks).size(), ""),
                     extAudioMap(CHECKS(externAudioChecks).size(), ""),
                     extAudioDef(CHECKS(externAudioChecks).size(), "");
    int extTrackNum = 1;

    Q_LOOP(k, 0, CHECKS(externAudioChecks).size()) {
        if (CHECKS(externAudioChecks)[k] == true) {
            _extAudioPaths << "-i" << FIELDS(externAudioPath)[k];
            extAudioMap[k] = QString("-map %1:a? ").arg(numToStr(extTrackNum));
            _audioMapParam.append({"-map", numToStr(extTrackNum) + ":a?" });
            extAudioLang[k] = QString("-metadata:s:a:%1 language=%2 ")
                           .arg(numToStr(audioNum), FIELDS(externAudioLangs)[k].replace(" ", "\u00A0"));
            _audioMetadataParam.append({"-metadata:s:a:"+numToStr(audioNum),"language="+FIELDS(externAudioLangs)[k]});
            extAudioTitle[k] = QString("-metadata:s:a:%1 title=%2 ")
                            .arg(numToStr(audioNum), FIELDS(externAudioTitles)[k].replace(" ", "\u00A0"));
            _audioMetadataParam.append({"-metadata:s:a:"+numToStr(audioNum),"title="+FIELDS(externAudioTitles)[k]});
            extAudioDef[k] = QString("-disposition:a:%1 %2 ")
                           .arg(numToStr(audioNum), CHECKS(externAudioDef)[k] ? "default" : "0");
            _audioMetadataParam.append({"-disposition:a:"+numToStr(audioNum),CHECKS(externAudioDef)[k] ? "default" : "0"});
            audioNum++;
            extTrackNum++;
        }
    }

    /**************************************** Subtitles **************************************/
    QString burn_subt_vf, burn_string;
    burn_string = "charenc=:force_style=\"'FontName='" + subtitle_font +
                          "',Fontsize=" + numToStr(subtitle_font_size) +
                          ",PrimaryColour=&H" + subtitle_font_color +
                          ",BorderStyle=4";
    if (burn_background) {
        burn_string += QString(",BackColour=&H" + subtitle_background_color);
    }
    burn_string += QString("'\"");
    Q_LOOP(k, 0, CHECKS(subtBurn).size()) {
        if (CHECKS(subtBurn)[k]) {
            _burn_subtitle = true;
            QString _input_file(input_file);
            burn_subt_vf = QString("subtitles='%1':%2:stream_index=%3").arg(_input_file, burn_string, numToStr(k));
            break;
        }
    }
    Q_LOOP(k, 0, CHECKS(externSubtBurn).size()) {
        if (CHECKS(externSubtBurn)[k]) {
            _burn_subtitle = true;
            burn_subt_vf = QString("subtitles='%1':%2").arg(FIELDS(externSubtPath)[k], burn_string);
            break;
        }
    }

    QVector<QString> subtitleLang(CHECKS(subtChecks).size(), ""),
                     subtitleTitle(CHECKS(subtChecks).size(), ""),
                     subtitleMap(CHECKS(subtChecks).size(), ""),
                     subtitleDef(CHECKS(subtChecks).size(), "");
    QStringList _subtitleMapParam,
            _subtitleMetadataParam;
    int     subtNum = 0;

    if (!_burn_subtitle) {
        Q_LOOP(k, 0, CHECKS(subtChecks).size()) {
            if (CHECKS(subtChecks)[k] == true) {
                subtitleMap[k] = QString("-map 0:s:%1? ").arg(numToStr(k));
                _subtitleMapParam.append({"-map", "0:s:"+numToStr(k)+"?"});
                subtitleLang[k] = QString("-metadata:s:s:%1 language=%2 ")
                                      .arg(numToStr(subtNum), FIELDS(subtLangs)[k].replace(" ", "\u00A0"));
                _subtitleMetadataParam.append({"-metadata:s:s:"+numToStr(subtNum), "language="+FIELDS(subtLangs)[k] });
                subtitleTitle[k] = QString("-metadata:s:s:%1 title=%2 ")
                                       .arg(numToStr(subtNum), FIELDS(subtTitles)[k].replace(" ", "\u00A0"));
                _subtitleMetadataParam.append({"-metadata:s:s:"+numToStr(subtNum), "title="+FIELDS(subtTitles)[k] });
                subtitleDef[k] = QString("-disposition:s:%1 %2 ")
                                     .arg(numToStr(subtNum), CHECKS(subtDef)[k] ? "default" : "0");
                _subtitleMetadataParam.append({"-disposition:s:"+numToStr(subtNum), CHECKS(subtDef)[k] ? "default" : "0" });
                subtNum++;
            }
        }
    }

    /****************************** External Subtitle streams *********************************/

    QVector<QString> extSubLang(CHECKS(externSubtChecks).size(), ""),
                     extSubTitle(CHECKS(externSubtChecks).size(), ""),
                     extSubMap(CHECKS(externSubtChecks).size(), ""),
                     extSubDef(CHECKS(externSubtChecks).size(), "");

    if (!_burn_subtitle) {
        Q_LOOP(k, 0, CHECKS(externSubtChecks).size()) {
            if (CHECKS(externSubtChecks)[k] == true) {
                _extSubPaths << "-i" << FIELDS(externSubtPath)[k];
                extSubMap[k] = QString("-map %1:s? ").arg(numToStr(extTrackNum));
                _subtitleMapParam.append({"-map", numToStr(extTrackNum)+":s?"});
                extSubLang[k] = QString("-metadata:s:s:%1 language=%2 ")
                                    .arg(numToStr(subtNum), FIELDS(externSubtLangs)[k].replace(" ", "\u00A0"));
                _subtitleMetadataParam.append({"-metadata:s:s:"+numToStr(subtNum), "language="+FIELDS(externSubtLangs)[k] });
                extSubTitle[k] = QString("-metadata:s:s:%1 title=%2 ")
                                     .arg(numToStr(subtNum), FIELDS(externSubtTitles)[k].replace(" ", "\u00A0"));
                _subtitleMetadataParam.append({"-metadata:s:s:"+numToStr(subtNum), "title="+FIELDS(externSubtTitles)[k] });
                extSubDef[k] = QString("-disposition:s:%1 %2 ")
                                   .arg(numToStr(subtNum), CHECKS(externSubtDef)[k] ? "default" : "0");
                _subtitleMetadataParam.append({"-disposition:s:"+numToStr(subtNum), CHECKS(externSubtDef)[k] ? "default" : "0" });
                subtNum++;
                extTrackNum++;
            }
        }
    }

    /************************************* Codec module ***************************************/

    const QString hwaccel = t.arr_params[_CODEC][1];
    const QString hwaccel_filter_vf = t.arr_params[_CODEC][3];
    _flag_hdr = static_cast<bool>(t.arr_params[_CODEC][2].toInt());

    /************************************* Level module **************************************/

    const QString selected_level = t.arr_level[_CODEC][_LEVEL];
    QStringList level;
    if (selected_level != "" && selected_level != tr("Auto"))
    {
        level.append({"-level:v",selected_level});
    }
    /************************************* Mode module ***************************************/

    QStringList mode;
    const QString bitrate = QString::number(1000000.0*_BQR.toDouble(), 'f', 0);
    const QString minrate = QString::number(1000000.0*_MINRATE.toDouble(), 'f', 0);
    const QString maxrate = QString::number(1000000.0*_MAXRATE.toDouble(), 'f', 0);
    const QString bufsize = QString::number(1000000.0*_BUFSIZE.toDouble(), 'f', 0);
    const QString selected_mode = t.arr_mode[_CODEC][_MODE];

    if (selected_mode == "CBR") {
        mode.append({"-b:v", bitrate, "-minrate", bitrate, "-maxrate", bitrate, "-bufsize", bufsize });
    }
    else
    if (selected_mode == "ABR") {
        mode.append({"-b:v", bitrate });
    }
    else
    if (selected_mode == "VBR") {
        mode.append({"-b:v", bitrate, "-minrate", minrate, "-maxrate", maxrate, "-bufsize", bufsize});
    }
    else
    if (selected_mode == "VBR_NV") {
        mode.append({"-b:v", bitrate, "-minrate", minrate, "-maxrate", maxrate, "-bufsize", bufsize, "-rc", "vbr"});
    }
    else
    if (selected_mode == "CRF") {
        mode.append({"-crf" , _BQR});
    }
    else
    if (selected_mode == "CQP") {
        mode.append({"-b:v", "0", "-cq", _BQR, "-qmin", _BQR, "-qmax", _BQR});
    }
    else
    if (selected_mode == "CQP_QS") {
        mode.append({"-global_quality", _BQR, "-look_ahead", "1"});
    }
    else
    if (selected_mode == "CQP_VA") {
        mode.append({"-qp", _BQR, "-rc_mode", "4"});
    }

    /************************************* Preset module ***************************************/

    QStringList preset;
    const QString selected_preset = t.getCurrentPreset(_CODEC, _PRESET);
    if (selected_preset != "" && selected_preset != tr("None")) {
        preset.append({"-preset", selected_preset.toLower() });
    }

    /************************************* Pass module ***************************************/

    const QString selected_pass = t.arr_pass[_CODEC][_PASS];
    QStringList pass;
    QStringList pass1;
    if (selected_pass == tr("2 Pass_x265")) {
        pass.append({"-x265-params","pass=2"});
        pass1.append({"-x265-params","pass=1"});
        _flag_two_pass = true;
    }
    else
    if (selected_pass == tr("2 Pass")) {
        pass.append({"-pass","2"});
        pass1.append({"-pass","1"});
        _flag_two_pass = true;
    }
    else
    if (selected_pass == tr("2 Pass Optimisation")) {
        pass.append({"-2pass","1"});
    }

    /************************************* Audio module ***************************************/

    QStringList acodec;
    const QString selected_acodec = t.arr_acodec[_CODEC][_AUDIO_CODEC];
    QString selected_bitrate = "";

    QStringList sampling;
    const QString selected_sampling = t.arr_sampling[_AUDIO_SAMPLING];
    if (selected_sampling != "Source") {
        sampling.append({"-af",QString("aresample=%1:resampler=soxr").arg(selected_sampling)});
    }

    QStringList channels;
    const QString selected_channels = t.arr_channels[_AUDIO_CHANNELS];
    if (selected_channels != "Source") {
        channels.append({"-ac", selected_channels});
    }

    if (selected_acodec == "AAC") {
        selected_bitrate = t.arr_bitrate[0][_AUDIO_BITRATE];
        acodec.append({"-c:a","aac", "-b:a", selected_bitrate});
    }
    else
    if (selected_acodec == "AC3") {
        selected_bitrate = t.arr_bitrate[1][_AUDIO_BITRATE];
        acodec.append({"-c:a", "ac3", "-b:a", selected_bitrate});
    }
    else
    if (selected_acodec == "DTS") {
        selected_bitrate = t.arr_bitrate[2][_AUDIO_BITRATE];
        acodec.append({"-strict", "-2", "-c:a", "dca", "-b:a", selected_bitrate});
    }
    else
    if (selected_acodec == "Vorbis") {
        selected_bitrate = t.arr_bitrate[3][_AUDIO_BITRATE];
        acodec.append({"-c:a", "libvorbis", "-b:a", selected_bitrate});
    }
    else
    if (selected_acodec == "Opus") {
        selected_bitrate = t.arr_bitrate[4][_AUDIO_BITRATE];
        acodec.append({"-c:a", "libopus", "-b:a", selected_bitrate});
    }
    else
    if (selected_acodec == "PCM 16 bit") {
        acodec.append({"-c:a", "pcm_s16le"});
    }
    else
    if (selected_acodec == "PCM 24 bit") {
        acodec.append({"-c:a", "pcm_s24le"});
    }
    else
    if (selected_acodec == "PCM 32 bit") {
        acodec.append({"-c:a", "pcm_s32le"});
    }
    else
    if (selected_acodec == tr("Source")) {
        acodec.append({"-c:a", "copy"});
    }
    const QStringList audio_param = sampling + acodec + channels;

    /************************************ Subtitle module *************************************/

    QStringList sub_param;

    if (_burn_subtitle) {
        _sub_mux_param.append("-sn");
        sub_param.append(_sub_mux_param);
    } else {
        if (container == "mkv") {
            _sub_mux_param.append({"-c:s","ass"});
        } else
        if (container == "webm") {
            _sub_mux_param.append({"-c:s","webvtt"});
        } else
        if (container == "mp4" || container == "mov") {
            _sub_mux_param.append({"-c:s", "mov_text"});
        } else {
            _sub_mux_param.append("-sn");
            emit onEncodingError(tr("Container \'%1\' will be transcoded without subtitles.")
                                         .arg(container), true);
        }

        if (_flag_hdr) {
            sub_param.append({"-c:s", "ass"});
        } else {
            sub_param.append(_sub_mux_param);
        }
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

    QStringList colorprim;
    QStringList colorprim_vf;
    const QString selected_colorprim = arr_colorprim[_PRIMARY];
    if (!curr_colorprim.contains(_hdr[CUR_COLOR_PRIMARY])) {
        _message = tr("Can\'t find color primaries %1 in source map.").arg(_hdr[CUR_COLOR_PRIMARY]);
        emit onEncodingInitError(_message);
        return;
    }
    if (selected_colorprim == "Source") {
        if (_hdr[CUR_COLOR_PRIMARY] != "") {
            colorprim.append({"-color_primaries", curr_colorprim[_hdr[CUR_COLOR_PRIMARY]]});
        }
    }
    else {
        colorprim.append({"-color_primaries", selected_colorprim});
        if (_REP_PRIM == 2) {
            colorprim_vf.append(QString("zscale=p=%1").arg(selected_colorprim));
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

    QStringList colormatrix;
    QStringList colormatrix_vf;
    const QString selected_colormatrix = arr_colormatrix[_MATRIX];
    if (!curr_colormatrix.contains(_hdr[CUR_COLOR_MATRIX])) {
        _message = tr("Can\'t find color matrix %1 in source map.").arg(_hdr[CUR_COLOR_MATRIX]);
        emit onEncodingInitError(_message);
        return;
    }
    if (selected_colormatrix == "Source") {
        if (_hdr[CUR_COLOR_MATRIX] != "") {
            colormatrix.append({"-colorspace", curr_colormatrix[_hdr[CUR_COLOR_MATRIX]]});
        }
    }
    else {
        colormatrix.append({"-colorspace", selected_colormatrix});
        if (_REP_MATRIX == 2) {
            colormatrix_vf.append(QString("zscale=m=%1").arg(selected_colormatrix));
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

    QStringList transfer;
    QStringList transfer_vf;
    const QString selected_transfer = arr_trc[_TRC];
    if (!curr_transfer.contains(_hdr[CUR_TRANSFER])) {
        _message = tr("Can\'t find transfer characteristics %1 in source map.").arg(_hdr[CUR_TRANSFER]);
        emit onEncodingInitError(_message);
        return;
    }
    if (selected_transfer == "Source") {
        if (_hdr[CUR_TRANSFER] != "") {
            transfer.append({"-color_trc",curr_transfer[_hdr[CUR_TRANSFER]]});
        }
    }
    else {
        transfer.append({"-color_trc", selected_transfer});
        if (_REP_TRC == 2) {
            transfer_vf.append(QString("zscale=t=%1").arg(selected_transfer));
        } else {

        }
    }

    QStringList codec = {"-map", "0:v:0?"};
    codec.append(_audioMapParam);
    codec.append(_subtitleMapParam);
    codec.append({"-map_metadata", "-1", "-map_chapters", "-1"});
    codec.append(_videoMetadataParam);
    codec.append(_audioMetadataParam);
    codec.append(_subtitleMetadataParam);
    if ((hwaccel_filter_vf != "") ||
        (fps_vf != "") ||
        (resize_vf != "") ||
        (colorprim_vf.count() != 0) ||
        (colormatrix_vf.count() != 0) ||
        (transfer_vf.count() != 0) ||
        (burn_subt_vf != "") ||
        _burn_subtitle)
    {
        codec.append("-vf");
    }
    codec.append(hwaccel_filter_vf.split(" "));
    codec.append(fps_vf.split(" "));
    codec.append(resize_vf.split(" "));
    codec.append(colorprim_vf);
    codec.append(colormatrix_vf);
    codec.append(transfer_vf);
    codec.append(burn_subt_vf);//.split(" "));
    codec.append(t.arr_params[_CODEC][0].split(" "));

    /************************************* HDR module ***************************************/

    QStringList color_range;
    QStringList max_lum;
    QStringList min_lum;
    QStringList max_cll;
    QStringList max_fall;
    QStringList chroma_coord;
    QStringList white_coord;
    if (_flag_hdr == true) {

        /********************************* Color range module **********************************/

        if (_COLOR_RANGE == 0) {                             // color range
            if (_hdr[CUR_COLOR_RANGE] == "Limited")
                color_range.append({"-color_range","tv"});
            else
            if (_hdr[CUR_COLOR_RANGE] == "Full")
                color_range.append({"-color_range","pc"});
        }
        else
        if (_COLOR_RANGE == 1) {
            color_range.append({"-color_range","pc"});
        }
        else
        if (_COLOR_RANGE == 2) {
            color_range.append({"-color_range","tv"});
        }

        /************************************* Lum module ***************************************/

        if (_MAX_LUM != "") {                           // max lum
            max_lum.append({"-s", QString("max-luminance=%1").arg(_MAX_LUM)});
        } else {
            if (_hdr[CUR_MAX_LUM] != "") {
                max_lum.append({"-s", QString("max-luminance=%1").arg(_hdr[CUR_MAX_LUM])});
            } else {
                max_lum.append({"-d", "max-luminance"});
            }
        }

        if (_MIN_LUM != "") {                           // min lum
            min_lum.append({"-s", QString("min-luminance=%1").arg(_MIN_LUM)});
        } else {
            if (_hdr[CUR_MIN_LUM] != "") {
                min_lum.append({"-s", QString("min-luminance=%1").arg(_hdr[CUR_MIN_LUM])});
            } else {
                min_lum.append({"-d", "min-luminance"});
            }
        }

        if (_MAX_CLL != "") {                           // max cll
            max_cll.append({"-s", QString("max-content-light=%1").arg(_MAX_CLL)});
        } else {
            if (_hdr[CUR_MAX_CLL] != "") {
                max_cll.append({"-s", QString("max-content-light=%1").arg(_hdr[CUR_MAX_CLL])});
            } else {
                max_cll.append({"-d", "max-content-light"});
            }
        }

        if (_MAX_FALL != "") {                           // max fall
            max_fall.append({"-s", QString("max-frame-light=%1").arg(_MAX_FALL)});
        } else {
            if (_hdr[CUR_MAX_FALL] != "") {
                max_fall.append({"-s", QString("max-frame-light=%1").arg(_hdr[CUR_MAX_FALL])});
            } else {
                max_fall.append({"-d", "max-frame-light"});
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
            chroma_coord.append({"-d", "chromaticity-coordinates-red-x", "-d", "chromaticity-coordinates-red-y",
                                    "-d", "chromaticity-coordinates-green-x", "-d", "chromaticity-coordinates-green-y",
                                    "-d", "chromaticity-coordinates-blue-x", "-d", "chromaticity-coordinates-blue-y"});
        } else {
            chroma_coord.append({"-s", "chromaticity-coordinates-red-x="+current_coord[red_x],
                                 "-s", "chromaticity-coordinates-red-y="+current_coord[red_y],
                                 "-s", "chromaticity-coordinates-green-x="+current_coord[green_x],
                                 "-s", "chromaticity-coordinates-green-y="+current_coord[green_y],
                                 "-s", "chromaticity-coordinates-blue-x="+current_coord[blue_x],
                                 "-s", "chromaticity-coordinates-blue-y="+current_coord[blue_y]});
        }
        if (current_coord[white_x] == "") {
            white_coord.append({"-d", "white-coordinates-x", "-d","white-coordinates-y"});
        } else {
            white_coord.append({"-s", QString("white-coordinates-x=%1").arg(current_coord[white_x]), "-s", QString("white-coordinates-y=%2 ").arg(current_coord[white_y])});
        }
    }

    /************************************* Result module ***************************************/

    _preset_0 = "-hide_banner -probesize 100M -analyzeduration 50M" + hwaccel + _splitStartParam.join(" ");
    _preset_pass1 = _splitParam + codec + level + preset + mode + pass1 + color_range
            + colorprim + colormatrix + transfer + QStringList {"-an","-sn","-f","null", "/dev/null"};
    _preset = _splitParam + codec + level + preset + mode + pass + color_range
            + colorprim + colormatrix + transfer + audio_param + sub_param;
    // DEBUG
    /*
    std::string _presetarray[_preset.length()];
    for (int i = 0; i < _preset.length(); i++)
    {
        _presetarray[i] = _preset[i].toStdString();
    }
    */

    _preset_mkvmerge = max_cll.join(" ") + max_fall.join(" ") + max_lum.join(" ") + min_lum.join(" ") + chroma_coord.join(" ") + white_coord.join(" ");
    Print("Flag two-pass: " << _flag_two_pass);
    Print("Flag HDR: " << _flag_hdr);
    Print("preset_0: " << _preset_0.toStdString());

    QString log("");
    if (_flag_two_pass && _flag_hdr) {
        Print("preset_pass1: " << _preset_pass1.join(" ").toStdString());
        Print("preset: " << _preset.join(" ").toStdString());
        Print("preset_mkvpropedit: " << _preset_mkvmerge.toStdString());
        log = QString("Preset pass 1: %1 -i <input file> %2\n"
                      "Preset pass 2: %3 -i <input file> %4 -y <output file>\n"
                      "Preset mkvpropedit: %5\n")
                .arg(_preset_0, _preset_pass1.join(" "), _preset_0, _preset.join(" "), _preset_mkvmerge);
    }
    else
    if (_flag_two_pass && !_flag_hdr) {
        Print("preset_pass1: " << _preset_pass1.join(" ").toStdString());
        Print("preset: " << _preset.join(" ").toStdString());
        log = QString("Preset pass 1: %1 -i <input file> %2\n"
                      "Preset pass 2: %3 -i <input file> %4 -y <output file>\n")
                .arg(_preset_0, _preset_pass1.join(" "), _preset_0, _preset.join(" "));
    }
    else
    if (!_flag_two_pass && _flag_hdr) {
        Print("preset: " << _preset.join(" ").toStdString());
        Print("preset_mkvpropedit: " << _preset_mkvmerge.toStdString());
        log = QString("Preset: %1 -i <input file> %2 -y <output file>\n"
                      "Preset mkvpropedit: %3\n")
                .arg(_preset_0, _preset.join(" "), _preset_mkvmerge);
    }
    else
    if (!_flag_two_pass && !_flag_hdr) {
        Print("preset: " << _preset.join(" ").toStdString());
        log = QString("Preset: %1 -i <input file> %2 -y <output file>\n")
                .arg(_preset_0, _preset.join(" "));
    }
    emit onEncodingLog(log);
    encode();
}

void Encoder::encode()   // Encode
{
    Print("Encode ...");
    QStringList arguments;
    processEncoding->disconnect();
    connect(processEncoding, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_1()));
    connect(processEncoding, SIGNAL(finished(int)), this, SLOT(completed(int)));
    emit onEncodingProgress(0, 0.0f);

    // DEBUG
    /*
    std::string _extSubPathsarray[_extSubPaths.length()];
    for (int i = 0; i < _extSubPaths.length(); i++)
    {
        _extSubPathsarray[i] = _extSubPaths[i].toStdString();
    }

    std::string _sub_mux_paramarray[_sub_mux_param.length()];
    for (int i = 0; i < _extSubPaths.length(); i++)
    {
        _sub_mux_paramarray[i] = _sub_mux_param[i].toStdString();
    }

    std::string _presetarray[_preset.length()];
    for (int i = 0; i < _preset.length(); i++)
    {
        _presetarray[i] = _preset[i].toStdString();
    }
    */

    if (_mux_mode) {
        Print("Muxing mode ...");
        _encoding_mode = tr("Muxing:");
        emit onEncodingMode(_encoding_mode);
        arguments << "-hide_banner" << "-i" << _temp_file << "-map" << "0:v:0?" << "-map" << "0:a?"
                  << "-map" << "0:s?" << "-movflags" << "+write_colr"
                  << "-c:v" << "copy" << "-c:a" << "copy" << _sub_mux_param << "-y" << _output_file;
    } else {
        if (*fr_count == 0) {
            _message = tr("The file does not contain FPS information!\nSelect the correct input file!");
            emit onEncodingInitError(_message);
            return;
        }
        emit onEncodingStarted();

        _loop_start = time(nullptr);
        if (!_flag_two_pass && !_flag_hdr) {
            Print("Encode non HDR...");
            _encoding_mode = tr("Encoding:");
            emit onEncodingMode(_encoding_mode);
            arguments << _preset_0.split(" ") << "-i" << _input_file
                      << _extAudioPaths
                      << _extSubPaths << _preset << "-y" << _output_file;
        }
        else
        if (!_flag_two_pass && _flag_hdr) {
            Print("Encode HDR...");
            _encoding_mode = tr("Encoding:");
            emit onEncodingMode(_encoding_mode);
            arguments << _preset_0.split(" ") << "-i" << _input_file
                      << _extAudioPaths
                      << _extSubPaths << _preset << "-y" << _temp_file;
        }
        else
        if (_flag_two_pass) {
            Print("Encode 1-st pass...");
            _encoding_mode = tr("1-st pass:");
            emit onEncodingMode(_encoding_mode);
            arguments << _preset_0.split(" ") << "-y" << "-i" << _input_file
                      << _extAudioPaths
                      << _extSubPaths << _preset_pass1;
        }
    }

    // Clean up empty slots.
    arguments.removeAll("");
    arguments.removeAll(" ");

    std::string argsarray[arguments.length()];
    for (int i = 0; i < arguments.length(); i++)
    {
        argsarray[i] = arguments[i].toStdString();
    }

    std::string myargs = arguments.join(" ").toStdString();

    //qDebug() << arguments;
     processEncoding->start("ffmpeg", arguments);
    if (!processEncoding->waitForStarted()) {
        Print("cmd command not found!!!");
        processEncoding->disconnect();
        _message = tr("An unknown error occurred!\n Possible FFMPEG not installed.\n");
        emit onEncodingInitError(_message);
    }
}

void Encoder::add_metadata() // Add metedata
{
    Print("Add metadata ...");
    _mux_mode = true;
    processEncoding->disconnect();
    connect(processEncoding, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_2()));
    connect(processEncoding, SIGNAL(finished(int)), this, SLOT(encode()));
    _encoding_mode = tr("Add data:");
    emit onEncodingMode(_encoding_mode);
    emit onEncodingProgress(0, 0.0f);
    QStringList arguments;
    arguments << "--edit" << "track:1" << _preset_mkvmerge.split(" ") << _temp_file;
    processEncoding->start("mkvpropedit", arguments);
    if (!processEncoding->waitForStarted()) {
        Print("cmd command not found!!!");
        processEncoding->disconnect();
        _message = tr("An unknown error occured!\n Possible mkvtoolnix not installed.\n");
        emit onEncodingInitError(_message);
    }
}

void Encoder::progress_1()   // Progress
{
    QString line = QString(processEncoding->readAllStandardOutput());
    const QString line_mod = line.replace("   ", " ").replace("  ", " ").replace("  ", " ").replace("= ", "=");
    emit onEncodingLog(line_mod);
    _error_message = line_mod;
    const int pos_st = line_mod.indexOf("frame=");
    if (pos_st == 0) {
        QStringList data = line_mod.split(" ");
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
    }
}

void Encoder::progress_2()   // Progress mkvpropedit
{
    const QString line = QString(processEncoding->readAllStandardOutput());
    emit onEncodingLog(line);
    _error_message = line;
    const int pos_st = line.indexOf("Done.");
    const int pos_nf = line.indexOf("Nothing to do.");
    static bool lock = false;
    if ((pos_st != -1) || (pos_nf != -1)) {
        emit onEncodingProgress(100, 0.0f);
        if (!lock) {
            lock = true;
            _loop_start = time(nullptr);
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
    if (processEncoding->state() == QProcess::Running)
        processEncoding->kill();
}

void Encoder::completed(int exit_code)
{
    processEncoding->disconnect();
    if (exit_code == 0) {
        if (_mux_mode) {
            if (_flag_hdr)
                QDir().remove(_temp_file);
            emit onEncodingProgress(100, 0.0f);
            emit onEncodingCompleted();
        } else {
            if (!_flag_two_pass && _flag_hdr) {
                add_metadata();
            } else
            if (!_flag_two_pass && !_flag_hdr) {
                emit onEncodingProgress(100, 0.0f);
                emit onEncodingCompleted();
            } else
            if (_flag_two_pass) {
                _flag_two_pass = false;
                encode();
            }
        }
    } else {
        if (_flag_hdr)
            QDir().remove(_temp_file);
        emit onEncodingError(_error_message);
    }
}

void Encoder::abort()
{
    processEncoding->disconnect();
    if (_flag_hdr)
        QDir().remove(_temp_file);
    emit onEncodingAborted();
}
