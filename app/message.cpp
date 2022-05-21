/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: message.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "message.h"
#include "ui_message.h"
#include <math.h>

#define ELAPSED_TIME 25

Message::Message(QWidget *parent, MessType mess_type,
                 const QString &message, const bool timer_flag) :
    BaseWindow(parent, false),
    ui(new Ui::Message),
    m_message(message),
    m_mess_type(mess_type),
    m_elps_t(0),
    m_windowActivated(false),
    m_timer_flag(timer_flag)
{
    QWidget *ui_widget = new QWidget(this);
    setCentralWidget(ui_widget);
    ui->setupUi(ui_widget);
    setTitleBar(ui->frame_top);
#ifdef Q_OS_UNIX
    setMaskWidget(ui_widget);
#endif
    connect(ui->closeWindow, &QPushButton::clicked, this, &Message::onCloseWindow);
    connect(ui->buttonApply, &QPushButton::clicked, this, &Message::onButtonApply);
    if (m_mess_type == MessType::INFO) {
        ui->spacerCancel->changeSize(0,0);
        ui->buttonCancel->hide();
        ui->buttonCancel->setFixedWidth(0);
    } else {
        connect(ui->buttonCancel, &QPushButton::clicked, this, &Message::onCloseWindow);
    }
}

Message::~Message()
{
    delete ui;
}

void Message::onButtonApply()
{
    acceptDialog();
}

void Message::onCloseWindow()
{
    closeDialog();
}

void Message::showEvent(QShowEvent *event)
{
    BaseWindow::showEvent(event);
    if (!m_windowActivated) {
        m_windowActivated = true;
        setMessage();
    }
}

void Message::setMessage()
{
    QFont font;
    font.setPointSize(10);
    ui->label_title->setFont(font);
    if (m_mess_type == MessType::INFO && m_timer_flag) {
        show_message();
        m_elps_t = ELAPSED_TIME;
        m_timer = new QTimer(this);
        m_timer->setInterval(1000);
        connect(m_timer, &QTimer::timeout, this, &Message::repeatHandler);
        m_timer->start();
    }
    else {
#if defined (Q_OS_WIN64)
        QSound::play("./cine-encoder.wav");
#elif defined (Q_OS_UNIX)
        QSound::play("/usr/share/sounds/cine-encoder.wav");
#endif
        show_message();
    }
}

void Message::show_message()
{
    ui->textBrowser->clear();
    ui->textBrowser->setAlignment(Qt::AlignCenter);
    ui->textBrowser->append(m_message);
    QTextCursor textCursor = ui->textBrowser->textCursor();
    textCursor.movePosition(QTextCursor::Start);
    ui->textBrowser->setTextCursor(textCursor);
}

void Message::repeatHandler()
{
    if (m_elps_t == 0) {
        this->close();
    }
    const int h = static_cast<int>(trunc(float(m_elps_t) / 3600));
    const int m = static_cast<int>(trunc((float(m_elps_t) - float(h * 3600)) / 60));
    const int s = m_elps_t - (h * 3600) - (m * 60);
    QString hrs = QString::number(h).rightJustified(2, '0');
    QString min = QString::number(m).rightJustified(2, '0');
    QString sec = QString::number(s).rightJustified(2, '0');
    m_message = tr("Pause\n\n Resume after: ") + QString("%1:%2:%3").arg(hrs, min, sec);
    show_message();
    m_elps_t--;
}
