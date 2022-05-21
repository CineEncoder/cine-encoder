/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: basewindow.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "basewindow.h"
#include <QEventLoop>
#include <QTimer>
#include <QEvent>
#include <QLabel>
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#define BORDER 0
#define RESIZABLE_AREA 4

BaseWindow::BaseWindow(QWidget *parent, bool isReizable) :
    QMainWindow(parent, Qt::Window | Qt::FramelessWindowHint |
                Qt::WindowSystemMenuHint | Qt::WindowMaximizeButtonHint),
    m_titlebar(nullptr),
    m_whiteList(QList<QWidget*>()),
    m_isResizeable(isReizable),
    m_windowActivated(false)
{
    setAttribute(Qt::WA_ShowModal);
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
    auto label = titlebar->findChild<QLabel*>("label_title");
    if (label)
        addIgnoreWidget(label);
}

void BaseWindow::addIgnoreWidget(QWidget* widget)
{
    if (!widget) return;
    if (m_whiteList.contains(widget)) return;
    m_whiteList.append(widget);
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
        setContentsMargins(BORDER, BORDER + 1, BORDER, BORDER);
    }
}

void BaseWindow::changeEvent(QEvent *event)
{
    QMainWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        const int margin = isMaximized() ? 8 : BORDER;
        setContentsMargins(margin, margin, margin, margin);
        QTimer::singleShot(50, this, [this]() {
            update();
        });
    }
}

bool BaseWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#if (QT_VERSION == QT_VERSION_CHECK(5, 11, 1))
    MSG* msg = *reinterpret_cast<MSG**>(message);
#else
    MSG* msg = reinterpret_cast<MSG*>(message);
#endif

    switch (msg->message)
    {
    case WM_ACTIVATE: {
        HWND hwnd = (HWND)winId();
        DWORD style = ::GetWindowLong(hwnd, GWL_STYLE);
        style = m_isResizeable ? style | WS_MAXIMIZEBOX : style & ~WS_MAXIMIZEBOX;
        ::SetWindowLong(hwnd, GWL_STYLE, style | WS_THICKFRAME | WS_CAPTION);
        const MARGINS mrg = {1, 1, 1, 1};
        DwmExtendFrameIntoClientArea(hwnd, &mrg);
        break;
    }

    case WM_SYSCOMMAND: {
        if (GET_SC_WPARAM(msg->wParam) == SC_RESTORE) {
            return false;
        } else
        if (GET_SC_WPARAM(msg->wParam) == SC_MINIMIZE) {
            return false;
        }
        break;
    }

    case WM_NCCALCSIZE: {
        NCCALCSIZE_PARAMS& params = *reinterpret_cast<NCCALCSIZE_PARAMS*>(msg->lParam);
        if (params.rgrc[0].top != 0) params.rgrc[0].top -= 1;
        *result = WVR_REDRAW;
        return true;
    }

    case WM_NCHITTEST: {
        *result = 0;
        RECT rect;
        GetWindowRect(HWND(winId()), &rect);
        long x = GET_X_LPARAM(msg->lParam);
        long y = GET_Y_LPARAM(msg->lParam);
        if (m_isResizeable) {
            bool resizeWidth = minimumWidth() != maximumWidth();
            bool resizeHeight = minimumHeight() != maximumHeight();

            if (resizeWidth) {
                if (x >= rect.left && x < rect.left + RESIZABLE_AREA) {
                    *result = HTLEFT;
                }
                if (x < rect.right && x >= rect.right - RESIZABLE_AREA) {
                    *result = HTRIGHT;
                }
            }
            if (resizeHeight) {
                if (y < rect.bottom && y >= rect.bottom - RESIZABLE_AREA) {
                    *result = HTBOTTOM;
                }
                if (y >= rect.top && y < rect.top + RESIZABLE_AREA) {
                    *result = HTTOP;
                }
            }
            if (resizeWidth && resizeHeight) {
                if (x >= rect.left && x < rect.left + RESIZABLE_AREA &&
                        y < rect.bottom && y >= rect.bottom - RESIZABLE_AREA) {
                    *result = HTBOTTOMLEFT;
                }
                if (x < rect.right && x >= rect.right - RESIZABLE_AREA &&
                        y < rect.bottom && y >= rect.bottom - RESIZABLE_AREA) {
                    *result = HTBOTTOMRIGHT;
                }
                if (x >= rect.left && x < rect.left + RESIZABLE_AREA &&
                        y >= rect.top && y < rect.top + RESIZABLE_AREA) {
                    *result = HTTOPLEFT;
                }
                if (x < rect.right && x >= rect.right - RESIZABLE_AREA &&
                        y >= rect.top && y < rect.top + RESIZABLE_AREA) {
                    *result = HTTOPRIGHT;
                }
            }
        }
        if (*result != 0)
            return true;
        if (!m_titlebar)
            return false;
        const double dpr = devicePixelRatioF();
        QPoint pos = m_titlebar->mapFromGlobal(QPoint(int(x/dpr), int(y/dpr)));
        if (!m_titlebar->rect().contains(pos))
            return false;
        QWidget* child = m_titlebar->childAt(pos);
        if (!child) {
            *result = HTCAPTION;
            return true;
        } else {
            if (m_whiteList.contains(child)) {
                *result = HTCAPTION;
                return true;
            }
        }
        return false;
    }

    default:
        break;
    }
    return QMainWindow::nativeEvent(eventType, message, result);
}
