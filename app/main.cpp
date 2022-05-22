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
#include "mainwindow.h"
#include "constants.h"
#include "helper.h"


int checkForDuplicates();

int main(int argc, char *argv[])
{
    std::setlocale(LC_ALL, "");
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("CineEncoder");
    QCoreApplication::setApplicationName("CineEncoder");
    QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);
    app.setStyle(QStyleFactory::create("Fusion"));
    if (checkForDuplicates() == 1)
        return 1;

    /******************* Read Settings ****************************/
    SETTINGS(stn);
    stn.beginGroup("Settings");
    const int fntSize = stn.value("Settings/font_size", FONTSIZE).toInt();
    const QString fntFamily = stn.value("Settings/font").toString();
    const QString savedLang = stn.value("Settings/language").toString();
    stn.endGroup();

    /********************* Set Font ******************************/
    QFont fnt = app.font();
    if (fntFamily != "")
        fnt.setFamily(fntFamily);
    fnt.setPointSize(fntSize);
    app.setFont(fnt);

    /******************* Set Translate ****************************/

    auto sysLang = QLocale::system().language();
    QMap<int, QString> langMap = {
        {QLocale::Chinese, "zh"},
        {QLocale::German,  "de"},
        {QLocale::Russian, "ru"}
    };
    const QString currLang = (langMap.contains(sysLang)) ?
                langMap.value(sysLang) : "";

    QTranslator tran;
    if (savedLang == "" && currLang != "") {
        if (tran.load(QString(":/resources/translation/translation_%1.qm").arg(currLang))) {
            app.installTranslator(&tran);
        }
    }
    else if (savedLang != "" && savedLang != "en") {
        if (tran.load(QString(":/resources/translation/translation_%1.qm").arg(savedLang))) {
            app.installTranslator(&tran);
        }
    }

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
        //qDebug() << list << "\n" << lindex << rindex;
        if (lindex != rindex) {
            QMessageBox msgBox(nullptr);
            msgBox.setWindowTitle("Cine Encoder");
            msgBox.setWindowIcon(QIcon(":/resources/icons/64x64/cine-encoder.png"));
            msgBox.setIcon(QMessageBox::Critical);
            msgBox.setText("The program is already running!");
            msgBox.exec();
            return 1;
        }
    } else {
        std::cout << "Command \""<< cmd.toStdString() << "\" not found." << std::endl;
    }
    return 0;
}
