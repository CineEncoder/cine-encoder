/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: about.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "about.h"
#include "ui_about.h"


About::About(QWidget *parent):
    FramelessWindow(parent),
    ui(new Ui::About),
    _clickPressedFlag(false)
{
    ui->setupUi(this);
    ui->frame_top->installEventFilter(this);
}

About::~About()
{
    delete ui;
}

void About::setParameters() /*** Set parameters ***/
{
    QFont font;
    font.setPointSize(10);
    ui->label_title->setFont(font);
    QString text("");
    QFile file(":/resources/html/about.html");
    if (file.open(QIODevice::ReadOnly)) {
        text = QString(file.readAll());
        file.close();
    }
    QString mod_text = text.arg(tr("Program for encoding HDR and SDR video."), tr("This software is free "
                                   "for personal and commercial use. It is distributed in the hope that it"
                                   " is useful but without any warranty. See the GNU General Public Licence"
                                   " v3 for more information."), tr("License: GNU General Public License "
                                   "Version 3"), tr("Copyright"));

    ui->textBrowser->setHtml(mod_text);
    ui->textBrowser->setOpenExternalLinks(true);
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
                _mouseClickCoordinate = mouse_event->pos() + QPoint(10,10);
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
