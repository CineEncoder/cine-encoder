#include "report.h"
#include "ui_report.h"
#include <QMouseEvent>
#include <QLayout>
#include <QToolTip>
#include <QTimer>
#include <iostream>

#define OFFSET QPoint(530, -30)


Report::Report(QWidget *parent, const QVector<ReportLog> &reportLog) :
    BaseDialog(parent, false),
    ui(new Ui::Report),
    m_activated(false)
{
    setWindowFlags((windowFlags() & ~Qt::Dialog & ~Qt::SubWindow) | Qt::Popup);
    setWindowModality(Qt::NonModal);
    setAttribute(Qt::WA_DeleteOnClose);
    QWidget *ui_widget = new QWidget(this);
    layout()->addWidget(ui_widget);
    ui->setupUi(ui_widget);
    setMaskWidget(ui_widget);
    ui_widget->setAutoFillBackground(true);

    setEnabled(true);
    connect(ui->closeReport, &QPushButton::clicked, this, [this]() {
        close();
    });

    QTimer *tmr = new QTimer(this);
    tmr->setSingleShot(false);
    tmr->setInterval(250);
    connect(tmr, &QTimer::timeout, this, &Report::moveWidget);
    tmr->start();

    QFont fnt = ui->reportTable->horizontalHeader()->font();
    fnt.setBold(true);
    fnt.setItalic(true);
    ui->reportTable->setEnabled(true);
    ui->reportTable->horizontalHeader()->setFont(fnt);
    ui->reportTable->horizontalHeader()->setVisible(true);
    ui->reportTable->verticalHeader()->setVisible(true);
    ui->reportTable->setShowGrid(false);
    ui->reportTable->setColumnWidth(0, 80);
    foreach (const ReportLog &a, reportLog) {
        const int ind = ui->reportTable->rowCount();
        ui->reportTable->setRowCount(ind + 1);

        QTableWidgetItem *__item_time = new QTableWidgetItem(a.reportTime);
        __item_time->setTextAlignment(Qt::AlignCenter);
        ui->reportTable->setItem(ind, 0, __item_time);

        QString msg = a.reportMsg;
        msg.replace("\n", "");
        QTableWidgetItem *__item_msg = new QTableWidgetItem(msg);
        __item_msg->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        ui->reportTable->setItem(ind, 1, __item_msg);

        QString iconPath(":/resources/icons/svg/info.svg");
        switch (a.reportIcon) {
        case ReportLog::Icon::Warning:
            iconPath = QString(":/resources/icons/svg/warning.svg");
            break;
        case ReportLog::Icon::Critical:
            iconPath = QString(":/resources/icons/svg/error.svg");
            break;
        default:
            break;
        }
        QPixmap pxmp(iconPath);

        QPixmap scaled = pxmp.scaled(QSize(30,30), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        ui->reportTable->item(ind, 1)->setIcon(QIcon(scaled));
    }
    const int ind = ui->reportTable->rowCount();
    if (ind > 0)
        ui->reportTable->selectRow(ind - 1);
    ui->reportTable->setMouseTracking(true);
    connect(ui->reportTable, &QTableWidget::itemEntered, this, &Report::onItemEntered);
}

Report::~Report()
{
    delete ui;
}

void Report::showEvent(QShowEvent *event)
{
    BaseDialog::showEvent(event);
    if (!m_activated) {
        m_activated = true;
        moveWidget();
    }
}

void Report::onItemEntered(QTableWidgetItem *item)
{
    if (item) {
        if (item->column() == 1)
            item->setToolTip(item->text());
    }
}

void Report::moveWidget()
{
    if (parentWidget()) {
        auto cw = parentWidget()->findChild<QWidget*>("centralwidget");
        if (cw)
            move(cw->mapToGlobal(cw->geometry().topRight()) - OFFSET);
    }
}
