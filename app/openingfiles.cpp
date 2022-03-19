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
#include <QGraphicsDropShadowEffect>


OpeningFiles::OpeningFiles(QWidget *parent):
    QDialog(parent),
    ui(new Ui::OpeningFiles)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setAttribute(Qt::WA_TranslucentBackground);

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(ui->widget_main);
    shadow->setBlurRadius(10.0);
    shadow->setColor(QColor(0, 0, 0, 160));
    shadow->setOffset(0.0);
    ui->widget_main->setGraphicsEffect(shadow);
}

OpeningFiles::~OpeningFiles()
{
    delete ui;
}

void OpeningFiles::setParameters(const bool &show, const QPoint &position)
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

void OpeningFiles::setText(const QString &text)
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

void OpeningFiles::setPercent(const int &percent)
{
    ui->progressBar_opening->setValue(percent);
}

