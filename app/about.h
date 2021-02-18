#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>
//#include <QShowEvent>
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

    bool eventFilter(QObject *watched, QEvent *event);

    void on_toolButton_6_clicked();

    void on_closeWindow_clicked();

private:

    Ui::About *ui_about;

    bool clickPressedFlag = false;

    QPoint mouseClickCoordinate;
};

#endif // ABOUT_H
