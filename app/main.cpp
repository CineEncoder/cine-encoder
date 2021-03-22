#include <QApplication>
#include <QSplashScreen>
#include <QElapsedTimer>
#include "mainwindow.h"


QString _cur_param[23];
QVector <QVector <QString> > _preset_table;


int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    QApplication app(argc, argv);
    QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);

    QPixmap pixmap(":/resources/images/splash.png");
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();
    app.processEvents();

    QElapsedTimer time;
    time.start();
    while (time.elapsed() < 1200)
    {
        app.processEvents();
    }

    MainWindow window;
    window.show();
    splash->finish(&window);
    delete splash;
    return app.exec();
}
