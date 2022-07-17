/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: main.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include <QApplication>
#include <QCoreApplication>
#include <QSplashScreen>
#include <QElapsedTimer>
#include <QFontDatabase>
#include <QStyleFactory>
#include <QMessageBox>
#include <QTranslator>
#include <QMap>
#include <iostream>
#include "mainwindow.h"
#include "helper.h"


int checkForDuplicates();

int main(int argc, char *argv[])
{
#ifdef Q_OS_UNIX
    qputenv("QT_QPA_PLATFORM", "xcb");
    qputenv("QT_LOGGING_RULES", "*.debug=false;qt.qpa.*=false");
#endif
    setlocale(LC_ALL, "");
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("CineEncoder");
    QCoreApplication::setApplicationName("CineEncoder");
    QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    //QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    app.setStyle(QStyleFactory::create("Fusion"));
    const QString sysLang = Helper::getSysLanguage();
    /*const int id = QFontDatabase::addApplicationFont(":/resources/fonts/interregular.otf");
    QString sysFamily = app.font().family();
    if (!QFontDatabase::applicationFontFamilies(id).isEmpty())
        sysFamily = QFontDatabase::applicationFontFamilies(id).at(0);*/

    /******************* Read Settings ****************************/
    SETTINGS(stn);
    stn.beginGroup("Settings");
    const bool allowDuplicates = stn.value("Settings/allow_duplicates", false).toBool();
    const int fntSize = stn.value("Settings/font_size", FONTSIZE).toInt();
    const QString fntFamily = stn.value("Settings/font").toString();
    const QString currLang = stn.value("Settings/language", sysLang).toString();
    stn.endGroup();

    /**************** Check for duplicates ************************/
    if (!allowDuplicates)
        if (checkForDuplicates() == 1)
            return 1;

    /******************* Set Translate ****************************/
    QTranslator trns;
    if (currLang != "en" && trns.load(QString(":/resources/translation/translation_%1.qm").arg(currLang)))
        app.installTranslator(&trns);

    /********************* Set Font ******************************/
    QFont fnt = app.font();
    if (fntFamily != "")
        fnt.setFamily(fntFamily);
    fnt.setPointSize(fntSize);
    fnt.setWeight(QFont::Medium);
    app.setFont(fnt);

    /******************* Set Splash *******************************/
    const QPixmap pixmap(":/resources/images/splash.png");
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();
    app.processEvents();

    QElapsedTimer time;
    time.start();
    while (time.elapsed() < 1000) {
        app.processEvents();
    }

    /******************* Set Window *******************************/
    Helper::detectEnv();
    MainWindow window;
    window.show();
    splash->finish(&window);
    delete splash;
    return app.exec();
}

int checkForDuplicates()
{
    QProcess process;
    process.setProcessChannelMode(QProcess::MergedChannels);
    QString cmd;
    QStringList arguments;
#if defined (Q_OS_WIN64)
    cmd = "cmd";
    arguments << "/C" << "echo" << "process" << "get" << "caption" << "|" << "wmic";
#elif defined (Q_OS_UNIX)
    cmd = "ps";
    arguments << "-A";
#endif
    process.start(cmd,  arguments);
    if (process.waitForFinished(1000)) {
        const QString list = QString(process.readAllStandardOutput());
        const int lindex = list.indexOf("cine_encoder");
        const int rindex = list.lastIndexOf("cine_encoder");
        if (lindex != rindex) {
            QMessageBox msgBox(nullptr);
            msgBox.setWindowTitle("Cine Encoder");
            msgBox.setWindowIcon(QIcon(":/resources/icons/64x64/cine-encoder.png"));
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText(QObject::tr("The program is already running!"));
            msgBox.exec();
            return 1;
        }
    } else {
        Print("Command \""<< cmd.toStdString() << "\" not found.");
    }
    return 0;
}
