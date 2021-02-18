#ifndef TASKCOMPLETE_H
#define TASKCOMPLETE_H

#include <QDialog>
#include <QTimer>
#include <QSound>
#include <QMouseEvent>
#include <math.h>
#include <iomanip>
#include <sstream>
//#include <iostream>


namespace Ui
{
    class Taskcomplete;
}


class Taskcomplete : public QDialog
{
    Q_OBJECT

public:

    explicit Taskcomplete(QWidget *parent = nullptr);

    ~Taskcomplete();

    void setMessage(const QString &_message, const bool &_timer_mode);

private slots:

    bool eventFilter(QObject *watched, QEvent *event);

    void on_pushButton_4_clicked();

    void on_closeWindow_clicked();

    void repeat_handler();

private:

    Ui::Taskcomplete *ui_taskcomplete;

    QTimer *timer = new QTimer(this);

    QString _message;

    int elps_t;

    bool clickPressedFlag = false;

    QPoint mouseClickCoordinate;

    void show_message(QString _message);
};

#endif // TASKCOMPLETE_H
