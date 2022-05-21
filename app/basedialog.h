/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: basedialog.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef BASEDIALOG_H
#define BASEDIALOG_H

#include <QDialog>

class BaseDialog : public QDialog
{
    Q_OBJECT
public:
    explicit BaseDialog(QWidget *parent = nullptr, bool isReizable = true);
    virtual ~BaseDialog();

protected:
    void setTitleBar(QWidget*);
    void setMaskWidget(QWidget*);
    virtual void showEvent(QShowEvent*) override;
    virtual void changeEvent(QEvent*) override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void onExpandWindow();

private:
    enum Resize {
        LEFT,   LEFT_TOP,   LEFT_BOTTOM,   TOP,
        RIGHT,  RIGHT_TOP,  RIGHT_BOTTOM,  BOTTOM
    };

    QWidget *m_titlebar,
            *m_maskwidget;

    QPoint   m_mouseClickCoordinate,
             m_globalMouseClickCoordinate;

    bool     m_windowActivated,
             m_clickPressedFlag,
             m_isResizable;

    int      m_oldPosX,
             m_oldPosY,
             m_oldWidth,
             m_oldHeight;

    QVector<bool> m_clickPressedToResizeFlag;
};

#endif // BASEDIALOG_H
