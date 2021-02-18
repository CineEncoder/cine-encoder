#include "about.h"
#include "ui_about.h"



About::About(QWidget *parent) :
    QDialog(parent),
    ui_about(new Ui::About)
{
    ui_about->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);
}

About::~About()
{
    delete ui_about;
}

void About::on_toolButton_6_clicked()   // Close window
{
    this->close();
}

void About::on_closeWindow_clicked()   // Close window
{
    this->close();
}

void About::setParameters() // Set parameters
{
    ui_about->frame_hint->installEventFilter(this);
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    QFile file;
    file.setFileName(":/resources/html/about.html");
    file.open(QFile::ReadOnly);
    QString text = file.readAll();
    ui_about->textBrowser->setHtml(text);
    ui_about->textBrowser->setOpenExternalLinks(true);
}

bool About::eventFilter(QObject *watched, QEvent *event)    // Drag window
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->button() == Qt::LeftButton)
        {
            clickPressedFlag = false;
            return QDialog::eventFilter(watched, event);
        }
    }

    if (watched == ui_about->frame_hint)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton)
            {
                mouseClickCoordinate = mouse_event->pos();
                clickPressedFlag = true;
                return QDialog::eventFilter(watched, event);
            }
        }
        else if ((event->type() == QEvent::MouseMove) && clickPressedFlag == true)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                this->move(mouse_event->globalPos() - mouseClickCoordinate);
                return QDialog::eventFilter(watched, event);
            }
        }
    }
    return QDialog::eventFilter(watched, event);
}
