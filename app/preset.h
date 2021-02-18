#ifndef PRESET_H
#define PRESET_H

#include <QDialog>
#include <QTimer>
#include <iostream>
#include <QMouseEvent>

extern QString _cur_param[23];

namespace Ui
{
class Preset;
}


class Preset : public QDialog
{
    Q_OBJECT

public:

    explicit Preset(QWidget *parent = nullptr);

    ~Preset();

    void setParameters();

private slots:

    bool eventFilter(QObject *watched, QEvent *event);

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_checkBox_2_stateChanged(int arg1);

    void on_comboBox_codec_currentTextChanged(const QString &arg1);

    void on_comboBox_mode_currentTextChanged(const QString &arg1);

    void on_comboBox_audio_codec_currentTextChanged(const QString &arg1);

    void on_comboBox_master_disp_currentTextChanged(const QString &arg1);

    void change_preset_name();

    void on_comboBox_width_editTextChanged();

    void on_comboBox_height_editTextChanged();

    void on_comboBox_bitrate_currentTextChanged();

    void on_comboBox_bitrate_editTextChanged();

    void on_comboBox_pass_currentIndexChanged(int index);

    void repeat_handler();

    void on_comboBox_audio_bitrate_currentTextChanged();

    void on_comboBox_container_currentTextChanged();

    void on_comboBox_preset_currentIndexChanged(int index);

    void on_closeWindow_2_clicked();

    void on_expandWindow_2_clicked();

    void on_hideWindow_2_clicked();

private:

    Ui::Preset *ui_preset;

    int _repeat;

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
};

#endif // PRESET_H
