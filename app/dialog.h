/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: dialog.h
 MODIFIED: September, 2021
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSound>
#include <QMouseEvent>


namespace Ui
{
    class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:

    explicit Dialog(QWidget *parent = nullptr);

    ~Dialog();

    void setMessage(const QString &_message);

private slots:

    void on_buttonApply_clicked();

    void on_buttonCancel_clicked();

    void on_closeWindow_clicked();

    bool eventFilter(QObject *watched, QEvent *event);

private:

    Ui::Dialog *ui_dialog;

    QString _message;

    bool clickPressedFlag = false;

    QPoint mouseClickCoordinate;

    void show_message(QString _message);
};

#endif // DIALOG_H
