/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: donate.h
 MODIFIED: September, 2021
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef DONATE_H
#define DONATE_H

#include <QDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QMouseEvent>

namespace Ui
{
class Donate;
}


class Donate : public QDialog
{
    Q_OBJECT

public:

    explicit Donate(QWidget *parent = nullptr);

    ~Donate();

    void setParameters();

private slots:

    void on_buttonCancel_clicked();

    void on_buttonPayPal_clicked();

    void on_buttonBitcoin_clicked();

    void on_closeWindow_clicked();

    bool eventFilter(QObject *watched, QEvent *event);

private:

    Ui::Donate *ui_donate;

    bool clickPressedFlag = false;

    QPoint mouseClickCoordinate;
};

#endif // DONATE_H
