/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: progress.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef PROGRESS_H
#define PROGRESS_H

#include "basedialog.h"


namespace Ui
{
    class Progress;
}

class Progress : public BaseDialog
{
    Q_OBJECT
public:
    explicit Progress(QWidget *parent, const QString &title);
    ~Progress();
    void setText(const QString &text);
    void setPercent(const int percent);

private:
    Ui::Progress *ui;
};

#endif // PROGRESS_H
