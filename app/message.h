/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: message.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef MESSAGE_H
#define MESSAGE_H

#include <QTimer>
#ifdef WM_CUSTOM
    #include "basewindow_cwm.h"
#else
    #ifdef Q_OS_WIN
        #include "platform_win/basewindow.h"
    #else
        #include "platform_unix/basewindow.h"
    #endif
#endif


enum class MessType : uint8_t {
    DIALOG, INFO
};

namespace Ui
{
    class Message;
}

class Message : public BaseWindow
{
    Q_OBJECT
public:
    enum class Icon : uchar {
        Info, Warning, Critical
    };
    explicit Message(QWidget *parent, MessType mess_type,
                     const QString &message, const bool timer_flag = false);
    ~Message(); 

private slots:
    void onButtonApply();
    void onCloseWindow();
    virtual void showEvent(QShowEvent*) final;

private:
    void setMessage();
    void show_message();
    void repeatHandler();
    virtual void onExpandWindow() final {};

    Ui::Message *ui;
    QTimer      *m_timer;
    QString  m_message;
    MessType m_mess_type;
    int      m_elps_t;
    bool     m_windowActivated,
             m_timer_flag;
};

#endif // MESSAGE_H
