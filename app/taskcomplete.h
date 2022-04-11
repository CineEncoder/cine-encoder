/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: taskcomplete.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef TASKCOMPLETE_H
#define TASKCOMPLETE_H

#include "framelesswindow.h"
#include <QTimer>
#include <QSound>
#include <QMouseEvent>
#include <math.h>
#include <iomanip>
#include <sstream>


namespace Ui
{
    class Taskcomplete;
}


class Taskcomplete : public FramelessWindow
{
    Q_OBJECT

public:

    explicit Taskcomplete(QWidget *parent = nullptr);
    ~Taskcomplete();

    void setMessage(const QString &_message, const bool &_timer_mode);

private slots:

    void on_closeWindow_clicked();
    void on_buttonCancel_clicked();
    void repeatHandler();
    bool eventFilter(QObject *watched, QEvent *event);

private:

    Ui::Taskcomplete *ui;

    QTimer *timer;

    QString _message;

    int _elps_t;

    bool _clickPressedFlag;

    QPoint _mouseClickCoordinate;

    void show_message(QString _message);
};

#endif // TASKCOMPLETE_H
