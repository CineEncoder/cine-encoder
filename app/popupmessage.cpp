/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: popupmessage.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "popupmessage.h"
#include "ui_popupmessage.h"
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QLayout>
#include <QTextBrowser>
#include <QPushButton>
#include <QTimer>

#define OFFSET QPoint(374, -20)


PopupMessage::PopupMessage(QWidget *parent, Icon icon, const QString &text) :
    BaseDialog(parent, false),
    ui(new Ui::PopupMessage),
    m_activated(false)
{
    setWindowFlags((windowFlags() & ~Qt::Dialog & ~Qt::SubWindow) | Qt::Tool);
    setWindowModality(Qt::NonModal);
    ui_widget = new QWidget(this);
    layout()->addWidget(ui_widget);
    ui->setupUi(ui_widget);
    setMaskWidget(ui_widget);
    ui_widget->setAttribute(Qt::WA_TranslucentBackground);
    ui_widget->setAutoFillBackground(true);

    QPixmap pxm(":/resources/icons/svg/popup.svg");
    ui->imageLabel->setPixmap(pxm.scaled(ui_widget->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    QGridLayout *lt = new QGridLayout(ui->imageLabel);
    lt->setContentsMargins(12, 34, 6, 6);
    lt->setSpacing(6);
    ui->imageLabel->setLayout(lt);

    QLabel *lab = new QLabel(ui->imageLabel);
    lab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lab->setFixedSize(QSize(30, 30));

    QString iconPath(":/resources/icons/svg/info.svg");
    switch (icon) {
    case Icon::Warning:
        iconPath = QString(":/resources/icons/svg/warning.svg");
        break;
    case Icon::Critical:
        iconPath = QString(":/resources/icons/svg/error.svg");
        break;
    default:
        break;
    }
    QPixmap logo(iconPath);
    lab->setPixmap(logo.scaled(QSize(30,30), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    lt->addWidget(lab, 0, 0, Qt::AlignTop);

    QTextBrowser *br = new QTextBrowser(ui->imageLabel);
    br->setEnabled(false);
    br->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lt->addWidget(br, 0, 1);
    br->setText(text);
    br->setStyleSheet("color: #303030");

    QTimer *tmr = new QTimer(this);
    tmr->setSingleShot(false);
    tmr->setInterval(250);
    connect(tmr, &QTimer::timeout, this, &PopupMessage::moveWidget);
    tmr->start();
}

PopupMessage::~PopupMessage()
{
    delete ui;
}

bool PopupMessage::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        this->deleteLater();
        return true;
    }
    return BaseDialog::eventFilter(obj, event);
}

void PopupMessage::showEvent(QShowEvent *event)
{
    BaseDialog::showEvent(event);
    if (!m_activated) {
        m_activated = true;
        moveWidget();
        //showEffect(EffectType::Arise);
        QTimer::singleShot(5000, this, [this]() {
            showEffect(EffectType::Fade);
        });
    }
}

void PopupMessage::moveWidget()
{
    if (parentWidget()) {
        auto cw = parentWidget()->findChild<QWidget*>("centralwidget");
        if (cw)
            move(cw->mapToGlobal(cw->geometry().topRight()) - OFFSET);
    }
}

void PopupMessage::showEffect(const EffectType efType)
{
    if (efType == EffectType::Arise) {
        /*anm->setDuration(800);
        anm->setStartValue(0);
        anm->setEndValue(1);
        anm->setEasingCurve(QEasingCurve::InCurve);*/
    } else
    if (efType == EffectType::Fade) {
        QGraphicsOpacityEffect *m_pGrEffect = new QGraphicsOpacityEffect(ui_widget);
        ui_widget->setGraphicsEffect(m_pGrEffect);
        QPropertyAnimation *anm = new QPropertyAnimation(m_pGrEffect, "opacity");
        anm->setDuration(2000);
        anm->setStartValue(1);
        anm->setEndValue(0);
        connect(anm, &QPropertyAnimation::finished, this, [this](){
            this->deleteLater();
        });
        anm->start(QPropertyAnimation::DeleteWhenStopped);
    }
}
