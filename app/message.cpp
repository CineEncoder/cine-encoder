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
#include "helper.h"
#include <QSound>

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
    ui->setupUi(centralWidget());
    setTitleBar(ui->frame_top);
    connect(ui->closeWindow, &QPushButton::clicked, this, &Message::onCloseWindow);
    connect(ui->buttonApply, &QPushButton::clicked, this, &Message::onButtonApply);
    Icon icon = Icon::Info;
    if (m_mess_type == MessType::INFO) {
        ui->spacerCancel->changeSize(0,0);
        ui->buttonCancel->hide();
        ui->buttonCancel->setFixedWidth(0);
        icon = Icon::Warning;
    } else {
        connect(ui->buttonCancel, &QPushButton::clicked, this, &Message::onCloseWindow);
    }    
    QString iconPath(":/resources/icons/svg/info.svg");
    switch (icon) {
    case Icon::Warning:
        iconPath = QString(":/resources/icons/svg/warning.svg");
        break;
    case Icon::Critical:
        iconPath = QString(":/resources/icons/svg/error.svg");
        break;
    default:
        break;
    }
    QPixmap logo(iconPath);
    ui->label->setPixmap(logo.scaled(QSize(20, 20), Qt::KeepAspectRatio, Qt::SmoothTransformation));
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
        QSizeF size(this->size());
        QPoint center = QPointF(size.width()/2, size.height()/2).toPoint();
        move(parentWidget()->geometry().center() - center);
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
    } else
    if (m_mess_type == MessType::INFO && !m_timer_flag) {
#if defined (Q_OS_WIN64)
        QSound::play("./cine-encoder.wav");
#elif defined (Q_OS_UNIX)
        QSound::play("/usr/share/sounds/cine-encoder.wav");
#endif
        show_message();
    } else {
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
    if (m_elps_t == 0)
        close();
    m_message = QString("%1\n\n %2: %3").arg(tr("Pause"), tr("Resume after:"),
                Helper::timeConverter(float(m_elps_t)));
    show_message();
    m_elps_t--;
}
