#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QDialog>
#include <QWidget>
#include <QEvent>

class FramelessWindow: public QDialog
{
    Q_OBJECT
public:
    FramelessWindow(QWidget *parent = nullptr);
    ~FramelessWindow();
protected:
    QWidget *_centralWidget;

private:
    bool event(QEvent *event);

    bool window_activated;

private slots:
    //bool eventFilter(QObject *watched, QEvent *event);
};

#endif // FRAMELESSWINDOW_H
