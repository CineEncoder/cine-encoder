#ifndef SELECTPRESET_H
#define SELECTPRESET_H

#include <QDialog>
#include <QFile>
#include <QMouseEvent>
#include <QHoverEvent>
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

    // ***************************** Top menu actions ***********************************//

    QAction *addsection;

    QAction *addpreset;

    QMenu* menu;

    // *********************************************************************************//

    QString _message;

    int *_ptr_pos_top;

    int *_ptr_pos_cld;

    // ******************************** Geometry ***************************************//

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

    QPoint mouseClickCoordinate;

    QPoint mouseCoordinate;

    int oldWidth;

    int oldHeight;

    int curWidth;

    int curHeight;

    // *********************************************************************************//

    bool call_dialog(const QString &_message);

    void call_task_complete(const QString &_message, const bool &timer_mode);
};

#endif // SELECTPRESET_H
