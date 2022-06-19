/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: progress.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "progress.h"
#include "ui_progress.h"
#include <QFontMetrics>
#include <math.h>


Progress::Progress(QWidget *parent, const QString &title):
    BaseDialog(parent, false),
    ui(new Ui::Progress)
{
    QWidget *ui_widget = new QWidget(this);
    layout()->addWidget(ui_widget);
    ui->setupUi(ui_widget);
    setMaskWidget(ui_widget);
    ui->label_title->setText(title);
}

Progress::~Progress()
{
    delete ui;
}

void Progress::setText(const QString &text)
{
    QFontMetrics fm = ui->label_filename->fontMetrics();
#if (QT_VERSION < QT_VERSION_CHECK(5,11,0))
    const int fwidth = fm.width(text);
#else
    const int fwidth = fm.horizontalAdvance(text);
#endif
    const int width = ui->label_filename->width();
    const QString elidedText = (fwidth > width) ? fm.elidedText(text, Qt::ElideMiddle, width, 0) : text;
    ui->label_filename->setText(elidedText);
}

void Progress::setPercent(const int percent)
{
    ui->progressBar_opening->setValue(percent);
}

