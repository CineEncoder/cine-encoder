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
    SETTINGS(_settings);
    _settings.beginGroup("Settings");
    int _fontSize = _settings.value("Settings/font_size").toInt();
    QString _fontFamily = _settings.value("Settings/font").toString();
    QString _language = _settings.value("Settings/language").toString();
    _settings.endGroup();

    /********************* Set Font ******************************/
    QFont font = app.font();
    if (_fontSize == 0) {
        _fontSize = 8;
    }
    if (_fontFamily != "") {
        font.setFamily(_fontFamily);
    }
    font.setPointSize(_fontSize);
    app.setFont(font);

    /******************* Set Translate ****************************/
    QString language = "";
    QLocale locale = QLocale::system();
    QMap<int, QString> langIndex;
    langIndex[QLocale::Chinese] = "zh";
    langIndex[QLocale::German] = "de";
    langIndex[QLocale::Russian] = "ru";
    if (langIndex.contains(locale.language())) {
        language = langIndex.value(locale.language());
    }

    QTranslator qtTranslator;
    if (_language == "" && language != "") {
        if (qtTranslator.load(":/resources/translation/translation_" + language + ".qm")) {
            app.installTranslator(&qtTranslator);
        }
    }
    else if (_language != "" && _language != "en") {
        if (qtTranslator.load(":/resources/translation/translation_" + _language + ".qm")) {
            app.installTranslator(&qtTranslator);
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
        QString list = QString(process.readAllStandardOutput());
        int lindex = list.indexOf("cine_encoder");
        int rindex = list.lastIndexOf("cine_encoder");
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
