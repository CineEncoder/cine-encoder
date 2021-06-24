#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
#include <QMouseEvent>
#include <QDesktopServices>
#include <QUrl>
#include <iostream>

namespace Ui
{
class About;
}


class About : public QDialog
{
    Q_OBJECT

public:

    explicit About(QWidget *parent = nullptr);

    ~About();

    void setParameters();

private slots:

    void on_buttonCancel_clicked();

    void on_closeWindow_clicked();

    bool eventFilter(QObject *watched, QEvent *event);

private:

    Ui::About *ui_about;

    bool clickPressedFlag = false;

    QPoint mouseClickCoordinate;
};

#endif // ABOUT_H
