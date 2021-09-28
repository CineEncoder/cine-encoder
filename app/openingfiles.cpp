/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: openingfiles.cpp
 MODIFIED: September, 2021
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "openingfiles.h"
#include "ui_openingfiles.h"


OpeningFiles::OpeningFiles(QWidget *parent): QDialog(parent), ui(new Ui::OpeningFiles)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
}

OpeningFiles::~OpeningFiles()
{
    delete ui;
}

void OpeningFiles::setParameters(bool &show, QPoint &position)
{
    QSize sizeWindow = this->size();
    int x_pos = position.x() - static_cast<int>(round(static_cast<float>(sizeWindow.width())/2));
    int y_pos = position.y() - static_cast<int>(round(static_cast<float>(sizeWindow.height())/2));
    this->setGeometry(x_pos, y_pos, sizeWindow.width(), sizeWindow.height());
    if (show) {
        this->show();
    }
    else {
        this->hide();
    }
}

void OpeningFiles::setText(QString &text)
{
    QString elidedText;
    QFontMetrics fm = ui->label_filename->fontMetrics();
    int fwidth = fm.horizontalAdvance(text);
    int width = ui->label_filename->width();
    if (fwidth > width) {
        elidedText = fm.elidedText(text, Qt::ElideMiddle, width, 0);
    } else {
        elidedText = text;
    }
    ui->label_filename->setText(elidedText);
}

void OpeningFiles::setPercent(int &percent)
{
    ui->progressBar_opening->setValue(percent);
}

