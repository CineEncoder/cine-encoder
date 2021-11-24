/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: dialog.cpp
 MODIFIED: November, 2021
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "dialog.h"
#include "ui_dialog.h"



Dialog::Dialog(QWidget *parent):
    QDialog(parent),
    ui(new Ui::Dialog),
    _clickPressedFlag(false)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);

    ui->frame_top->installEventFilter(this);
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::setMessage(const QString &message)   /*** Set parameters ***/
{
    QFont font;
    font.setPointSize(10);
    ui->label_title->setFont(font);
#if defined (Q_OS_WIN64)
    QSound::play("./cine-encoder.wav");
#elif defined (Q_OS_UNIX)
    QSound::play("/usr/share/sounds/cine-encoder.wav");
#endif
    ui->textBrowser->clear();
    ui->textBrowser->setAlignment(Qt::AlignCenter);
    ui->textBrowser->append(message);
}

void Dialog::on_buttonApply_clicked()
{
    this->accept();
}

void Dialog::on_buttonCancel_clicked()
{
    this->close();
}

void Dialog::on_closeWindow_clicked()
{
    this->close();
}

bool Dialog::eventFilter(QObject *watched, QEvent *event)
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
