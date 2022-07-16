#include "qdoublebutton.h"
#include <QVariant>
#include <QStyle>
#include <QHBoxLayout>


QDoubleButton::QDoubleButton(QWidget *parent) :
    QWidget(parent),
    m_index(0)
{
    QHBoxLayout *lut = new QHBoxLayout(this);
    lut->setContentsMargins(0,0,0,0);
    lut->setSpacing(0);
    setLayout(lut);

    m_pLButton = new QPushButton(this);
    m_pLButton->setObjectName("leftButton");
    lut->addWidget(m_pLButton);

    m_pRButton = new QPushButton(this);
    m_pRButton->setObjectName("rightButton");
    lut->addWidget(m_pRButton);

    connect(m_pLButton, &QPushButton::clicked, this, &QDoubleButton::onLeft);
    connect(m_pRButton, &QPushButton::clicked, this, &QDoubleButton::onRight);
    m_pLButton->setEnabled(false);
    m_pRButton->setEnabled(true);
}

QDoubleButton::~QDoubleButton()
{

}

void QDoubleButton::setIcons(const QIcon &lIcon, const QIcon &rIcon)
{
    m_pLButton->setIconSize(QSize(20,20));
    m_pRButton->setIconSize(QSize(20,20));
    m_pLButton->setIcon(lIcon);
    m_pRButton->setIcon(rIcon);
}

uchar QDoubleButton::currentIndex()
{
    return m_index;
}

void QDoubleButton::setCurrentIndex(uchar index)
{
    m_index = index == 0 ? 0 : 1;
    switchButtons();
}

void QDoubleButton::setToolTips(const QString &lTip, const QString &rTip)
{
    m_pLButton->setToolTip(lTip);
    m_pRButton->setToolTip(rTip);
}

void QDoubleButton::onLeft()
{
    m_index = 0;
    switchButtons();
}

void QDoubleButton::onRight()
{
    m_index = 1;
    switchButtons();
}

void QDoubleButton::switchButtons()
{
    m_pLButton->setEnabled(m_index != 0);
    m_pRButton->setEnabled(m_index == 0);
    emit indexChanged(m_index);
}
