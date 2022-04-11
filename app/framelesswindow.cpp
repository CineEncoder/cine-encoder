#include "framelesswindow.h"
#include <QGraphicsDropShadowEffect>


FramelessWindow::FramelessWindow(QWidget *parent) :
    QDialog(parent),
    window_activated(false)
{
    this->setAttribute(Qt::WA_Hover, true);
    this->installEventFilter(this);
#ifdef Q_OS_WIN64
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
                         Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
#else
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#endif
    this->setMouseTracking(true);
    this->setAcceptDrops(true);
    this->setAttribute(Qt::WA_TranslucentBackground);
}

FramelessWindow::~FramelessWindow()
{

}

bool FramelessWindow::event(QEvent *event)
{
    if (event->type() == QEvent::Show) {
        if (!window_activated) {
            window_activated = true;
            QList<QString> whiteList = {"centralwidget", "widget_main"};
            QList<QWidget*> children = findChildren<QWidget*>();
            foreach (QWidget *child, children) {
                if (whiteList.contains(child->objectName())) {
                    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(child);
                    shadow->setBlurRadius(11.0);
                    shadow->setColor(QColor(0, 0, 0, 80));
                    shadow->setOffset(0.0);
                    child->setGraphicsEffect(shadow);
                    break;
                }
            }
        }
    }
    return QDialog::event(event);
}

/*bool FramelessWindow::eventFilter(QObject *watched, QEvent *event)
{

    return QDialog::eventFilter(watched, event);
}*/
