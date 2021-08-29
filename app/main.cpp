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

    QPixmap pixmap(":/resources/images/splash.png");
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();
    app.processEvents();

    QElapsedTimer time;
    time.start();
    while (time.elapsed() < 1000) {
        app.processEvents();
    }

    Widget window;
    window.show();
    splash->finish(&window);
    delete splash;
    return app.exec();
}
