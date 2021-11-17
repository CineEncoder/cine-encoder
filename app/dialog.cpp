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



Dialog::Dialog(QWidget *parent): QDialog(parent), ui_dialog(new Ui::Dialog)
{
    ui_dialog->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);

    ui_dialog->frame_top->installEventFilter(this);
}

Dialog::~Dialog()
{
    delete ui_dialog;
}

void Dialog::setMessage(const QString &_message)   /*** Set parameters ***/
{
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    QFont font;
    font.setPointSize(10);
    ui_dialog->label_title->setFont(font);
#ifdef Q_OS_WIN
    QSound::play("./cine-encoder.wav");
#else
    QSound::play("/usr/share/sounds/cine-encoder.wav");
#endif
    show_message(_message);
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

void Dialog::show_message(QString _message)   /*** Show message ***/
{
    ui_dialog->textBrowser->clear();
    ui_dialog->textBrowser->setAlignment(Qt::AlignCenter);
    ui_dialog->textBrowser->append(_message);
}

bool Dialog::eventFilter(QObject *watched, QEvent *event)
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

    if (watched == ui_dialog->frame_top)
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

