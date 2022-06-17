/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: basedialog.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "basewindow_cwm.h"
#include "qboxlayout.h"
#include <QApplication>
#include <QMouseEvent>
#include <QIcon>
#include <QTimer>
#include <QGraphicsDropShadowEffect>

#define BORDER 15


BaseWindow::BaseWindow(QWidget *parent, bool isReizable) :
    QMainWindow(parent, Qt::Window | Qt::FramelessWindowHint),
    m_titlebar(nullptr),
    m_maskwidget(nullptr),
    m_mouseClickCoordinate(QPoint()),
    m_windowActivated(false),
    m_clickPressedFlag(false),
    m_isResizable(isReizable),
    m_clickPressedToResizeFlag(8, false)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowModal);
    setAttribute(Qt::WA_Hover, true);
    QWidget *ui_widget = new QWidget(this);
    setCentralWidget(ui_widget);
    setMaskWidget(ui_widget);
    installEventFilter(this);
    setWindowIcon(QIcon(QPixmap(":/resources/icons/svg/cine-encoder.svg")));
}

BaseWindow::~BaseWindow()
{

}

int BaseWindow::exec()
{
    show();
    setWindowModality(Qt::ApplicationModal);
    QEventLoop loop;
    connect(this, &BaseWindow::exitLoop, &loop, &QEventLoop::exit);
    const int code = loop.exec();
    hide();
    this->deleteLater();
    return code;
}

void BaseWindow::acceptDialog()
{
    emit BaseWindow::exitLoop(Dialog::Accept);
}

void BaseWindow::closeDialog()
{
    emit BaseWindow::exitLoop(Dialog::Decline);
}

void BaseWindow::setTitleBar(QWidget* titlebar)
{
    m_titlebar = titlebar;
    m_titlebar->installEventFilter(this);
}

void BaseWindow::setMaskWidget(QWidget* maskwidget)
{
    m_maskwidget = maskwidget;
    m_maskwidget->setAttribute(Qt::WA_Hover, true);
    m_maskwidget->setAttribute(Qt::WA_NoMousePropagation, true);
    m_maskwidget->installEventFilter(this);
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(m_maskwidget);
    shadow->setBlurRadius(25.0);
    shadow->setColor(QColor(0, 0, 0, 80));
    shadow->setOffset(0.0);
    m_maskwidget->setGraphicsEffect(shadow);
}

void BaseWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    if (!m_windowActivated) {
        m_windowActivated = true;
        setContentsMargins(BORDER, BORDER, BORDER, BORDER);
    }
}

void BaseWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        const int margin = isMaximized() ? 0 : BORDER;
        setContentsMargins(margin, margin, margin, margin);
        QTimer::singleShot(50, this, [this](){
            update();
        });
    }
}

bool BaseWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->button() == Qt::LeftButton) {
            setCursor(QCursor(Qt::ArrowCursor));
            m_clickPressedFlag = false;
            m_clickPressedToResizeFlag.fill(false);
            return true;
        }
    } else
    if (watched == m_maskwidget) {
        if (event->type() == QEvent::HoverEnter) {
            setCursor(QCursor(Qt::ArrowCursor));
            /*QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
            QApplication::restoreOverrideCursor();*/
            return true;
        }
    } else
    if (watched == m_titlebar) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton) {
                m_mouseClickCoordinate = mouse_event->pos() +
                        QPoint(BORDER+2, BORDER+2);
                m_clickPressedFlag = true;
                return true;
            }
        } else
        if ((event->type() == QEvent::MouseMove) && m_clickPressedFlag) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton) {
                if (isMaximized() && mouse_event->globalPos().y() > 80) {
                    onExpandWindow();
                    QTimer::singleShot(50, this, [this, mouse_event](){
                        m_mouseClickCoordinate = QPoint(int(float(m_titlebar->width())/2), 30);
                        this->move(mouse_event->globalPos() - m_mouseClickCoordinate);
                    });
                } else
                if (!isMaximized()) {
                    this->move(mouse_event->globalPos() - m_mouseClickCoordinate);
                }
                return true;
            }
        } else
        if (event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton) {
                if (m_isResizable)
                    onExpandWindow();
                return true;
            }
        }
    } else
    if (watched == this) {
        if (!isMaximized() && m_isResizable) {
            /*if (event->type() == QEvent::HoverLeave) {
                QGuiApplication::restoreOverrideCursor();
                return true;
            } else*/
            if (event->type() == QEvent::HoverMove && m_clickPressedToResizeFlag.indexOf(true) == -1) {
                const QPoint mouseCoordinate = this->mapFromGlobal(QCursor::pos());
                if (mouseCoordinate.x() < BORDER) {
                    if (mouseCoordinate.y() < BORDER) {
                        setCursor(QCursor(Qt::SizeFDiagCursor));
                        return true;
                    } else
                    if (mouseCoordinate.y() > BORDER && mouseCoordinate.y() < (height() - BORDER)) {
                        setCursor(QCursor(Qt::SizeHorCursor));
                        return true;
                    } else
                    if (mouseCoordinate.y() > (height() - BORDER)) {
                        setCursor(QCursor(Qt::SizeBDiagCursor));
                        return true;
                    }
                } else
                if (mouseCoordinate.x() > BORDER && mouseCoordinate.x() < (width() - BORDER)) {
                    if (mouseCoordinate.y() < BORDER || mouseCoordinate.y() > (height() - BORDER)) {
                        setCursor(QCursor(Qt::SizeVerCursor));
                        return true;
                    }
                } else
                if (mouseCoordinate.x() > (width() - BORDER)) {
                    if (mouseCoordinate.y() < BORDER) {
                        setCursor(QCursor(Qt::SizeBDiagCursor));
                        return true;
                    } else
                    if (mouseCoordinate.y() > BORDER && mouseCoordinate.y() < (height() - BORDER)) {
                        setCursor(QCursor(Qt::SizeHorCursor));
                        return true;
                    } else
                    if (mouseCoordinate.y() > (height() - BORDER)) {
                        setCursor(QCursor(Qt::SizeFDiagCursor));
                        return true;
                    }
                }
                setCursor(QCursor(Qt::ArrowCursor));
            } else
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->button() == Qt::LeftButton) {
                    m_oldPosX = this->pos().x();
                    m_oldPosY = this->pos().y();
                    m_oldWidth = this->width();
                    m_oldHeight = this->height();
                    m_mouseClickCoordinate = mouse_event->pos();
                    m_globalMouseClickCoordinate = mouse_event->globalPos();
                    const QPoint mouseClickCoordinate = mouse_event->pos();
                    if (mouseClickCoordinate.x() < BORDER) {
                        if (mouseClickCoordinate.y() < BORDER) {
                            m_clickPressedToResizeFlag[Resize::LEFT_TOP] = true;
                            return true;
                        } else
                        if (mouseClickCoordinate.y() > BORDER && mouseClickCoordinate.y() < (m_oldHeight - BORDER)) {
                            m_clickPressedToResizeFlag[Resize::LEFT] = true;
                            return true;
                        } else
                        if (mouseClickCoordinate.y() > (m_oldHeight - BORDER)) {
                            m_clickPressedToResizeFlag[Resize::LEFT_BOTTOM] = true;
                            return true;
                        }
                    } else
                    if (mouseClickCoordinate.x() > BORDER && mouseClickCoordinate.x() < (m_oldWidth - BORDER)) {
                        if (mouseClickCoordinate.y() < BORDER) {
                            m_clickPressedToResizeFlag[Resize::TOP] = true;
                            return true;
                        } else
                        if (mouseClickCoordinate.y() > (m_oldHeight - BORDER)) {
                            m_clickPressedToResizeFlag[Resize::BOTTOM] = true;
                            return true;
                        }
                    } else
                    if (mouseClickCoordinate.x() > (m_oldWidth - BORDER)) {
                        if (mouseClickCoordinate.y() < BORDER) {
                            m_clickPressedToResizeFlag[Resize::RIGHT_TOP] = true;
                            return true;
                        } else
                        if (mouseClickCoordinate.y() > BORDER && mouseClickCoordinate.y() < (m_oldHeight - BORDER)) {
                            m_clickPressedToResizeFlag[Resize::RIGHT] = true;
                            return true;
                        } else
                        if (mouseClickCoordinate.y() > (m_oldHeight - BORDER)) {
                            m_clickPressedToResizeFlag[Resize::RIGHT_BOTTOM] = true;
                            return true;
                        }
                    }
                }
            } else
            if (event->type() == QEvent::MouseMove) {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->buttons() & Qt::LeftButton) {
                    const int index = m_clickPressedToResizeFlag.indexOf(true);
                    if (index != -1) {
                        const int deltaX = mouse_event->globalPos().x() - m_mouseClickCoordinate.x();
                        const int deltaY = mouse_event->globalPos().y() - m_mouseClickCoordinate.y();
                        const int deltaWidth = deltaX - m_globalMouseClickCoordinate.x() + m_mouseClickCoordinate.x();
                        const int deltaHeight = deltaY - m_globalMouseClickCoordinate.y() + m_mouseClickCoordinate.y();
                        switch (index) {
                        case Resize::LEFT:
                            if (deltaWidth > 0 && size().width() <= minimumSize().width()) break;
                            setGeometry(deltaX, m_oldPosY, m_oldWidth - deltaWidth, m_oldHeight);
                            break;
                        case Resize::LEFT_TOP:
                            setGeometry(deltaX, deltaY, m_oldWidth - deltaWidth, m_oldHeight - deltaHeight);
                            break;
                        case Resize::LEFT_BOTTOM:
                            setGeometry(deltaX, m_oldPosY, m_oldWidth - deltaWidth, m_oldHeight + deltaHeight);
                            break;
                        case Resize::TOP:
                            if (deltaHeight > 0 && size().height() <= minimumSize().height()) break;
                            setGeometry(m_oldPosX, deltaY, m_oldWidth, m_oldHeight - deltaHeight);
                            break;
                        case Resize::RIGHT:
                            setGeometry(m_oldPosX, m_oldPosY, m_oldWidth + deltaWidth, m_oldHeight);
                            break;
                        case Resize::RIGHT_TOP:
                            setGeometry(m_oldPosX, deltaY, m_oldWidth + deltaWidth, m_oldHeight - deltaHeight);
                            break;
                        case Resize::RIGHT_BOTTOM:
                            setGeometry(m_oldPosX, m_oldPosY, m_oldWidth + deltaWidth, m_oldHeight + deltaHeight);
                            break;
                        case Resize::BOTTOM:
                            setGeometry(m_oldPosX, m_oldPosY, m_oldWidth, m_oldHeight + deltaHeight);
                            break;
                        }
                        return true;
                    }
                }
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void BaseWindow::onExpandWindow()
{
    isMaximized() ? showNormal() : showMaximized();
}
