#include "about.h"
#include "ui_about.h"



About::About(QWidget *parent): QDialog(parent), ui_about(new Ui::About)
{
    ui_about->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);

    ui_about->frame_top->installEventFilter(this);
}

About::~About()
{
    delete ui_about;
}

void About::setParameters() /*** Set parameters ***/
{
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    QFont font;
    font.setPointSize(10);
    ui_about->label_title->setFont(font);
    QString text("");
    QFile file(":/resources/html/about.html");
    if (file.open(QFile::ReadOnly)) {
        text = file.readAll();
        file.close();
    }
    ui_about->textBrowser->setHtml(text);
    ui_about->textBrowser->setOpenExternalLinks(true);
}

void About::on_buttonCancel_clicked()   /*** Close window ***/
{
    this->close();
}

void About::on_closeWindow_clicked()   /*** Close window ***/
{
    this->close();
}

bool About::eventFilter(QObject *watched, QEvent *event)    /*** Drag window ***/
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->button() == Qt::LeftButton)
        {
            clickPressedFlag = false;
            return true;
        }
        return false;
    }

    if (watched == ui_about->frame_top)
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton)
            {
                mouseClickCoordinate = mouse_event->pos();
                clickPressedFlag = true;
                return true;
            }
            return false;
        }

        if ((event->type() == QEvent::MouseMove) && clickPressedFlag == true)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                this->move(mouse_event->globalPos() - mouseClickCoordinate);
                return true;
            }
            return false;
        }
        return false;
    }
    return QDialog::eventFilter(watched, event);
}
