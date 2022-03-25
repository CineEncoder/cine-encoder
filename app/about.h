/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: about.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>


namespace Ui
{
    class About;
}


class About : public QDialog
{
    Q_OBJECT

public:

    explicit About(QWidget *parent = nullptr);
    ~About();

    void setParameters();

private slots:

    void on_buttonCancel_clicked();
    void on_closeWindow_clicked();
    bool eventFilter(QObject *watched, QEvent *event);

private:

    Ui::About *ui;

    bool _clickPressedFlag;

    QPoint _mouseClickCoordinate;
};

#endif // ABOUT_H
