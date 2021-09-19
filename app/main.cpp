/****************************************************************
MAIN.CPP
****************************************************************/

#include <QApplication>
#include <QCoreApplication>
#include <QSplashScreen>
#include <QElapsedTimer>
#include <QFontDatabase>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("CineEncoder");
    QCoreApplication::setApplicationName("CineEncoder");
    //QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);

    /********************* Set Fonts ******************************/
//    QFontDatabase database;
//    QFontDatabase::WritingSystem values = QFontDatabase::WritingSystem::Latin;
//    const QStringList fontFamilies = database.families(values);
//    for (const QString &family : fontFamilies) {
//        std::cout << "Style: " << family.toStdString() << std::endl;
//    }

    QFont font = app.font();
    //std::cout << "Style: " << font.family().toStdString() << " Size: " << font.pointSize() << std::endl;
    //font.setFamily("Microsoft Yahei");
    //font.setFamily("Microsoft Yahei");
    font.setPointSize(8);
    app.setFont(font);

    /******************* Set Translate ****************************/
    QString _language = "";
    QLocale locale = QLocale::system();
    if (locale.language() == QLocale::Chinese) {
        _language = "zh";
    }
    else if (locale.language() == QLocale::German) {
        _language = "de";
    }
    else if (locale.language() == QLocale::Russian) {
        _language = "ru";
    }

    QString _settings_path = QDir::homePath() + QString("/CineEncoder");
    QSettings *_settings = new QSettings(_settings_path + QString("/ce_window.ini"), QSettings::IniFormat, nullptr);
    _settings->beginGroup("Settings");
    QString language = _settings->value("Settings/language").toString();
    _settings->endGroup();
    QTranslator qtTranslator;
    if (language == "" && _language != "") {
        if (qtTranslator.load(":/resources/translation/translation_" + _language + ".qm")) {
            app.installTranslator(&qtTranslator);
        }
    }
    else if (language != "" && language != "en") {
        if (qtTranslator.load(":/resources/translation/translation_" + language + ".qm")) {
            app.installTranslator(&qtTranslator);
        }
    }
    delete _settings;

    /******************* Set Splash *******************************/
    QPixmap pixmap(":/resources/images/splash.png");
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();
    app.processEvents();

    QElapsedTimer time;
    time.start();
    while (time.elapsed() < 1000) {
        app.processEvents();
    }

    /******************* Set Window *******************************/
    Widget window;
    window.show();
    splash->finish(&window);
    delete splash;
    return app.exec();
}
