/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: donate.cpp
 MODIFIED: November, 2021
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "donate.h"
#include "ui_donate.h"



Donate::Donate(QWidget *parent):
    QDialog(parent),
    ui(new Ui::Donate),
    _clickPressedFlag(false)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);

    ui->frame_top->installEventFilter(this);
}

Donate::~Donate()
{
    delete ui;
}

void Donate::setParameters()
{
    QFont font;
    font.setPointSize(10);
    ui->label_title->setFont(font);
    QString text("");
    QFile file(":/resources/html/donate.html");
    if (file.open(QIODevice::ReadOnly)) {
        text = QString(file.readAll());
        file.close();
    }
    QString mod_text = text.arg(tr("This software is free for personal and commercial use. "
                                "It is distributed in the hope that it is useful but without "
                                "any warranty. See the GNU General Public Licence v3 for more "
                                "information."), tr("If you find this application useful, "
                                "consider making a donation to support the development."));
    ui->textBrowser->setHtml(mod_text);
    ui->textBrowser->setOpenExternalLinks(true);
}

void Donate::on_buttonCancel_clicked()   /*** Close window ***/
{
    this->close();
}

void Donate::on_closeWindow_clicked()   /*** Close window ***/
{
    this->close();
}

void Donate::on_buttonPayPal_clicked()   /*** Open browser PayPal ***/
{
    QDesktopServices::openUrl(QUrl("https://paypal.me/KozhukharenkoOleg?locale.x=ru_RU", QUrl::TolerantMode));
}

void Donate::on_buttonBitcoin_clicked()   /*** Open browser Bitcoin ***/
{
    QDesktopServices::openUrl(QUrl("https://github.com/CineEncoder/cine-encoder", QUrl::TolerantMode));
}

bool Donate::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->button() == Qt::LeftButton) {
            _clickPressedFlag = false;
            return true;
        }
        return QDialog::eventFilter(watched, event);
    }

    if (watched == ui->frame_top) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton) {
                _mouseClickCoordinate = mouse_event->pos();
                _clickPressedFlag = true;
                return true;
            }
            return QDialog::eventFilter(watched, event);
        }

        if ((event->type() == QEvent::MouseMove) && _clickPressedFlag) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton) {
                this->move(mouse_event->globalPos() - _mouseClickCoordinate);
                return true;
            }
            return QDialog::eventFilter(watched, event);
        }
        return QDialog::eventFilter(watched, event);
    }
    return QDialog::eventFilter(watched, event);
}
