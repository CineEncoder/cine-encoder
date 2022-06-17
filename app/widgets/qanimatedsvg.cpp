#include "qanimatedsvg.h"
#include <QPainter>

#define INTERVAL 900


QAnimatedSvg::QAnimatedSvg(QLabel *parent, const QSize &size) :
    QObject(parent),
    m_pLabel(parent),
    m_fileName(QString()),
    m_size(size),
    m_degree(0)
{
    m_pTimer = new QTimer(this);
    m_pTimer->setInterval(INTERVAL);
    connect(m_pTimer, &QTimer::timeout, this, &QAnimatedSvg::updateView);
}

QAnimatedSvg::~QAnimatedSvg()
{

}

void QAnimatedSvg::setFileName(const QString &fileName)
{
    m_fileName = fileName;
    if (m_pLabel && !m_fileName.isEmpty()) {
        QPixmap pix(m_fileName);
        QPixmap scal_pixmap(pix.scaled(m_size,
                                          Qt::KeepAspectRatio,
                                          Qt::SmoothTransformation));
        m_pLabel->setPixmap(scal_pixmap);
    }
}

void QAnimatedSvg::start()
{
    m_pTimer->start();
}

void QAnimatedSvg::stop()
{
    m_pTimer->stop();
}

void QAnimatedSvg::updateView()
{
    if (m_pLabel && !m_fileName.isEmpty()) {
        if (m_degree > 350)
            m_degree = 25;
        m_degree += 35;
        QPixmap pix(m_fileName);
        QPixmap rotPix(pix.size());
        rotPix.fill(QColor::fromRgb(0, 0, 0, 0));
        QSizeF size = QSizeF(pix.size());
        QPainter *p = new QPainter(&rotPix);
        p->translate(size.height()/2, size.height()/2);
        p->rotate(m_degree);
        p->translate(-size.height()/2, -size.height()/2);
        p->drawPixmap(0, 0, pix);
        p->end();
        delete p;
        QPixmap scal_pixmap(rotPix.scaled(m_size,
                                          Qt::KeepAspectRatio,
                                          Qt::SmoothTransformation));
        m_pLabel->setPixmap(scal_pixmap);
    }
}
