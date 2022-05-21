/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: donate.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "notification.h"
#include "ui_notification.h"
#include <QDesktopServices>
#include <QFile>


Notification::Notification(QWidget *parent, MessConf mess_conf, const QString &title):
    BaseWindow(parent, false),
    ui(new Ui::Notification),
    m_mess_conf(mess_conf)
{
    QWidget *ui_widget = new QWidget(this);
    setCentralWidget(ui_widget);
    ui->setupUi(ui_widget);
    setTitleBar(ui->frame_top);
#ifdef Q_OS_UNIX
    setMaskWidget(ui_widget);
#endif
    QFont font;
    font.setPointSize(10);
    ui->label_title->setFont(font);
    ui->label_title->setText(title);
    connect(ui->closeWindow, &QPushButton::clicked, this, &Notification::onCloseWindow);
    connect(ui->buttonCancel, &QPushButton::clicked, this, &Notification::onCloseWindow);
    if (m_mess_conf == MessConf::AllBtns) {
        connect(ui->buttonPayPal, &QPushButton::clicked, this, &Notification::onButtonPayPal);
        connect(ui->buttonBitcoin, &QPushButton::clicked, this, &Notification::onButtonBitcoin);
    } else {
        ui->spacerPayPal->changeSize(0,0);
        ui->spacerBitcoin->changeSize(0,0);
        ui->buttonPayPal->hide();
        ui->buttonPayPal->setFixedWidth(0);
        ui->buttonBitcoin->hide();
        ui->buttonBitcoin->setFixedWidth(0);
    }
    setMessage();
}

Notification::~Notification()
{
    delete ui;
}

void Notification::setMessage()
{
    QString text("");
    QString mod_text("");
    QString fileName = (m_mess_conf == MessConf::AllBtns) ?
                ":/resources/html/donate.html" : ":/resources/html/about.html";
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        text = QString(file.readAll());
        file.close();
    }
    if (m_mess_conf == MessConf::AllBtns) {
        mod_text = text.arg(tr("This software is free for personal and commercial use. "
                               "It is distributed in the hope that it is useful but without "
                               "any warranty. See the GNU General Public Licence v3 for more "
                               "information."), tr("If you find this application useful, "
                               "consider making a donation to support the development."));
    } else {
        mod_text = text.arg(tr("Program for encoding HDR and SDR video."), tr("This software is free "
                               "for personal and commercial use. It is distributed in the hope that it"
                               " is useful but without any warranty. See the GNU General Public Licence"
                               " v3 for more information."), tr("License: GNU General Public License "
                               "Version 3"), tr("Copyright"));
    }
    ui->textBrowser->setHtml(mod_text);
    ui->textBrowser->setOpenExternalLinks(true);
}

void Notification::onCloseWindow()
{
    closeDialog();
}

void Notification::onButtonPayPal()   /*** Open browser PayPal ***/
{
    QDesktopServices::openUrl(QUrl("https://paypal.me/KozhukharenkoOleg?locale.x=ru_RU", QUrl::TolerantMode));
}

void Notification::onButtonBitcoin()   /*** Open browser Bitcoin ***/
{
    QDesktopServices::openUrl(QUrl("https://github.com/CineEncoder/cine-encoder", QUrl::TolerantMode));
}
