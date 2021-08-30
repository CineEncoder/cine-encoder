#include <QApplication>
#include <QCoreApplication>
#include <QSplashScreen>
#include <QElapsedTimer>
#include "mainwindow.h"


int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("CineEncoder");
    QCoreApplication::setApplicationName("CineEncoder");
    QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);

    QFont font = app.font();
    font.setPointSize(8);
    app.setFont(font);

    /******************* Set Translate ****************************/
    QString _settings_path = QDir::homePath() + QString("/CineEncoder");
    QSettings *_settings = new QSettings(_settings_path + QString("/ce_window.ini"), QSettings::IniFormat, nullptr);
    _settings->beginGroup("Settings");
    QString _language = _settings->value("Settings/language").toString();
    _settings->endGroup();
    QTranslator qtTranslator;
    if (_language != "en") {
        if (qtTranslator.load(":/resources/translation/translation_" + _language + ".qm")) {
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
