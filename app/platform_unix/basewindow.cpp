/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: basewindow.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "platform_unix/basewindow.h"
#include <QApplication>
#include <QMouseEvent>
#include <QEventLoop>
#include <QTimer>
#include <QGraphicsDropShadowEffect>

#define BORDER 15


BaseWindow::BaseWindow(QWidget *parent, bool isReizable) :
    QMainWindow(parent, Qt::Window | Qt::FramelessWindowHint),
    m_titlebar(nullptr),
    m_maskwidget(nullptr),
    m_windowActivated(false),
    m_isResizable(isReizable)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_ShowModal);
    QWidget *ui_widget = new QWidget(this);
    setCentralWidget(ui_widget);
    setMaskWidget(ui_widget);
    m_resizingCornerEdge = XUtils::CornerEdge::kInvalid;
    installEventFilter(this);
    setMouseTracking(true);
    XUtils::SetMouseTransparent(this, true);
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

void BaseWindow::onExpandWindow()
{
    isMaximized() ? showNormal() : showMaximized();
}

void BaseWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);
    if (!m_windowActivated) {
        m_windowActivated = true;
        setContentsMargins(BORDER, BORDER, BORDER, BORDER);
    }
}

void BaseWindow::mouseMoveEvent(QMouseEvent *event)
{
    const int x = event->x();
    const int y = event->y();
    if (m_resizingCornerEdge == XUtils::CornerEdge::kInvalid && m_isResizable) {
        XUtils::UpdateCursorShape(this, x, y, this->contentsMargins(), BORDER);
    }
    return QMainWindow::mouseMoveEvent(event);
}

void BaseWindow::mousePressEvent(QMouseEvent *event)
{
    const int x = event->x();
    const int y = event->y();
    if (event->button() == Qt::LeftButton && m_isResizable) {
        const XUtils::CornerEdge ce = XUtils::GetCornerEdge(this, x, y,
                                                            this->contentsMargins(),
                                                            BORDER);
        if (ce != XUtils::CornerEdge::kInvalid) {
            m_resizingCornerEdge = ce;
            XUtils::SendButtonRelease(this, event->pos(), event->globalPos());
            XUtils::StartResizing(this, QCursor::pos(), ce);
        }
    }
    return QMainWindow::mousePressEvent(event);
}

void BaseWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_resizingCornerEdge = XUtils::CornerEdge::kInvalid;
    return QMainWindow::mouseReleaseEvent(event);
}

void BaseWindow::resizeEvent(QResizeEvent *event)
{
    int resizeHandleWidth = BORDER;
    XUtils::SetWindowExtents(this, this->contentsMargins(), resizeHandleWidth);
    return QMainWindow::resizeEvent(event);
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
    if (watched == m_maskwidget) {
        if (event->type() == QEvent::HoverEnter) {
            QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
            QApplication::restoreOverrideCursor();
            return true;
        }
    } else
    if (watched == m_titlebar) {
        if ((event->type() == QEvent::MouseButtonPress)) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton) {
                XUtils::SendButtonRelease(this, mouse_event->pos(), mouse_event->globalPos());
                XUtils::MoveWindow(this, Qt::LeftButton);
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
    }
    return QMainWindow::eventFilter(watched, event);
}
