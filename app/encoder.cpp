/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: encoder.cpp
 MODIFIED: February, 2022
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "encoder.h"

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
    std::cout << "Make preset..." << std::endl;                       // Debug information //
    _temp_file = temp_file;
    _input_file = input_file;
    _output_file = output_file;
    fr_count = _fr_count;
    int _CODEC = _cur_param[CurParamIndex::CODEC].toInt();
    int _MODE = _cur_param[CurParamIndex::MODE].toInt();
    int _CONTAINER = _cur_param[CurParamIndex::CONTAINER].toInt();
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

    QString width[16] = {
        "Source", "7680", "4520", "4096", "3840", "3656", "2048", "1920",
        "1828",   "1440", "1280", "1024", "768",  "720",  "640",  "320"
    };

    QString height[32] = {
        "Source", "4320", "3112", "3072", "2664", "2540", "2468", "2304",
        "2214",   "2204", "2160", "2056", "1976", "1744", "1556", "1536",
        "1332",   "1234", "1152", "1107", "1102", "1080", "1028", "988",
        "872",    "768",  "720",  "576",  "540",  "486",  "480",  "240"
    };

    QString resize_vf = "";
    QString new_width = (width[_WIDTH] != "Source") ? width[_WIDTH] : _width;
    QString new_height = (height[_HEIGHT] != "Source") ? height[_HEIGHT] : _height;
    if ((width[_WIDTH] != "Source") || (height[_HEIGHT] != "Source")) {
        if (_CODEC >= CODEC_QSV_FIRST && _CODEC <= CODEC_QSV_LAST) { // QSV
            resize_vf = QString("scale_qsv=w=%1:h=%2,setsar=1:1").arg(new_width, new_height);
        }
        else if (_CODEC >= CODEC_VAAPI_FIRST && _CODEC <= CODEC_VAAPI_LAST) { // VAAPI
            resize_vf = QString("scale_vaapi=w=%1:h=%2,setsar=1:1").arg(new_width, new_height);
        }
        else {
            resize_vf = QString("scale=%1:%2,setsar=1:1").arg(new_width, new_height);
        }
    }

    /******************************************* FPS *****************************************/

    QString frame_rate[14] = {
        "Source", "120", "60", "59.940", "50", "48", "30",
        "29.970", "25",  "24", "23.976", "20", "18", "16"
    };

    QString blending[4] = {
        "Simple", "Interpolated", "MCI", "Blend"
    };

    QString fps_vf = "";
    double fps_dest;

    if (frame_rate[_FRAME_RATE] != "Source") {
        fps_dest = frame_rate[_FRAME_RATE].toDouble();
        if (_CODEC >= CODEC_QSV_FIRST && _CODEC <= CODEC_QSV_LAST) { // QSV
            fps_vf = QString("vpp_qsv=framerate=%1").arg(frame_rate[_FRAME_RATE]);
        }
        else if (_CODEC >= CODEC_VAAPI_FIRST && _CODEC <= CODEC_VAAPI_LAST) { // VAAPI
            fps_vf = QString("fps=fps=%1").arg(frame_rate[_FRAME_RATE]);
        }
        else {
            if (blending[_BLENDING] == "Simple") {
                fps_vf = QString("fps=fps=%1").arg(frame_rate[_FRAME_RATE]);
            }
            else if (blending[_BLENDING] == "Interpolated") {
                fps_vf = QString("framerate=fps=%1").arg(frame_rate[_FRAME_RATE]);
            }
            else if (blending[_BLENDING] == "MCI") {
                fps_vf = QString("minterpolate=fps=%1:mi_mode=mci:mc_mode=aobmc:me_mode=bidir:vsbmc=1")
                        .arg(frame_rate[_FRAME_RATE]);
            }
            else if (blending[_BLENDING] == "Blend") {
                fps_vf = QString("minterpolate=fps=%1:mi_mode=blend").arg(frame_rate[_FRAME_RATE]);
            }
        }
    } else {
        fps_dest = _fps.toDouble();
    }

    /****************************************** Split ****************************************/

    QString _splitStartParam = "";
    QString _splitParam = "";

    if (_endTime > 0 && _startTime < _endTime) {
        double duration = _endTime - _startTime;
        *fr_count = static_cast<int>(round(duration * fps_dest));
        int startFrame = static_cast<int>(round(_startTime * fps_dest));
        int endFrame = static_cast<int>(round(_endTime * fps_dest));
        int amountFrames = endFrame - startFrame;
        _splitStartParam = QString(" -ss %1").arg(QString::number(_startTime, 'f', 3));
        _splitParam = QString("-vframes %1 ").arg(QString::number(amountFrames));
    } else {
        *fr_count = static_cast<int>(round(_dur * fps_dest));
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

    /*********************************** Intel QSV presets ************************************/
    QString intelQSV_filter = "hwmap=derive_device=qsv,format=qsv";
#if defined (Q_OS_WIN64)
    QString intelQSVhwaccel = " -hwaccel dxva2 -hwaccel_output_format dxva2_vld";
#elif defined (Q_OS_UNIX)
    QString intelQSVhwaccel = " -hwaccel vaapi -hwaccel_output_format vaapi";
#endif

    /************************************* XDCAM presets **************************************/
    QString xdcam_preset = "-pix_fmt yuv422p -c:v mpeg2video -profile:v 0 -flags ilme -top 1 "
                           "-metadata creation_time=now -vtag xd5c -timecode 01:00:00:00 ";

    /************************************* XAVC presets **************************************/
    QString xavc_preset = "-pix_fmt yuv422p -c:v libx264 -me_method tesa -subq 9 -partitions all -direct-pred auto "
                          "-psy 0 -g 0 -keyint_min 0 -x264opts filler -x264opts force-cfr -tune fastdecode ";

    /************************************* Codec module ***************************************/

    QString arr_codec[NUMBER_PRESETS][4] = {
        {"-pix_fmt yuv420p12le -c:v libx265 -profile:v main12 ",        "",                     "1", ""},
        {"-pix_fmt yuv420p10le -c:v libx265 -profile:v main10 ",        "",                     "1", ""},
        {"-pix_fmt yuv420p -c:v libx265 -profile:v main ",              "",                     "0", ""},
        {"-pix_fmt yuv420p -c:v libx264 -profile:v high ",              "",                     "0", ""},
        {"-pix_fmt yuv420p10le -c:v libvpx-vp9 -speed 4 -profile:v 2 ", "",                     "1", ""},
        {"-pix_fmt yuv420p -c:v libvpx-vp9 -speed 4 ",                  "",                     "0", ""},
        {"-c:v hevc_qsv -profile:v main10 ",                            intelQSVhwaccel,   "1", intelQSV_filter},
        {"-pix_fmt qsv -c:v hevc_qsv -profile:v main ",                 intelQSVhwaccel,   "0", intelQSV_filter},
        {"-pix_fmt qsv -c:v h264_qsv -profile:v high ",                 intelQSVhwaccel,   "0", intelQSV_filter},
        {"-c:v vp9_qsv -profile:v 2 ",                                  intelQSVhwaccel,   "1", intelQSV_filter},
        {"-pix_fmt qsv -c:v vp9_qsv ",                                  intelQSVhwaccel,   "0", intelQSV_filter},
        {"-pix_fmt qsv -c:v mpeg2_qsv -profile:v high ",                intelQSVhwaccel,   "0", intelQSV_filter},
        {"-c:v h264_vaapi -profile:v high ",                 " -hwaccel vaapi -hwaccel_output_format vaapi",   "0", "format=nv12|vaapi,hwupload"}, // Intel VAAPI h264
        {"-pix_fmt p010le -c:v hevc_nvenc -profile:v main10 ",          " -hwaccel cuda",       "1", ""},
        {"-pix_fmt yuv420p -c:v hevc_nvenc -profile:v main ",           " -hwaccel cuda",       "0", ""},
        {"-pix_fmt yuv420p -c:v h264_nvenc -profile:v high ",           " -hwaccel cuda",       "0", ""},
        {"-pix_fmt yuv422p10le -c:v prores_ks -profile:v 0 ",           "",                     "1", ""},
        {"-pix_fmt yuv422p10le -c:v prores_ks -profile:v 1 ",           "",                     "1", ""},
        {"-pix_fmt yuv422p10le -c:v prores_ks -profile:v 2 ",           "",                     "1", ""},
        {"-pix_fmt yuv422p10le -c:v prores_ks -profile:v 3 ",           "",                     "1", ""},
        {"-pix_fmt yuv444p10le -c:v prores_ks -profile:v 4 ",           "",                     "1", ""},
        {"-pix_fmt yuv444p10le -c:v prores_ks -profile:v 5 ",           "",                     "1", ""},
        {"-pix_fmt yuv422p -c:v dnxhd -profile:v dnxhr_lb ",            "",                     "0", ""},
        {"-pix_fmt yuv422p -c:v dnxhd -profile:v dnxhr_sq ",            "",                     "0", ""},
        {"-pix_fmt yuv422p -c:v dnxhd -profile:v dnxhr_hq ",            "",                     "0", ""},
        {"-pix_fmt yuv422p10le -c:v dnxhd -profile:v dnxhr_hqx ",       "",                     "1", ""},
        {"-pix_fmt yuv444p10le -c:v dnxhd -profile:v dnxhr_444 ",       "",                     "1", ""},
        {xdcam_preset,                                                  "",                     "0", ""},
        {xavc_preset,                                                   " -guess_layout_max 0", "0", ""},
        {"-movflags +write_colr -c:v copy ",                            "",                     "1", ""}
    };

    QString hwaccel = arr_codec[_CODEC][1];
    QString hwaccel_filter_vf = arr_codec[_CODEC][3];
    _flag_hdr = static_cast<bool>(arr_codec[_CODEC][2].toInt());

    /************************************* Level module **************************************/

    QString arr_level[NUMBER_PRESETS][21] = {
        {"Auto", "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "1b", "1.1", "1.2", "1.3", "2", "2.1", "2.2", "3",   "3.1", "3.2", "4",   "4.1", "4.2", "5", "5.1", "5.2", "6", "6.1", "6.2"},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "1b", "1.1", "1.2", "1.3", "2", "2.1", "2.2", "3",   "3.1", "3.2", "4",   "4.1", "4.2", "5", "5.1", "5.2", "6", "6.1", "6.2"},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "1b", "1.1", "1.2", "1.3", "2", "2.1", "2.2", "3",   "3.1", "3.2", "4",   "4.1", "4.2", "5", "5.1", "5.2", "6", "6.1", "6.2"}, // Intel VAAPI h264
        {"Auto", "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "1b", "1.1", "1.2", "1.3", "2", "2.1", "2.2", "3",   "3.1", "3.2", "4",   "4.1", "4.2", "5", "5.1", "5.2", "6", "6.1", "6.2"},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"2",    "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"5.2",  "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""}
    };

    QString level = "";
    QString selected_level = arr_level[_CODEC][_LEVEL];
    if (selected_level != "" && selected_level != "Auto") {
        level = QString("-level:v %1 ").arg(selected_level);
    }

    /************************************* Mode module ***************************************/

    QString arr_mode[NUMBER_PRESETS][5] = {
        {"CBR",    "ABR", "VBR", "CRF", "CQP"},
        {"CBR",    "ABR", "VBR", "CRF", "CQP"},
        {"CBR",    "ABR", "VBR", "CRF", "CQP"},
        {"CBR",    "ABR", "VBR", "CRF", "CQP"},
        {"ABR",    "CRF", "",    "",    ""},
        {"ABR",    "CRF", "",    "",    ""},
        {"VBR",    "",    "",    "",    ""},
        {"VBR",    "",    "",    "",    ""},
        {"VBR",    "CQP", "",    "",    ""},
        {"ABR",    "CRF", "",    "",    ""},
        {"ABR",    "CRF", "",    "",    ""},
        {"VBR",    "",    "",    "",    ""},
        {"VBR",    "CQP", "",    "",    ""}, // Intel VAAPI
        {"VBR_NV", "",    "",    "",    ""},
        {"VBR_NV", "",    "",    "",    ""},
        {"VBR_NV", "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"VBR",    "",    "",    "",    ""},
        {"CBR",    "",    "",    "",    ""},
        {"",       "",    "",    "",    ""}
    };
    QString mode = "";
    QString bitrate = QString::number(1000000.0*_BQR.toDouble(), 'f', 0);
    QString minrate = QString::number(1000000.0*_MINRATE.toDouble(), 'f', 0);
    QString maxrate = QString::number(1000000.0*_MAXRATE.toDouble(), 'f', 0);
    QString bufsize = QString::number(1000000.0*_BUFSIZE.toDouble(), 'f', 0);
    QString selected_mode = arr_mode[_CODEC][_MODE];

    if (selected_mode == "CBR") {
        mode = QString("-b:v %1 -minrate %1 -maxrate %1 -bufsize %2 ").arg(bitrate, bufsize);
    }
    else if (selected_mode == "ABR") {
        mode = QString("-b:v %1 ").arg(bitrate);
    }
    else if (selected_mode == "VBR") {
        mode = QString("-b:v %1 -minrate %2 -maxrate %3 -bufsize %4 ").arg(bitrate, minrate, maxrate, bufsize);
    }
    else if (selected_mode == "VBR_NV") {
        mode = QString("-b:v %1 -minrate %2 -maxrate %3 -bufsize %4 -rc vbr_hq ").arg(bitrate, minrate, maxrate, bufsize);
    }
    else if (selected_mode == "CRF") {
        mode = QString("-crf %1 ").arg(_BQR);
    }
    else if (selected_mode == "CQP") {
        if (_CODEC >= CODEC_QSV_FIRST && _CODEC <= CODEC_QSV_LAST) { // QSV
            mode = QString("-global_quality %1 -look_ahead 1 ").arg(_BQR);
        } else if (_CODEC >= CODEC_VAAPI_FIRST && _CODEC <= CODEC_VAAPI_LAST) { // VAAPI
            mode = QString("-qp %1 -rc_mode 4 ").arg(_BQR);
        } else {
            mode = QString("-b:v 0 -cq %1 -qmin %1 -qmax %1 ").arg(_BQR);
        }
    }

    /************************************* Preset module ***************************************/

    QString arr_preset[NUMBER_PRESETS][10] = {
        {"None", "Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow",     "Slower", "Veryslow"},
        {"None", "Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow",     "Slower", "Veryslow"},
        {"None", "Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow",     "Slower", "Veryslow"},
        {"None", "Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow",     "Slower", "Veryslow"},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "Veryfast",  "Faster",    "Fast",     "Medium", "Slow", "Slower", "Veryslow", "",       ""},
        {"None", "Veryfast",  "Faster",    "Fast",     "Medium", "Slow", "Slower", "Veryslow", "",       ""},
        {"None", "Veryfast",  "Faster",    "Fast",     "Medium", "Slow", "Slower", "Veryslow", "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "Veryfast",  "Faster",    "Fast",     "Medium", "Slow", "Slower", "Veryslow", "",       ""},
        {"None", "Veryfast",  "Faster",    "Fast",     "Medium", "Slow", "Slower", "Veryslow", "",       ""}, // Intel VAAPI h264
        {"None", "Slow",      "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "Slow",      "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "Slow",      "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow",     "Slower", "Veryslow"},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""}
    };
    QString preset = "";
    QString selected_preset = arr_preset[_CODEC][_PRESET];
    if (selected_preset != "" && selected_preset != "None") {
        preset = QString("-preset ") + selected_preset.toLower() + QString(" ");
    }

    /************************************* Pass module ***************************************/

    QString arr_pass[NUMBER_PRESETS][2] = {
        {"",                     "-x265-params pass=2 "},
        {"",                     "-x265-params pass=2 "},
        {"",                     "-x265-params pass=2 "},
        {"",                     "-pass 2 "},
        {"",                     "-pass 2 "},
        {"",                     "-pass 2 "},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""}, // Intel VAAPI h264
        {"-2pass 1 ",            ""},
        {"-2pass 1 ",            ""},
        {"-2pass 1 ",            ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""}
    };
    QString pass1 = "";
    QString pass = arr_pass[_CODEC][_PASS];
    if (pass == "-x265-params pass=2 ") {
        pass1 = "-x265-params pass=1 ";
        _flag_two_pass = true;
    }
    if (pass == "-pass 2 ") {
        pass1 = "-pass 1 ";
        _flag_two_pass = true;
    }

    /************************************* Audio module ***************************************/

    QString arr_acodec[NUMBER_PRESETS][6] = {
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"Opus",  "Vorbis", "Source", "",       "",     ""},
        {"Opus",  "Vorbis", "Source", "",       "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"Opus",  "Vorbis", "Source", "",       "",     ""},
        {"Opus",  "Vorbis", "Source", "",       "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""}, // Intel VAAPI h264
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "",       "",       "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"AAC",   "AC3",    "DTS",    "Vorbis", "Opus", "Source"}
    };

    QString arr_bitrate[5][17] = {
        {"384k",  "320k",  "256k",  "192k",  "128k",  "96k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AAC
        {"640k",  "448k",  "384k",  "256k",  "",      "",      "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AC3
        {"3840k", "3072k", "2048k", "1920k", "1536k", "1472k", "1344k", "1280k", "1152k", "1024k", "960k", "768k", "640k", "576k", "512k", "448k", "384k"}, // DTS
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // Vorbis
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""} // Opus
    };

    QString arr_sampling[12] = {
        "Source", "8000",  "11025", "16000", "22050",  "32000",
        "44100",  "48000", "88200", "96000", "176400", "192000"
    };

    QString arr_channels[3] = {
        "Source", "1", "2"
    };

    QString acodec = "";
    QString selected_acodec = arr_acodec[_CODEC][_AUDIO_CODEC];
    QString selected_bitrate = "";

    QString sampling = "";
    QString selected_sampling = arr_sampling[_AUDIO_SAMPLING];
    if (selected_sampling != "Source") {
        sampling = QString("-af aresample=%1:resampler=soxr ").arg(selected_sampling);
    }

    QString channels = "";
    QString selected_channels = arr_channels[_AUDIO_CHANNELS];
    if (selected_channels != "Source") {
        channels = QString(" -ac %1").arg(selected_channels);
    }


    if (selected_acodec == "AAC") {
        selected_bitrate = arr_bitrate[0][_AUDIO_BITRATE];
        acodec = QString("-c:a aac -b:a %1").arg(selected_bitrate);
    }
    else if (selected_acodec == "AC3") {
        selected_bitrate = arr_bitrate[1][_AUDIO_BITRATE];
        acodec = QString("-c:a ac3 -b:a %1").arg(selected_bitrate);
    }
    else if (selected_acodec == "DTS") {
        selected_bitrate = arr_bitrate[2][_AUDIO_BITRATE];
        acodec = QString("-strict -2 -c:a dca -b:a %1").arg(selected_bitrate);
    }
    else if (selected_acodec == "Vorbis") {
        selected_bitrate = arr_bitrate[3][_AUDIO_BITRATE];
        acodec = QString("-c:a libvorbis -b:a %1").arg(selected_bitrate);
    }
    else if (selected_acodec == "Opus") {
        selected_bitrate = arr_bitrate[4][_AUDIO_BITRATE];
        acodec = QString("-c:a libopus -b:a %1").arg(selected_bitrate);
    }
    else if (selected_acodec == "PCM16") {
        acodec = "-c:a pcm_s16le";
    }
    else if (selected_acodec == "PCM24") {
        acodec = "-c:a pcm_s24le";
    }
    else if (selected_acodec == "PCM32") {
        acodec = "-c:a pcm_s32le";
    }
    else if (selected_acodec == "Source") {
        acodec = "-c:a copy";
    }
    QString audio_param = sampling + acodec + channels;

    /************************************ Subtitle module *************************************/

    QString sub_param("");
    if (container == "MKV") {
        _sub_mux_param = QString("-c:s copy");
    }
    else if (container == "WebM") {
        _sub_mux_param = QString("-c:s webvtt");
    }
    else if (container == "MP4" || container == "MOV") {
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

    QString arr_colorprim[11] = {
        "Source",    "bt470m",   "bt470bg",  "bt709",    "bt2020", "smpte170m",
        "smpte240m", "smpte428", "smpte431", "smpte432", "film"
    };
    QMap<QString, QString> curr_colorprim = {
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
    QString selected_colorprim = arr_colorprim[_PRIMARY];
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

    QString arr_colormatrix[14] = {
        "Source", "bt470bg", "bt709", "bt2020nc", "bt2020c", "smpte170m", "smpte240m",
        "smpte2085", "chroma-derived-nc", "chroma-derived-c", "fcc", "GBR", "ICtCp", "YCgCo"
    };
    QMap<QString, QString> curr_colormatrix = {
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
    QString selected_colormatrix = arr_colormatrix[_MATRIX];
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

    QString arr_trc[17] = {
        "Source", "bt470m", "bt470bg", "bt709", "bt1361e", "bt2020-10", "bt2020-12", "smpte170m",
        "smpte240m", "smpte428", "smpte2084", "arib-std-b67", "linear", "log100", "log316",
        "iec61966-2-1", "iec61966-2-4"
    };
    QMap<QString, QString> curr_transfer = {
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
    QString selected_transfer = arr_trc[_TRC];
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
    QString vf_transform_arr[vf_size] = {
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

    QString codec = QString("-map 0:v:0? ") + _audioMapParam + _subtitleMapParam +
                    QString("-map_metadata -1 ") + _videoMetadataParam + _audioMetadataParam +
                    _subtitleMetadataParam + transform + arr_codec[_CODEC][0];

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
            if (_hdr[CUR_COLOR_RANGE] == "Limited") {
                color_range = "-color_range tv ";
            }
            else if (_hdr[CUR_COLOR_RANGE] == "Full") {
                color_range = "-color_range pc ";
            }
        }
        else if (_COLOR_RANGE == 1) {
            color_range = "-color_range pc ";
        }
        else if (_COLOR_RANGE == 2) {
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

        QString chroma_coord_curr_red_x = "";
        QString chroma_coord_curr_red_y = "";
        QString chroma_coord_curr_green_x = "";
        QString chroma_coord_curr_green_y = "";
        QString chroma_coord_curr_blue_x = "";
        QString chroma_coord_curr_blue_y = "";
        QString white_coord_curr_x = "";
        QString white_coord_curr_y = "";
        if (_MASTER_DISPLAY == 0) {     // From source
            if (_hdr[CUR_MASTER_DISPLAY] == "Display P3") {
                chroma_coord_curr_red_x = "0.680";
                chroma_coord_curr_red_y = "0.320";
                chroma_coord_curr_green_x = "0.265";
                chroma_coord_curr_green_y = "0.690";
                chroma_coord_curr_blue_x = "0.150";
                chroma_coord_curr_blue_y = "0.060";
                white_coord_curr_x = "0.3127";
                white_coord_curr_y = "0.3290";
            }
            if (_hdr[CUR_MASTER_DISPLAY] == "DCI P3") {
                chroma_coord_curr_red_x = "0.680";
                chroma_coord_curr_red_y = "0.320";
                chroma_coord_curr_green_x = "0.265";
                chroma_coord_curr_green_y = "0.690";
                chroma_coord_curr_blue_x = "0.150";
                chroma_coord_curr_blue_y = "0.060";
                white_coord_curr_x = "0.314";
                white_coord_curr_y = "0.3510";
            }
            if (_hdr[CUR_MASTER_DISPLAY] == "BT.2020") {
                chroma_coord_curr_red_x = "0.708";
                chroma_coord_curr_red_y = "0.292";
                chroma_coord_curr_green_x = "0.170";
                chroma_coord_curr_green_y = "0.797";
                chroma_coord_curr_blue_x = "0.131";
                chroma_coord_curr_blue_y = "0.046";
                white_coord_curr_x = "0.3127";
                white_coord_curr_y = "0.3290";
            }
            if (_hdr[CUR_MASTER_DISPLAY] == "BT.709") {
                chroma_coord_curr_red_x = "0.640";
                chroma_coord_curr_red_y = "0.330";
                chroma_coord_curr_green_x = "0.30";
                chroma_coord_curr_green_y = "0.60";
                chroma_coord_curr_blue_x = "0.150";
                chroma_coord_curr_blue_y = "0.060";
                white_coord_curr_x = "0.3127";
                white_coord_curr_y = "0.3290";
            }
            if (_hdr[CUR_MASTER_DISPLAY] == "Undefined") {
                QStringList chr = _hdr[CUR_CHROMA_COORD].split(",");
                if (chr.size() == 6) {
                    chroma_coord_curr_red_x = chr[0];
                    chroma_coord_curr_red_y = chr[1];
                    chroma_coord_curr_green_x = chr[2];
                    chroma_coord_curr_green_y = chr[3];
                    chroma_coord_curr_blue_x = chr[4];
                    chroma_coord_curr_blue_y = chr[5];
                } else {
                    _message = tr("Incorrect master display chroma coordinates source parameters!");
                    emit onEncodingInitError(_message);
                    return;
                }
                QStringList wht = _hdr[CUR_WHITE_COORD].split(",");
                if (wht.size() == 2) {
                    white_coord_curr_x = wht[0];
                    white_coord_curr_y = wht[1];
                } else {
                    _message = tr("Incorrect master display white point coordinates source parameters!");
                    emit onEncodingInitError(_message);
                    return;
                }
            }
        }
        if (_MASTER_DISPLAY == 1) {     // Display P3
            chroma_coord_curr_red_x = "0.680";
            chroma_coord_curr_red_y = "0.320";
            chroma_coord_curr_green_x = "0.265";
            chroma_coord_curr_green_y = "0.690";
            chroma_coord_curr_blue_x = "0.150";
            chroma_coord_curr_blue_y = "0.060";
            white_coord_curr_x = "0.3127";
            white_coord_curr_y = "0.3290";
        }
        if (_MASTER_DISPLAY == 2) {     // DCI P3
            chroma_coord_curr_red_x = "0.680";
            chroma_coord_curr_red_y = "0.320";
            chroma_coord_curr_green_x = "0.265";
            chroma_coord_curr_green_y = "0.690";
            chroma_coord_curr_blue_x = "0.150";
            chroma_coord_curr_blue_y = "0.060";
            white_coord_curr_x = "0.314";
            white_coord_curr_y = "0.3510";
        }
        if (_MASTER_DISPLAY == 3) {     // BT.2020
            chroma_coord_curr_red_x = "0.708";
            chroma_coord_curr_red_y = "0.292";
            chroma_coord_curr_green_x = "0.170";
            chroma_coord_curr_green_y = "0.797";
            chroma_coord_curr_blue_x = "0.131";
            chroma_coord_curr_blue_y = "0.046";
            white_coord_curr_x = "0.3127";
            white_coord_curr_y = "0.3290";
        }
        if (_MASTER_DISPLAY == 4) {     // BT.709
            chroma_coord_curr_red_x = "0.640";
            chroma_coord_curr_red_y = "0.330";
            chroma_coord_curr_green_x = "0.30";
            chroma_coord_curr_green_y = "0.60";
            chroma_coord_curr_blue_x = "0.150";
            chroma_coord_curr_blue_y = "0.060";
            white_coord_curr_x = "0.3127";
            white_coord_curr_y = "0.3290";
        }
        if (_MASTER_DISPLAY == 5) {     // Custom
            QStringList chr = _CHROMA_COORD.split(",");
            if (chr.size() == 6) {
                chroma_coord_curr_red_x = chr[0];
                chroma_coord_curr_red_y = chr[1];
                chroma_coord_curr_green_x = chr[2];
                chroma_coord_curr_green_y = chr[3];
                chroma_coord_curr_blue_x = chr[4];
                chroma_coord_curr_blue_y = chr[5];
            }
            QStringList wht = _WHITE_COORD.split(",");
            if (wht.size() == 2) {
                white_coord_curr_x = wht[0];
                white_coord_curr_y = wht[1];
            }
        }

        if (chroma_coord_curr_red_x == "") {
            chroma_coord = "-d chromaticity-coordinates-red-x -d chromaticity-coordinates-red-y "
                           "-d chromaticity-coordinates-green-x -d chromaticity-coordinates-green-y "
                           "-d chromaticity-coordinates-blue-x -d chromaticity-coordinates-blue-y ";
        } else {
            chroma_coord = QString("-s chromaticity-coordinates-red-x=%1 -s chromaticity-coordinates-red-y=%2 "
                                   "-s chromaticity-coordinates-green-x=%3 -s chromaticity-coordinates-green-y=%4 "
                                   "-s chromaticity-coordinates-blue-x=%5 -s chromaticity-coordinates-blue-y=%6 ")
                                   .arg(chroma_coord_curr_red_x, chroma_coord_curr_red_y, chroma_coord_curr_green_x,
                                        chroma_coord_curr_green_y, chroma_coord_curr_blue_x, chroma_coord_curr_blue_y);
        }
        if (white_coord_curr_x == "") {
            white_coord = "-d white-coordinates-x -d white-coordinates-y ";
        } else {
            white_coord = QString("-s white-coordinates-x=%1 -s white-coordinates-y=%2 ").arg(white_coord_curr_x, white_coord_curr_y);
        }
    }

    /************************************* Result module ***************************************/

    _preset_0 = "-hide_banner" + hwaccel + _splitStartParam;
    _preset_pass1 = _splitParam + codec + level + preset + mode + pass1 + color_range
            + colorprim + colormatrix + transfer + "-an -sn -f null /dev/null";
    _preset = _splitParam + codec + level + preset + mode + pass + color_range
            + colorprim + colormatrix + transfer + audio_param + sub_param;
    _preset_mkvmerge = QString("%1%2%3%4%5%6 ").arg(max_cll, max_fall, max_lum, min_lum, chroma_coord, white_coord);
    std::cout << "Flag two-pass: " << _flag_two_pass << std::endl;
    std::cout << "Flag HDR: " << _flag_hdr << std::endl;
    std::cout << "preset_0: " << _preset_0.toStdString() << std::endl;
    QString log("");
    if ((_flag_two_pass == true) && (_flag_hdr == true)) {
        std::cout << "preset_pass1: " << _preset_pass1.toStdString() << std::endl;
        std::cout << "preset: " << _preset.toStdString() << std::endl;
        std::cout << "preset_mkvpropedit: " << _preset_mkvmerge.toStdString() << std::endl;
        log = QString("Preset pass 1: ") + _preset_0 + QString(" -i <input file> ") + _preset_pass1 + QString("\n") +
              QString("Preset pass 2: ") + _preset_0 + QString(" -i <input file> ") + _preset  + QString(" -y <output file>\n") +
              QString("Preset mkvpropedit: ") + _preset_mkvmerge  + QString("\n");
    }
    else if ((_flag_two_pass == true) && (_flag_hdr == false)) {
        std::cout << "preset_pass1: " << _preset_pass1.toStdString() << std::endl;
        std::cout << "preset: " << _preset.toStdString() << std::endl;
        log = QString("Preset pass 1: ") + _preset_0 + QString(" -i <input file> ") + _preset_pass1 + QString("\n") +
              QString("Preset pass 2: ") + _preset_0 + QString(" -i <input file> ") + _preset  + QString(" -y <output file>\n");
    }
    else if ((_flag_two_pass == false) && (_flag_hdr == true)) {
        std::cout << "preset: " << _preset.toStdString() << std::endl;
        std::cout << "preset_mkvpropedit: " << _preset_mkvmerge.toStdString() << std::endl;
        log = QString("Preset: ") + _preset_0 + QString(" -i <input file> ") + _preset  + QString(" -y <output file>\n") +
              QString("Preset mkvpropedit: ") + _preset_mkvmerge  + QString("\n");
    }
    else if ((_flag_two_pass == false) && (_flag_hdr == false)) {
        std::cout << "preset: " << _preset.toStdString() << std::endl;
        log = QString("Preset: ") + _preset_0 + QString(" -i <input file> ") + _preset  + QString(" -y <output file>\n");
    }
    emit onEncodingLog(log);
    encode();
}

void Encoder::encode()   /*** Encode ***/
{
    std::cout << "Encode ..." << std::endl;  //  Debug info //
    QStringList arguments;
    _calling_pr_1 = true;
    processEncoding->disconnect();
    connect(processEncoding, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_1()));
    connect(processEncoding, SIGNAL(finished(int)), this, SLOT(error()));
    emit onEncodingProgress(0, 0.0f);
    if (_mux_mode == true) {
        std::cout << "Muxing mode ..." << std::endl;  //  Debug info //
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
        if (_flag_two_pass == false && _flag_hdr == false) {
            std::cout << "Encode non HDR..." << std::endl;  //  Debug info //
            _encoding_mode = tr("Encoding:");
            emit onEncodingMode(_encoding_mode);
            arguments << _preset_0.split(" ") << "-i" << _input_file << _preset.split(" ") << "-y" << _output_file;
        }
        else if (_flag_two_pass == false && _flag_hdr == true) {
            std::cout << "Encode HDR..." << std::endl;  //  Debug info //
            _encoding_mode = tr("Encoding:");
            emit onEncodingMode(_encoding_mode);
            arguments << _preset_0.split(" ") << "-i" << _input_file << _preset.split(" ") << "-y" << _temp_file;
        }
        else if (_flag_two_pass == true) {
            std::cout << "Encode 1-st pass..." << std::endl;  //  Debug info //
            _encoding_mode = tr("1-st pass:");
            emit onEncodingMode(_encoding_mode);
            arguments << _preset_0.split(" ") << "-y" << "-i" << _input_file << _preset_pass1.split(" ");
        }
    }
    //qDebug() << arguments;
    processEncoding->start("ffmpeg", arguments);
    if (!processEncoding->waitForStarted()) {
        std::cout << "cmd command not found!!!" << std::endl;
        processEncoding->disconnect();
        _message = tr("An unknown error occurred!\n Possible FFMPEG not installed.\n");
        emit onEncodingInitError(_message);
    }
}

void Encoder::add_metadata() /*** Add metedata ***/
{
    std::cout << "Add metadata ..." << std::endl;  //  Debug info //
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
        std::cout << "cmd command not found!!!" << std::endl;
        processEncoding->disconnect();
        _message = tr("An unknown error occured!\n Possible mkvtoolnix not installed.\n");
        emit onEncodingInitError(_message);
    }
}

void Encoder::progress_1()   /*** Progress 1 ***/
{
    QString line = processEncoding->readAllStandardOutput();
    QString line_mod6 = line.replace("   ", " ").replace("  ", " ").replace("  ", " ").replace("= ", "=");
    emit onEncodingLog(line_mod6);
    int pos_err_1 = line_mod6.indexOf("[error]:");
    int pos_err_2 = line_mod6.indexOf("Error");
    int pos_err_3 = line_mod6.indexOf(" @ ");
    if (pos_err_1 != -1) {
        QStringList error = line_mod6.split(":");
        if (error.size() >= 2) {
            _error_message = error[1];
        }
    }
    if (pos_err_2 != -1) {
        _error_message = line_mod6;
    }
    if (pos_err_3 != -1) {
        QStringList error = line_mod6.split("]");
        if (error.size() >= 2) {
            _error_message = error[1];
        }
    }
    int pos_st = line_mod6.indexOf("frame=");
    if (pos_st == 0) {
        QStringList data = line_mod6.split(" ");
        QString frame_qstr = data[0].replace("frame=", "");
        int frame = frame_qstr.toInt();
        if (frame == 0) {
            frame = 1;
        }
        time_t iter_start = time(nullptr);
        int timer = static_cast<int>(iter_start - _loop_start);
        float full_time = static_cast<float>(timer * (*fr_count)) / (frame);
        float rem_time = full_time - static_cast<float>(timer);
        if (rem_time < 0.0f) {
            rem_time = 0.0f;
        }
        if (rem_time > MAXIMUM_ALLOWED_TIME) {
            rem_time = MAXIMUM_ALLOWED_TIME;
        }

        float percent = static_cast<float>(frame * 100) / *fr_count;
        int percent_int = static_cast<int>(round(percent));
        if (percent_int > 100) {
            percent_int = 100;
        }
        emit onEncodingProgress(percent_int, rem_time);

        if ((percent_int >= 95) && (_calling_pr_1 == true)) {
             disconnect(processEncoding, SIGNAL(finished(int)), this, SLOT(error()));
             if (_mux_mode == true) {
                 connect(processEncoding, SIGNAL(finished(int)), this, SLOT(completed()));
             } else {
                 if (_flag_two_pass == false && _flag_hdr == true) {
                     disconnect(processEncoding, SIGNAL(finished(int)), this, SLOT(encode()));
                     connect(processEncoding, SIGNAL(finished(int)), this, SLOT(add_metadata()));
                 }
                 if (_flag_two_pass == false && _flag_hdr == false) {
                     disconnect(processEncoding, SIGNAL(finished(int)), this, SLOT(encode()));
                     connect(processEncoding, SIGNAL(finished(int)), this, SLOT(completed()));
                 }
                 if (_flag_two_pass == true) {
                     connect(processEncoding, SIGNAL(finished(int)), this, SLOT(encode()));
                     _flag_two_pass = false;
                 }
             }
             _calling_pr_1 = false;
        }
    }
}

void Encoder::progress_2()   /*** Progress 2 ***/
{
    QString line = processEncoding->readAllStandardOutput();
    emit onEncodingLog(line);
    int pos_st = line.indexOf("Done.");
    int pos_nf = line.indexOf("Nothing to do.");
    if ((pos_st != -1) or (pos_nf != -1)) {
        int percent = 100;
        emit onEncodingProgress(percent, 0.0f);
        if ((percent == 100) && (_calling_pr_1 == true)) {
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
    if (_flag_hdr == true) {
        QDir().remove(_temp_file);
    }
    emit onEncodingCompleted();
}

void Encoder::abort()
{
    processEncoding->disconnect();
    if (_flag_hdr == true) {
        QDir().remove(_temp_file);
    }
    emit onEncodingAborted();
}

void Encoder::error()
{
    processEncoding->disconnect();
    if (_flag_hdr == true) {
        QDir().remove(_temp_file);
    }
    emit onEncodingError(_error_message);
}
