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
#include "helper.h"


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
    const QString elidedText = Helper::elideText(ui->label_filename, text, Qt::ElideMiddle);
    ui->label_filename->setText(elidedText);
}

void Progress::setPercent(const int percent)
{
    ui->progressBar_opening->setValue(percent);
}

