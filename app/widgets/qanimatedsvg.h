#ifndef QANIMATEDSVG_H
#define QANIMATEDSVG_H

#include <QObject>
#include <QLabel>
#include <QTimer>


class QAnimatedSvg : public QObject
{
    Q_OBJECT
public:
    QAnimatedSvg(QLabel *parent, const QSize &size);
    ~QAnimatedSvg();
    void setFileName(const QString &);
    void start();
    void stop();

private:
    void updateView();
    QTimer *m_pTimer;
    QLabel *m_pLabel;
    QString m_fileName;
    QSize   m_size;
    int     m_degree;
};

#endif // QANIMATEDSVG_H
