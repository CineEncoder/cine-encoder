/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: openingfiles.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef OPENINGFILES_H
#define OPENINGFILES_H

#include <QDialog>
#include <QFontMetrics>
#include <math.h>


namespace Ui
{
    class OpeningFiles;
}

class OpeningFiles : public QDialog
{
    Q_OBJECT

public:

    explicit OpeningFiles(QWidget *parent = nullptr);
    ~OpeningFiles();

    void setParameters(const bool &show, const QPoint &position);
    void setText(const QString &text);
    void setPercent(const int &percent);

private:

    Ui::OpeningFiles *ui;
};

#endif // OPENINGFILES_H
