/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: basewindow.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef BASEWINDOW_H
#define BASEWINDOW_H

#include <QMainWindow>
#include "xutil.h"


class BaseWindow: public QMainWindow
{
    Q_OBJECT
public:
    explicit BaseWindow(QWidget *parent = nullptr, bool isReizable = true);
    virtual ~BaseWindow();
    int exec();
    void acceptDialog();
    void closeDialog();
    enum Dialog {
        Decline, Accept
    };

signals:
    void exitLoop(int);

protected:
    void setTitleBar(QWidget*);
    void setMaskWidget(QWidget*);
    void onExpandWindow();
    virtual void showEvent(QShowEvent*) override;
    virtual void mouseMoveEvent(QMouseEvent *) override;
    virtual void mousePressEvent(QMouseEvent *) override;
    virtual void mouseReleaseEvent(QMouseEvent *) override;
    virtual void resizeEvent(QResizeEvent *) override;
    virtual void changeEvent(QEvent*) override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    XUtils::CornerEdge m_resizingCornerEdge;
    QWidget *m_titlebar,
            *m_maskwidget;
    bool     m_windowActivated,
             m_isResizable;
};

#endif // BASEWINDOW_H



