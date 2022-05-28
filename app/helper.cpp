#include "helper.h"
#include <QLocale>
#include <QProcess>
#include <QFileDialog>
#include <iostream>
#include <math.h>

#define trc(num) static_cast<int>(trunc(num))
#define rnd(num) static_cast<int>(round(num))
#define intToStr(num, offset) QString::number(num).rightJustified(offset, '0')


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
        std::cout << "printenv not found" << std::endl;
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
        if (first_symbol != -1 && row.indexOf('=') != -1)
            varList.append(row.mid(first_symbol));
    }
    foreach (const QString &var, varList) {
        varNames.append(var.split('=')[0].remove(' ').remove('\n'));
        varValues.append(var.split('=')[1].remove(' ').remove('\n'));
        style = style.remove(var + QString(";"));
    }
    for (int i = 0; i < varNames.size() && i < varValues.size(); i++)
        style = style.replace(varNames[i], varValues[i]);
    //std::cout << style.toStdString() << std::endl;
    return style;
}

void Helper::openFileDialog(FileDialogType dialogType,
                            const QString  &title,
                            const QString  &path,
                            QStringList    &result)
{
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
    const QString dir = (path != QString("")) ? path : QDir::homePath();
    dlg.setDirectory(dir);

    switch(dialogType) {
    case FileDialogType::OPENVFILES:
        dlg.setFileMode(QFileDialog::ExistingFiles);
        dlg.setNameFilter(tr("Video Files:") + " *.avi, *.m2ts, *.m4v, *.mkv, *.mov, *.mp4, \
                             *.mpeg, *.mpg, *.mxf, *.ts, *.webm (*.avi *.m2ts *.m4v \
                             *.mkv *.mov *.mp4 *.mpeg *.mpg *.mxf *.ts *.webm);;" +
                             tr("All files") + " (*.*)");
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
