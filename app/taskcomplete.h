/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: taskcomplete.h
 MODIFIED: November, 2021
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef TASKCOMPLETE_H
#define TASKCOMPLETE_H

#include <QDialog>
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


class Taskcomplete : public QDialog
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

    int elps_t;

    bool clickPressedFlag;

    QPoint mouseClickCoordinate;

    void show_message(QString _message);
};

#endif // TASKCOMPLETE_H
