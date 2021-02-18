#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QSound>
#include <QMouseEvent>


namespace Ui
{
    class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:

    explicit Dialog(QWidget *parent = nullptr);

    ~Dialog();

    void setMessage(const QString &_message, bool *_aceptFlag);

private slots:

    bool eventFilter(QObject *watched, QEvent *event);

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_closeWindow_clicked();

private:

    Ui::Dialog *ui_dialog;

    QString _message;

    bool *acceptFlag;

    bool clickPressedFlag = false;

    QPoint mouseClickCoordinate;

    void show_message(QString _message);
};

#endif // DIALOG_H
