#ifndef REPORT_H
#define REPORT_H

#include "basedialog.h"
#include "constants.h"
#include <QTableWidgetItem>

using namespace Constants;

namespace Ui
{
    class Report;
}

class Report : public BaseDialog
{
    Q_OBJECT
public:
    explicit Report(QWidget *parent, const QVector<ReportLog> &reportLog);
    ~Report();

private:
    virtual void showEvent(QShowEvent*) final;
    void onItemEntered(QTableWidgetItem*);
    void moveWidget();
    Ui::Report *ui;
    bool m_activated;
};

#endif // REPORT_H
