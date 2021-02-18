#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QFileDialog>
#include <QCloseEvent>
#include <QMouseEvent>
//#include <iostream>

namespace Ui
{
class Settings;
}


class Settings : public QDialog
{
    Q_OBJECT

public:

    explicit Settings(QWidget *parent = nullptr);

    ~Settings();

    void setParameters(bool *ptr_batch_mode, QFile *ptr_stn_file,
                       QString *ptr_output_folder, QString *ptr_temp_folder,
                       bool *ptr_protection, bool *ptr_showHDR_mode, int *ptr_timer_interval, int *ptr_theme);

private slots:

    bool eventFilter(QObject *watched, QEvent *event);

    void on_pushButton_7_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

    void on_checkBox_1_clicked();

    void on_pushButton_4_clicked();

    void closeEvent(QCloseEvent *event);

    void on_checkBox_3_clicked();

    void on_closeWindow_clicked();

    void on_expandWindow_clicked();

    void on_hideWindow_clicked();

    void on_buttonReset_clicked();

    void on_checkBox_2_clicked();

private:

    Ui::Settings *ui_settings;

    QString _message;

    QString _curr_output_folder;

    QString _curr_temp_folder;

    int _curr_theme;

    int _curr_timer_interval;

    bool _curr_batch_mode;

    bool _curr_showHDR_mode;

    bool _curr_protection;

    bool _flag_save;

    QFile *_ptr_stn_file;

    QString *_ptr_output_folder;

    QString *_ptr_temp_folder;

    int *_ptr_timer_interval;

    int *_ptr_theme;

    bool *_ptr_batch_mode;

    bool *_ptr_showHDR_mode;

    bool *_ptr_protection;

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

    void call_task_complete(const QString &_message, const bool &timer_mode);
};

#endif // SETTINGS_H
