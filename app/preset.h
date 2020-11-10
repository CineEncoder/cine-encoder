#ifndef PRESET_H
#define PRESET_H

#include <QDialog>


namespace Ui {
class Preset;
}


class Preset : public QDialog
{
    Q_OBJECT

public:

    explicit Preset(QWidget *parent = nullptr);
    ~Preset();



private slots:

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

private:

    Ui::Preset *ui_preset;
    int _repeat;
};

#endif // PRESET_H
