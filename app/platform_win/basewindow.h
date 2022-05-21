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
    void addIgnoreWidget(QWidget* widget);
    void onExpandWindow();
    virtual void showEvent(QShowEvent*) override;
    virtual void changeEvent(QEvent*) override;

private:
    virtual bool nativeEvent(const QByteArray&, void*, long*) final;

    QWidget* m_titlebar;
    QList<QWidget*>  m_whiteList;
    bool     m_isResizeable,
             m_windowActivated;
};

#endif // BASEWINDOW_H
