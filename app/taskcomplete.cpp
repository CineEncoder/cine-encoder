/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: taskcomplete.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "taskcomplete.h"
#include "ui_taskcomplete.h"


Taskcomplete::Taskcomplete(QWidget *parent):
    FramelessWindow(parent),
    ui(new Ui::Taskcomplete),
    _clickPressedFlag(false)
{
    ui->setupUi(this);
    ui->frame_top->installEventFilter(this);
}

Taskcomplete::~Taskcomplete()
{
    delete ui;
}

void Taskcomplete::setMessage(const QString &_message, const bool &_timer_mode)   /*** Set parameters ***/
{
    QFont font;
    font.setPointSize(10);
    ui->label_title->setFont(font);
    if (_timer_mode) {
        show_message(_message);
        _elps_t = 25;
        timer = new QTimer(this);
        timer->setInterval(1000);
        connect(timer, SIGNAL(timeout()), this, SLOT(repeatHandler()));
        timer->start();
    }
    else {
#ifdef Q_OS_WIN
        QSound::play("./cine-encoder.wav");
#else
        QSound::play("/usr/share/sounds/cine-encoder.wav");
#endif
        show_message(_message);
    }
}

void Taskcomplete::on_closeWindow_clicked() /*** Close window ***/
{
    this->close();
}

void Taskcomplete::on_buttonCancel_clicked() /*** Close window ***/
{
    this->close();
}

void Taskcomplete::show_message(QString _message)   /*** Show message ***/
{
    ui->textBrowser_task->clear();
    ui->textBrowser_task->setAlignment(Qt::AlignCenter);
    ui->textBrowser_task->append(_message);
    QTextCursor textCursor = ui->textBrowser_task->textCursor();
    textCursor.movePosition(QTextCursor::Start);
    ui->textBrowser_task->setTextCursor(textCursor);
}

void Taskcomplete::repeatHandler() /*** Repeat handler ***/
{
    if (_elps_t == 0) {
        this->close();
    }
    int h = static_cast<int>(trunc(float(_elps_t) / 3600));
    int m = static_cast<int>(trunc((float(_elps_t) - float(h * 3600)) / 60));
    int s = _elps_t - (h * 3600) - (m * 60);
    QString hrs = QString::number(h);
    QString min = QString::number(m);
    QString sec = QString::number(s);
    std::ostringstream sstr;
    sstr << std::setw(2) << std::setfill('0') << hrs.toStdString() << ":"
         << std::setw(2) << std::setfill('0') << min.toStdString() << ":"
         << std::setw(2) << std::setfill('0') << sec.toStdString();
    std::string tm = sstr.str();
    _message = tr("Pause\n\n Resume after: ") + QString::fromStdString(tm);
    show_message(_message);
    _elps_t--;
}

bool Taskcomplete::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->button() == Qt::LeftButton) {
            _clickPressedFlag = false;
            return true;
        }
    } else

    if (watched == ui->frame_top) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton) {
                _mouseClickCoordinate = mouse_event->pos() + QPoint(7,7);
                _clickPressedFlag = true;
                return true;
            }
        } else
        if ((event->type() == QEvent::MouseMove) && _clickPressedFlag) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton) {
                this->move(mouse_event->globalPos() - _mouseClickCoordinate);
                return true;
            }
        }
    }
    return QDialog::eventFilter(watched, event);
}

