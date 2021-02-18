#include "taskcomplete.h"
#include "ui_taskcomplete.h"



Taskcomplete::Taskcomplete(QWidget *parent) :
    QDialog(parent),
    ui_taskcomplete(new Ui::Taskcomplete)
{
    ui_taskcomplete->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);
}

Taskcomplete::~Taskcomplete()
{
    delete ui_taskcomplete;
}

void Taskcomplete::on_pushButton_4_clicked() // Close window
{
    this->close();
}

void Taskcomplete::on_closeWindow_clicked() // Close window
{
    this->close();
}

void Taskcomplete::setMessage(const QString &_message, const bool &_timer_mode)   // Set parameters
{
    ui_taskcomplete->frame_hint->installEventFilter(this);
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    if (_timer_mode == true)
    {
        show_message(_message);
        elps_t = 25;
        timer->setInterval(1000);
        connect(timer, SIGNAL(timeout()), this, SLOT(repeat_handler()));
        timer->start();
    }
    else
    {
#ifdef Q_OS_WIN
        QSound::play("./cine-encoder.wav");
#else
        QSound::play("/usr/share/sounds/cine-encoder.wav");
#endif
        show_message(_message);
    }
}

void Taskcomplete::repeat_handler() // Repeat handler
{
    if (elps_t == 0)
    {
        this->close();
    }
    int h = static_cast<int>(trunc(elps_t / 3600));
    int m = static_cast<int>(trunc((elps_t - (h * 3600)) / 60));
    int s = static_cast<int>(trunc(elps_t - (h * 3600) - (m * 60)));
    QString hrs = QString::number(h);
    QString min = QString::number(m);
    QString sec = QString::number(s);
    std::ostringstream sstr;
    sstr << std::setw(2) << std::setfill('0') << hrs.toStdString() << ":"
         << std::setw(2) << std::setfill('0') << min.toStdString() << ":"
         << std::setw(2) << std::setfill('0') << sec.toStdString();
    std::string tm = sstr.str();
    _message = "Pause\n\n Resume after: " + QString::fromStdString(tm);
    show_message(_message);
    elps_t--;
}

void Taskcomplete::show_message(QString _message)   // Show message
{
    ui_taskcomplete->textBrowser->clear();
    ui_taskcomplete->textBrowser->setAlignment(Qt::AlignCenter);
    ui_taskcomplete->textBrowser->append(_message);
}

bool Taskcomplete::eventFilter(QObject *watched, QEvent *event)
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

    if (watched == ui_taskcomplete->frame_hint)
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

