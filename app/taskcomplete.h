#ifndef TASKCOMPLETE_H
#define TASKCOMPLETE_H

#include <QDialog>
#include <QTimer>


namespace Ui {
class Taskcomplete;
}



class Taskcomplete : public QDialog
{
    Q_OBJECT


public:

    explicit Taskcomplete(QWidget *parent = nullptr);
    ~Taskcomplete();



    void set_message(QString _message, bool timer_mode);



private slots:

    void on_pushButton_4_clicked();

    void repeat_handler();

private:

    Ui::Taskcomplete *ui_taskcomplete;
    QString _message;
    QTimer *timer = new QTimer(this);
    int elps_t;
    bool mode;

    void show_message(QString _message);
};

#endif // TASKCOMPLETE_H
