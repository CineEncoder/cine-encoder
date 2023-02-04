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
    ui(new Ui::Progress),
    m_windowActivated(false)
{
    QWidget *ui_widget = new QWidget(this);
    layout()->addWidget(ui_widget);
    ui->setupUi(ui_widget);
    setMaskWidget(ui_widget);
    ui->frame_main->setProperty("scale", int(Helper::scaling() * 100));
    ui->label_title->setText(title);
}

Progress::~Progress()
{
    delete ui;
}

void Progress::showEvent(QShowEvent *event)
{
    BaseDialog::showEvent(event);
    if (!m_windowActivated) {
        m_windowActivated = true;
        resize(QSize(302, 120) * Helper::scaling());
        QSizeF size(this->size());
        QPoint center = QPointF(size.width()/2, size.height()/2).toPoint();
        move(parentWidget()->geometry().center() - center);
    }
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

