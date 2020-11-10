#include "taskcomplete.h"
#include "ui_taskcomplete.h"
#include <math.h>
#include <iomanip>
#include <sstream>
//#include <iostream>

//extern bool _protection;

Taskcomplete::Taskcomplete(QWidget *parent) :
    QDialog(parent),
    ui_taskcomplete(new Ui::Taskcomplete)
{
    ui_taskcomplete->setupUi(this);

}

Taskcomplete::~Taskcomplete()
{
    delete ui_taskcomplete;
}


void Taskcomplete::on_pushButton_4_clicked()
{
    this->close();
}

void Taskcomplete::set_message(QString _message, bool timer_mode)
{
    if (timer_mode == true) {
        show_message(_message);
        elps_t = 25;
        timer->setInterval(1000);
        connect(timer, SIGNAL(timeout()), this, SLOT(repeat_handler()));
        timer->start();
    } else {
        show_message(_message);
    }
}

void Taskcomplete::show_message(QString _message)
{
    ui_taskcomplete->textBrowser->clear();
    ui_taskcomplete->textBrowser->setAlignment(Qt::AlignCenter);
    ui_taskcomplete->textBrowser->append(_message);
}

void Taskcomplete::repeat_handler()
{
    if (elps_t == 0) {
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
