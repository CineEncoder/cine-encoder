#ifndef QSTREAMVIEWEXTERN_H
#define QSTREAMVIEWEXTERN_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include "constants.h"


class QStreamViewExtern : public QWidget
{
    Q_OBJECT
public:
    QStreamViewExtern(QWidget *parent);
    ~QStreamViewExtern();
    void clearList();
    void setList(Data &data);

private:
    QWidget *createCell(bool &state,
                        const QString &format,
                        QString &lang,
                        QString &title);
    QVBoxLayout *m_pLayout;
    QLabel      *m_label;
};

#endif // QSTREAMVIEWEXTERN_H
