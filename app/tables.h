#ifndef TABLES_H
#define TABLES_H

#include <QObject>
#include <QMap>
#include "constants.h"

using namespace Constants;

class Tables : public QObject
{
    Q_OBJECT
public:

    const QString aspect_ratio[34][2] = {
        {tr("Source"), tr("Source")}, // Source
        {"640",    "480"},    // 640x480
        {"720",    "480"},    // 720x480
        {"720",    "576"},    // 720x576
        {"720",    "486"},    // 720x486
        {"720",    "540"},    // 720x540
        {"768",    "576"},    // 768x576
        {"1280",   "720"},    // 1280x720
        {"1920",   "1080"},   // 1920x1080
        {"1440",   "1080"},   // 1440x1080
        {"1828",   "988"},    // 1828x988
        {"1828",   "1028"},   // 1828x1028
        {"1828",   "1102"},   // 1828x1102
        {"1828",   "1332"},   // 1828x1332
        {"1828",   "1556"},   // 1828x1556
        {"2048",   "872"},    // 2048x872
        {"2048",   "1107"},   // 2048x1107
        {"2048",   "1152"},   // 2048x1152
        {"2048",   "1234"},   // 2048x1234
        {"2048",   "1536"},   // 2048x1536
        {"3656",   "1976"},   // 3656x1976
        {"3656",   "2056"},   // 3656x2056
        {"3656",   "2204"},   // 3656x2204
        {"3656",   "2664"},   // 3656x2664
        {"3656",   "3112"},   // 3656x3112
        {"4096",   "1744"},   // 4096x1744
        {"4096",   "2214"},   // 4096x2214
        {"4096",   "2304"},   // 4096x2304
        {"4096",   "2468"},   // 4096x2468
        {"4096",   "3072"},   // 4096x3072
        {"3840",   "2160"},   // 3840x2160
        {"4520",   "2540"},   // 4520x2540
        {"7680",   "4320"},   // 7680x4320
        {tr("Custom"), tr("Custom")}  // Custom
    };

    const QString arr_width[16] = {
        "Source", "7680", "4520", "4096", "3840", "3656", "2048", "1920",
        "1828",   "1440", "1280", "1024", "768",  "720",  "640",  "320"
    };

    const QString arr_height[34] = {
        "Source", "4320", "3112", "3072", "2664", "2540", "2468", "2304",
        "2214",   "2204", "2160", "2056", "1976", "1744", "1716", "1556",
        "1536",   "1332", "1234", "1152", "1107", "1102", "1080", "1028",
        "988",    "872",  "804",  "768",  "720",  "576",  "540",  "486",
        "480",    "240"
    };

    const QString frame_rate[14] = {
        "Source", "120", "60", "59.940", "50", "48", "30",
        "29.970", "25",  "24", "23.976", "20", "18", "16"
    };

    enum Blending {
        Simple, Interpolated, MCI, Blend
    };

    const QString arr_codec[NUMBER_PRESETS][3] = {
        {tr("H.265/HEVC 4:2:0 12 bit"),           tr("YUV, 4:2:0, 12 bit"), "HDR: "},
        {tr("H.265/HEVC 4:2:0 10 bit"),           tr("YUV, 4:2:0, 10 bit"), "HDR: "},
        {tr("H.265/HEVC 4:2:0 8 bit"),            tr("YUV, 4:2:0, 8 bit"),  ""},
        {tr("H.264/AVC 4:2:0 8 bit"),             tr("YUV, 4:2:0, 8 bit"),  ""},
        {tr("VP9 4:2:0 10 bit"),                  tr("YUV, 4:2:0, 10 bit"), "HDR: "},
        {tr("VP9 4:2:0 8 bit"),                   tr("YUV, 4:2:0, 8 bit"),  ""},
        {tr("Intel QSV H.265/HEVC 4:2:0 10 bit"), tr("YUV, 4:2:0, 10 bit"), "HDR: "},
        {tr("Intel QSV H.265/HEVC 4:2:0 8 bit"),  tr("YUV, 4:2:0, 8 bit"),  ""},
        {tr("Intel QSV H.264/AVC 4:2:0 8 bit"),   tr("YUV, 4:2:0, 8 bit"),  ""},
        {tr("Intel QSV VP9 4:2:0 10 bit"),        tr("YUV, 4:2:0, 10 bit"), "HDR: "},
        {tr("Intel QSV VP9 4:2:0 8 bit"),         tr("YUV, 4:2:0, 8 bit"),  ""},
        {tr("Intel QSV MPEG-2 4:2:0 8 bit"),      tr("YUV, 4:2:0, 8 bit"),  ""},
        {tr("Intel VAAPI H.264/AVC 4:2:0 8 bit"), tr("YUV, 4:2:0, 8 bit"),  ""}, // Intel VAAPI h264
        {tr("NVENC H.265/HEVC 4:2:0 10 bit"),     tr("YUV, 4:2:0, 10 bit"), "HDR: "},
        {tr("NVENC H.265/HEVC 4:2:0 8 bit"),      tr("YUV, 4:2:0, 8 bit"),  ""},
        {tr("NVENC H.264/AVC 4:2:0 8 bit"),       tr("YUV, 4:2:0, 8 bit"),  ""},
        {tr("ProRes Proxy"),                      tr("YUV, 4:2:2, 10 bit"), "HDR: "},
        {"ProRes LT",                             tr("YUV, 4:2:2, 10 bit"), "HDR: "},
        {tr("ProRes Standard"),                   tr("YUV, 4:2:2, 10 bit"), "HDR: "},
        {"ProRes HQ",                             tr("YUV, 4:2:2, 10 bit"), "HDR: "},
        {"ProRes 4444",                           tr("YUV, 4:4:4, 10 bit"), "HDR: "},
        {"ProRes 4444XQ",                         tr("YUV, 4:4:4, 10 bit"), "HDR: "},
        {"DNxHR LB",                              tr("YUV, 4:2:2, 8 bit"),  ""},
        {"DNxHR SQ",                              tr("YUV, 4:2:2, 8 bit"),  ""},
        {"DNxHR HQ",                              tr("YUV, 4:2:2, 8 bit"),  ""},
        {"DNxHR HQX",                             tr("YUV, 4:2:2, 10 bit"), "HDR: "},
        {"DNxHR 444",                             tr("YUV, 4:4:4, 10 bit"), "HDR: "},
        {"XDCAM HD422",                           tr("YUV, 4:2:2, 8 bit"),  ""},
        {"XAVC 4:2:2",                            tr("YUV, 4:2:2, 8 bit"),  ""},
        {tr("Source"),                            "",                       "HDR: "}
    };

    /*********************************** Intel QSV presets ************************************/
    const QString intelQSV_filter = "hwmap=derive_device=qsv,format=qsv";
    const QString vaapi_filter = "format=nv12|vaapi,hwupload";
#if defined (Q_OS_WIN64)
    const QString intelQSVhwaccel = " -hwaccel dxva2 -hwaccel_output_format dxva2_vld";
    const QString vaapiHwaccel = " -hwaccel dxva2 -hwaccel_output_format dxva2_vld";
#elif defined (Q_OS_UNIX)
    const QString intelQSVhwaccel = " -hwaccel vaapi -hwaccel_output_format vaapi";
    const QString vaapiHwaccel = " -hwaccel vaapi -hwaccel_output_format vaapi";
#endif

    /************************************* XDCAM presets **************************************/
    const QString xdcam_preset = "-pix_fmt yuv422p -c:v mpeg2video -profile:v 0 -flags ilme -top 1 "
                                 "-metadata creation_time=now -vtag xd5c -timecode 01:00:00:00 ";

    /************************************* XAVC presets **************************************/
    const QString xavc_preset = "-pix_fmt yuv422p -c:v libx264 -me_method tesa -subq 9 -partitions all -direct-pred auto "
                                "-psy 0 -g 0 -keyint_min 0 -x264opts filler -x264opts force-cfr -tune fastdecode ";

    const QString arr_params[NUMBER_PRESETS][4] = {
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
        {"-c:v h264_vaapi -profile:v high ",                            vaapiHwaccel,      "0", vaapi_filter}, // Intel VAAPI h264
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

    const QString arr_mode[NUMBER_PRESETS][5] = {
        {"CBR",      "ABR",    "VBR", "CRF", "CQP"}, // H265
        {"CBR",      "ABR",    "VBR", "CRF", "CQP"}, // H265
        {"CBR",      "ABR",    "VBR", "CRF", "CQP"}, // H265
        {"CBR",      "ABR",    "VBR", "CRF", "CQP"}, // H264
        {"ABR",      "CRF",    "",    "",    ""}, // VP9
        {"ABR",      "CRF",    "",    "",    ""}, // VP9
        {"VBR",      "",       "",    "",    ""}, // QSV H265
        {"VBR",      "",       "",    "",    ""}, // QSV H265
        {"VBR",      "CQP_QS", "",    "",    ""}, // QSV H264
        {"ABR",      "CRF",    "",    "",    ""}, // QSV VP9
        {"ABR",      "CRF",    "",    "",    ""}, // QSV VP9
        {"VBR",      "",       "",    "",    ""}, // QSV MPEG2
        {"VBR",      "CQP_VA", "",    "",    ""}, // Intel VAAPI h264
        {"VBR_NV",   "",       "",    "",    ""}, // NVENC H265
        {"VBR_NV",   "",       "",    "",    ""}, // NVENC H265
        {"VBR_NV",   "",       "",    "",    ""}, // NVENC H264
        {tr("Auto"), "",       "",    "",    ""},
        {tr("Auto"), "",       "",    "",    ""},
        {tr("Auto"), "",       "",    "",    ""},
        {tr("Auto"), "",       "",    "",    ""},
        {tr("Auto"), "",       "",    "",    ""},
        {tr("Auto"), "",       "",    "",    ""},
        {tr("Auto"), "",       "",    "",    ""},
        {tr("Auto"), "",       "",    "",    ""},
        {tr("Auto"), "",       "",    "",    ""},
        {tr("Auto"), "",       "",    "",    ""},
        {tr("Auto"), "",       "",    "",    ""},
        {"VBR",      "",       "",    "",    ""}, // XDCAM
        {"CBR",      "",       "",    "",    ""}, // XAVC
        {tr("Auto"), "",       "",    "",    ""}  // Source
    };

    const QString arr_preset[NUMBER_PRESETS][10] = {
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Veryfast"),  tr("Faster"),    tr("Fast"),     tr("Medium"), tr("Slow"), tr("Slower"), tr("Veryslow"), "",           ""},
        {tr("None"), tr("Veryfast"),  tr("Faster"),    tr("Fast"),     tr("Medium"), tr("Slow"), tr("Slower"), tr("Veryslow"), "",           ""},
        {tr("None"), tr("Veryfast"),  tr("Faster"),    tr("Fast"),     tr("Medium"), tr("Slow"), tr("Slower"), tr("Veryslow"), "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Veryfast"),  tr("Faster"),    tr("Fast"),     tr("Medium"), tr("Slow"), tr("Slower"), tr("Veryslow"), "",           ""},
        {tr("None"), tr("Veryfast"),  tr("Faster"),    tr("Fast"),     tr("Medium"), tr("Slow"), tr("Slower"), tr("Veryslow"), "",           ""}, // Intel VAAPI h264
        {tr("None"), tr("Slow"),      "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Slow"),      "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Slow"),      "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""}
    };

    const QString arr_level[NUMBER_PRESETS][21] = {
        {tr("Auto"), "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},    // H265
        {tr("Auto"), "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},    // H265
        {tr("Auto"), "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},    // H265
        {tr("Auto"), "1", "1b", "1.1", "1.2", "1.3", "2", "2.1", "2.2", "3",   "3.1", "3.2", "4",   "4.1", "4.2", "5", "5.1", "5.2", "6", "6.1", "6.2"}, // H264
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},    // VP9
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},    // VP9
        {tr("Auto"), "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},    // QSV H265
        {tr("Auto"), "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},    // QSV H265
        {tr("Auto"), "1", "1b", "1.1", "1.2", "1.3", "2", "2.1", "2.2", "3",   "3.1", "3.2", "4",   "4.1", "4.2", "5", "5.1", "5.2", "6", "6.1", "6.2"}, // QSV H264
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},    // QSV VP9
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},    // QSV VP9
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},    // QSV MPEG2
        {tr("Auto"), "1", "1b", "1.1", "1.2", "1.3", "2", "2.1", "2.2", "3",   "3.1", "3.2", "4",   "4.1", "4.2", "5", "5.1", "5.2", "6", "6.1", "6.2"}, // Intel VAAPI h264
        {tr("Auto"), "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},    // NVENC H265
        {tr("Auto"), "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},    // NVENC H265
        {tr("Auto"), "1", "1b", "1.1", "1.2", "1.3", "2", "2.1", "2.2", "3",   "3.1", "3.2", "4",   "4.1", "4.2", "5", "5.1", "5.2", "6", "6.1", "6.2"}, // NVENC H264
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"2",        "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""}, // XDCAM
        {"5.2",      "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""}, // XAVC
        {tr("Auto"), "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""}  // Source
    };

    const QString arr_pass[NUMBER_PRESETS][2] = {
        {tr("1 Pass"), tr("2 Pass_x265")},
        {tr("1 Pass"), tr("2 Pass_x265")},
        {tr("1 Pass"), tr("2 Pass_x265")},
        {tr("1 Pass"), tr("2 Pass")},
        {tr("1 Pass"), tr("2 Pass")},
        {tr("1 Pass"), tr("2 Pass")},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""}, // Intel VAAPI h264
        {tr("2 Pass Optimisation"), ""},
        {tr("2 Pass Optimisation"), ""},
        {tr("2 Pass Optimisation"), ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""}
    };

    const QString arr_acodec[NUMBER_PRESETS][6] = {
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"Opus",       "Vorbis",     tr("Source"), "",           "",     ""},
        {"Opus",       "Vorbis",     tr("Source"), "",           "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"Opus",       "Vorbis",     tr("Source"), "",           "",     ""},
        {"Opus",       "Vorbis",     tr("Source"), "",           "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""}, // Intel VAAPI h264
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "",           "",           "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"AAC",        "AC3",        "DTS",        "Vorbis",     "Opus", tr("Source")}
    };

    const QString arr_bitrate[5][17] = {
        {"384k",  "320k",  "256k",  "192k",  "128k",  "96k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AAC
        {"640k",  "448k",  "384k",  "256k",  "",      "",      "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AC3
        {"3840k", "3072k", "2048k", "1920k", "1536k", "1472k", "1344k", "1280k", "1152k", "1024k", "960k", "768k", "640k", "576k", "512k", "448k", "384k"}, // DTS
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // Vorbis
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""} // Opus
    };

    const QString arr_sampling[12] = {
        "Source", "8000",  "11025", "16000", "22050",  "32000",
        "44100",  "48000", "88200", "96000", "176400", "192000"
    };

    const QString arr_channels[3] = {
        "Source", "1", "2"
    };

    const QString arr_container[NUMBER_PRESETS][5] = {
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"WebM", "MKV", "",    "",     ""},
        {"WebM", "MKV", "",    "",     ""},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"WebM", "MKV", "",    "",     ""},
        {"WebM", "MKV", "",    "",     ""},
        {"MKV",  "MPG", "AVI", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "",     ""}, // Intel VAAPI h264
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MXF",  "",    "",    "",     ""},
        {"MXF",  "",    "",    "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"}
    };

    /** Get Current **/

    QString getCodecName(int row) const
    {
        if (row >= 0 && row < NUMBER_PRESETS) {
            QString codec = arr_codec[row][0];
            const int pos = codec.indexOf("4:2:0");
            if (pos != -1) {
                codec = codec.left(pos - 1);
            }
            return codec;
        }
        return QString();
    }

    QString getCurrentPreset(int row, int column) const
    {
        if (row >= 0 && row < NUMBER_PRESETS && column >= 0 && column < 10) {
            const QMap<QString, QString> presetImpl = {
                {tr("Ultrafast"), "Ultrafast"},
                {tr("Superfast"), "Superfast"},
                {tr("Veryfast"),  "Veryfast"},
                {tr("Faster"),    "Faster"},
                {tr("Fast"),      "Fast"},
                {tr("Medium"),    "Medium"},
                {tr("Slow"),      "Slow"},
                {tr("Slower"),    "Slower"},
                {tr("Veryslow"),  "Veryslow"}
            };
            QString preset = arr_preset[row][column];
            const int pos = preset.indexOf("_");
            if (pos != -1) {
                preset = preset.left(pos - 1);
            }
            if (presetImpl.contains(preset)) {
                preset = presetImpl[preset];
            }
            return preset;
        }
        return QString();
    }

    QString getCurrentPass(int row, int column) const
    {
        if (row >= 0 && row < NUMBER_PRESETS && column >= 0 && column < 2) {
            QString pass = arr_pass[row][column];
            const int pos = pass.indexOf("_");
            if (pos != -1) {
                pass = pass.left(pos);
            }
            return pass;
        }
        return QString();
    }

    QString getCurrentMode(int row, int column) const
    {
        if (row >= 0 && row < NUMBER_PRESETS && column >= 0 && column < 5) {
            QString mode = arr_mode[row][column];
            const int pos = mode.indexOf("_");
            if (pos != -1) {
                mode = mode.left(pos);
            }
            return mode;
        }
        return QString();
    }  

    /** Get List **/

    QStringList getModesListByRow(int row) const
    {
        return getListByRow<NUMBER_PRESETS, 5>(row, arr_mode);
    }

    QStringList getPresetsListByRow(int row) const
    {
        return getListByRow<NUMBER_PRESETS, 10>(row, arr_preset);
    }

    QStringList getContainersListByRow(int row) const
    {
        return getListByRow<NUMBER_PRESETS, 5>(row, arr_container);
    }

    QStringList getLevelsListByRow(int row) const
    {
        return getListByRow<NUMBER_PRESETS, 21>(row, arr_level);
    }

    QStringList getPassesListByRow(int row) const
    {
        return getListByRow<NUMBER_PRESETS, 2>(row, arr_pass);
    }

    QStringList getAudioCodecsListByRow(int row) const
    {
        return getListByRow<NUMBER_PRESETS, 6>(row, arr_acodec);
    }

private:
    template<const int r, const int c>
    QStringList getListByRow(const int row, const QString (&arr)[r][c]) const
    {
        Q_ASSERT(row < r);
        QStringList list;
        const QMap<QString, QString> modeImpl = {
            {"CBR", tr("Constant Bitrate")},
            {"ABR", tr("Average Bitrate")},
            {"VBR", tr("Variable Bitrate")},
            {"CRF", tr("Constant Rate Factor")},
            {"CQP", tr("Constant QP")}
        };
        for (int i = 0; i < c; i++) {
            QString val = arr[row][i];
            if (val != "") {
                const int pos = val.indexOf("_");
                if (pos != -1) {
                    val = val.left(pos);
                }
                if (modeImpl.contains(val)) {
                    list << modeImpl[val];
                } else list << val;
            }
        }
        return list;
    }

};

#endif // TABLES_H
