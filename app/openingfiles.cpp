#include "openingfiles.h"
#include "ui_openingfiles.h"
#include <math.h>

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
    ui->label_filename->setText(text);
}

void OpeningFiles::setPercent(int &percent)
{
    ui->progressBar_opening->setValue(percent);
}

