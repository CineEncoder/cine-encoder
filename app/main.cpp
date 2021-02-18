#include <QApplication>
#include <QSplashScreen>
#include <QElapsedTimer>
#include "mainwindow.h"



QString _cur_param[23];
QVector <QVector <QString> > _preset_table;


/*void message(const QString &path, QSplashScreen *&splash)
{
    QByteArray path_ba = path.toLocal8Bit();
    const char *path_ch = path_ba.data();

    splash->showMessage (
        QObject::tr(path_ch),
        Qt::AlignBottom | Qt::AlignLeft
    );
}*/

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "");
    QApplication app(argc, argv);
    QCoreApplication::setAttribute(Qt::AA_UseStyleSheetPropagationInWidgetStyles, true);

    QPixmap pixmap(":/resources/images/splash.png");
    QSplashScreen *splash = new QSplashScreen(pixmap);
    splash->show();
    app.processEvents();

    /*foreach (const QString &path, app.libraryPaths()) {
        message(path, splash);
        QCoreApplication::processEvents();
    }*/

    QElapsedTimer time;
    time.start();
    while (time.elapsed() < 1200)
    {
        /*const int progress = 1 + static_cast< double >(time.elapsed()) / LOAD_TIME_MSEC * 100.0;
        splash->showMessage(
            QObject::tr("Loading... %1%").arg(progress),
            Qt::AlignBottom | Qt::AlignRight
        );
        splash->setPixmap(pixmap);*/
        app.processEvents();
    }

    MainWindow window;
    window.show();
    splash->finish(&window);
    delete splash;
    return app.exec();
}
