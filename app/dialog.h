/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: dialog.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef DIALOG_H
#define DIALOG_H

#include <QSound>
#include <QMouseEvent>
#include "framelesswindow.h"


namespace Ui
{
    class Dialog;
}

class Dialog : public FramelessWindow
{
    Q_OBJECT

public:

    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

    void setMessage(const QString &message);

private slots:

    void on_buttonApply_clicked();
    void on_buttonCancel_clicked();
    void on_closeWindow_clicked();
    bool eventFilter(QObject *watched, QEvent *event);

private:

    Ui::Dialog *ui;

    bool _clickPressedFlag;

    QPoint _mouseClickCoordinate;
};

#endif // DIALOG_H
