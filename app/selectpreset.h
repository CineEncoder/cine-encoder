#ifndef SELECTPRESET_H
#define SELECTPRESET_H

#include <QDialog>
#include <QFile>
#include <QMouseEvent>
#include <QtGlobal>
#include <QMenu>
#include <QMessageBox>
#include <iostream>

extern QVector <QVector <QString> > _preset_table;
extern QString _cur_param[23];

namespace Ui
{
class SelectPreset;
}


class SelectPreset : public QDialog
{
    Q_OBJECT

public:

    explicit SelectPreset(QWidget *parent = nullptr);

    ~SelectPreset();

    void setParameters(int *ptr_pos_top, int *ptr_pos_cld);

private slots:

    bool eventFilter(QObject *watched, QEvent *event);

    void on_pushButton_7_clicked();

    void on_pushButton_6_clicked();

    void on_actionRemove_preset_clicked();

    void on_actionEdit_preset_clicked();

    void add_section();

    void add_preset();

    void on_closeWindow_2_clicked();

    void on_expandWindow_2_clicked();

    void on_hideWindow_2_clicked();

private:

    Ui::SelectPreset *ui_selectpreset;

    QString _message;

    int *_ptr_pos_top;

    int *_ptr_pos_cld;

    bool _expandWindowsState = false;

    bool clickPressedFlag = false;

    bool clickPressed_Left_ResizeFlag = false;

    bool clickPressed_Left_Top_ResizeFlag = false;

    bool clickPressed_Top_ResizeFlag = false;

    bool clickPressed_Right_Top_ResizeFlag = false;

    bool clickPressed_Right_ResizeFlag = false;

    bool clickPressed_Right_Bottom_ResizeFlag = false;

    bool clickPressed_Bottom_ResizeFlag = false;

    bool clickPressed_Left_Bottom_ResizeFlag = false;

    int _posX;

    int _posY;

    int _width;

    int _height;

    QPoint mouseClickCoordinate;

    bool call_dialog(const QString &_message);

    void call_task_complete(const QString &_message, const bool &timer_mode);
};

#endif // SELECTPRESET_H
