/****************************************************************
MAIN.CPP
****************************************************************/

#include <QApplication>
#include <QCoreApplication>
#include <QSplashScreen>
#include <QElapsedTimer>
#include <QFontDatabase>
#include <QMap>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("CineEncoder");
    QCoreApplication::setApplicationName("CineEncoder");
    QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);

    /******************* Read Settings ****************************/
    QString _settings_path = QDir::homePath() + QString("/CineEncoder");
    QSettings *_settings = new QSettings(_settings_path + QString("/ce_window.ini"), QSettings::IniFormat, nullptr);
    _settings->beginGroup("Settings");
    int _fontSize = _settings->value("Settings/font_size").toInt();
    QString _fontFamily = _settings->value("Settings/font").toString();
    QString _language = _settings->value("Settings/language").toString();
    _settings->endGroup();
    delete _settings;

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
