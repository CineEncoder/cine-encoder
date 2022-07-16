#include "helper.h"
#include <QLocale>
#include <QProcess>
#include <QFileDialog>
#include <QFontMetrics>
#include <iostream>
#include <math.h>

#define trc(num) static_cast<int>(trunc(num))
#define rnd(num) static_cast<int>(round(num))
#define intToStr(num, offset) QString::number(num).rightJustified(offset, '0')
#define Print(a) std::cout << a << std::endl


Helper::Helper()
{

}

Helper::~Helper()
{

}

Helper::DesktopEnv Helper::m_desktopEnv = Helper::DesktopEnv::UNDEF;

void Helper::detectEnv()
{
#if defined (Q_OS_UNIX)
    QProcess prc;
    prc.setProcessChannelMode(QProcess::MergedChannels);
    QStringList arg;
    arg << "XDG_CURRENT_DESKTOP";
    prc.start("printenv", arg);
    if (prc.waitForFinished(1500)) {
        const QString line = QString(prc.readAllStandardOutput());
        m_desktopEnv = (line.indexOf("GNOME") != -1) ?
                    DesktopEnv::GNOME : DesktopEnv::OTHER;
    } else {
        Print("printenv not found");
    }
#endif
}

Helper::DesktopEnv Helper::getEnv()
{
    return m_desktopEnv;
}

QString Helper::getParsedCss(const QString &list)    // Parsing CSS
{
    QString style = list;
    QStringList varList;
    QStringList varNames;
    QStringList varValues;
    QStringList splitList;
    splitList << list.split(';');

    foreach (const QString &row, splitList) {
        const int first_symbol = row.indexOf('@');
        if (first_symbol != -1 && row.indexOf('=') != -1 && row.indexOf('[') == -1)
            varList.append(row.mid(first_symbol));
    }
    foreach (const QString &var, varList) {
        varNames.append(var.split('=')[0].remove(' ').remove('\n'));
        varValues.append(var.split('=')[1].remove(' ').remove('\n'));
        style = style.remove(var + QString(";"));
    }
    for (int i = 0; i < varNames.size() && i < varValues.size(); i++)
        style = style.replace(varNames[i], varValues[i]);
    //Print(style.toStdString());
    return style;
}

void Helper::openFileDialog(FileDialogType dialogType,
                            const QString  &title,
                            const QString  &path,
                            QStringList    &result)
{
    const QVector<QString> videoFilters = {
        QString("%1: *.avi, *.m2ts, *.m4v, *.mkv, *.mov, *.mp4, *.mpeg, *.mpg, *.mxf, *.wmv, *.ts, *.webm \
               (*.avi *.m2ts *.m4v *.mkv *.mov *.mp4 *.mpeg *.mpg *.mxf *.wmv *.ts *.webm)").arg(tr("Video Files")),
        "Audio Video Interleave *.avi (*.avi)",
        "Blu-ray BDAV Video *.m2ts (*.m2ts)",
        "iTunes Video *.m4v (*.m4v)",
        "Matroska *.mkv (*.mkv)",
        "QuickTime *.mov (*.mov)",
        "MPEG-4 *.mp4 (*.mp4)",
        "MPEG *.mpeg, *.mpg (*.mpeg *.mpg)",
        "Material Exchange Format *.mxf (*.mxf)",
        "Windows Media Video *.wmv (*.wmv)",
        "Video Transport Stream *.ts (*.ts)",
        "WebM *.webm (*.webm)",
        QString("%1 (*.*)").arg(tr("All files"))
    };

    const QVector<QString> audioFilters = {
        QString("%1: *.wma, *.ac3, *.aac, *.alac, *.mka, *.dts, *.thd, *.eac3, *.mp3, *.wav, *.vorbis, *.ogg, *.flac, *.opus \
               (*.wma *.ac3 *.aac *.alac *.mka *.dts *.thd *.eac3 *.mp3 *.wav *.vorbis *.ogg *.flac *.opus)").arg(tr("Audio Files")),
        "Windows Media Audio *.wma (*.wma)",
        "Dolby Digital *.ac3 (*.ac3)",
        "Advanced Audio Coding *.aac (*.aac)",
        "Apple Lossless Audio Codec *.alac (*.alac)",
        "Matroska Audio *.mka (*.mka)",
        "Dolby TrueHD *.dts, *.thd (*.dts *.thd)",
        "Dolby Digital Plus *.eac3 (*.eac3)",
        "MPEG-1 Audio Layer III, MPEG-2 Audio Layer III *.mp3 (*.mp3)",
        "Waveform Audio File Format *.wav (*.wav)",
        "Ogg Vorbis *.vorbis, *.ogg (*.vorbis *.ogg)",
        "Free Lossless Audio Codec *.flac (*.flac)",
        "Opus Audio Format *.opus (*.opus)",
        QString("%1 (*.*)").arg(tr("All files"))
    };

    const QVector<QString> subtitleFilters = {
        QString("%1: *.srt, *.vtt, *.sbv, *.sub, *.ttml, *.rt, *.scc, *.ssa, *.txt, *.mks, *.ass, *.mpl, *.xml, *.stl, *.mmc \
               (*.srt *.vtt *.sbv *.sub *.ttml *.rt *.scc *.ssa *.txt *.mks *.ass *.mpl *.xml *.stl *.mmc)").arg(tr("Subtitle Files")),
        "SubRip Subtitle *.srt (*.srt)",
        "Web Video Text Tracks *.vtt (*.vtt)",
        "YouTube Captions *.sbv (*.sbv)",
        "MicroDVD Subtitle *.sub (*.sub)",
        "Timed Text Markup Language *.ttml (*.ttml)",
        "RealText Streaming *.rt (*.rt)",
        "Scenarist Closed Caption *.scc (*.scc)",
        "Sub Station Alpha *.ssa (*.ssa)",
        "Plain Text *.txt (*.txt)",
        "Matroska Subtitle *.mks (*.mks)",
        "Aegisub Advanced SubStation Alpha *.ass (*.ass)",
        "AVCHD Playlist *.mpl (*.mpl)",
        "XML File *.xml (*.xml)",
        "Spruce Technologies Subtitle *.stl (*.stl)",
        "Microsoft Media Catalog *.mmc (*.mmc)",
        QString("%1 (*.*)").arg(tr("All files"))
    };

    auto getFilter = [](QVector<QString> filters)->QString {
        QString filter;
        foreach (auto flt, filters) {
            filter += flt;
            if (flt != filters.back())
                filter += ";;";
        }
        return filter;
    };
    result.clear();
    QFileDialog dlg(nullptr);
    dlg.setWindowTitle(title);
    dlg.setMinimumWidth(600);
    dlg.setWindowFlags(Qt::Dialog | Qt::SubWindow);
    QFileDialog::Options opt = QFileDialog::DontResolveSymlinks;
    if (Helper::getEnv() == DesktopEnv::GNOME)
        opt |= QFileDialog::DontUseNativeDialog;
    dlg.setOptions(opt);
    dlg.setAcceptMode(QFileDialog::AcceptOpen);
    const QString dir = (!path.isEmpty()) ? path : QDir::homePath();
    dlg.setDirectory(dir);

    switch(dialogType) {
    case FileDialogType::OPENVFILES:
        dlg.setFileMode(QFileDialog::ExistingFiles);
        dlg.setNameFilter(getFilter(videoFilters));
        break;
    case FileDialogType::OPENAFILES:
        dlg.setFileMode(QFileDialog::ExistingFiles);
        dlg.setNameFilter(getFilter(audioFilters));
        break;
    case FileDialogType::OPENSFILES:
        dlg.setFileMode(QFileDialog::ExistingFiles);
        dlg.setNameFilter(getFilter(subtitleFilters));
        break;
    case FileDialogType::SELECTFOLDER:
        dlg.setFileMode(QFileDialog::DirectoryOnly);
        break;
    }
    if (dlg.exec() == QFileDialog::Accepted) {
        result = dlg.selectedFiles();
    }
}

QString Helper::timeConverter(const float time)     // Time converter to hh:mm:ss
{
    const int h = trc(time / 3600);
    const int m = trc((time - float(h * 3600)) / 60);
    const int s = rnd(time - float(h * 3600) - float(m * 60));
    return QString("%1:%2:%3").arg(intToStr(h,2), intToStr(m,2), intToStr(s,2));
}

QString Helper::timeConverter(const double time)     // Time converter to hh:mm:ss.msc
{
    const int h = trc(time / 3600);
    const int m = trc((time - double(h * 3600)) / 60);
    const int s = trc(time - double(h * 3600) - double(m * 60));
    const int ms = rnd(1000 * (time - double(h * 3600) - double(m * 60) - double(s)));
    return QString("%1:%2:%3.%4").arg(intToStr(h,2), intToStr(m,2), intToStr(s,2), intToStr(ms,3));
}

QString Helper::getSysLanguage()
{
    auto _sysLang = QLocale::system().language();
    QMap<int, QString> langMap = {
        {QLocale::English, "en"},
        {QLocale::Chinese, "zh"},
        {QLocale::German,  "de"},
        {QLocale::Russian, "ru"}
    };
    return (langMap.contains(_sysLang)) ? langMap.value(_sysLang) : "en";
}

QString Helper::elideText(QWidget *w,
                          const QString &text,
                          const Qt::TextElideMode elide)
{
    QFontMetrics fm = w->fontMetrics();
#if (QT_VERSION < QT_VERSION_CHECK(5,11,0))
    const int fwidth = fm.width(text);
#else
    const int fwidth = fm.horizontalAdvance(text);
#endif
    const int width = w->width();
    return (fwidth > width) ? fm.elidedText(text, elide, width, 0) : text;
}

QString Helper::recalcChannels(const QString &ch)
{
    return (ch == "6") ? "5.1" :
           (ch == "8") ? "7.1" : ch;
}

bool Helper::isSupported(const QString &format)
{
    const QVector<QString> unspFormats = {
        "PGS"
    };
    return unspFormats.indexOf(format) == -1 ? true: false;
}
