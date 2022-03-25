/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: preset.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef PRESET_H
#define PRESET_H

#include <QDialog>
#include <QTimer>
#include <QListView>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QCloseEvent>
#include <QResizeEvent>
#include <iostream>
#include <math.h>
#include "constants.h"


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

    void setParameters(QByteArray *ptr_presetWindowGeometry, QVector<QString> *_old_param);

private slots:

    void closeEvent(QCloseEvent *close_preset);
    bool eventFilter(QObject *watched, QEvent *event);
    void on_closeWindow_clicked();
    void on_buttonCancel_clicked();
    void on_buttonApply_clicked();
    void on_buttonTab_1_clicked();
    void on_buttonTab_2_clicked();
    void on_buttonTab_3_clicked();
    void on_buttonTab_4_clicked();
    void change_preset_name();
    void on_comboBoxAspectRatio_currentIndexChanged(int index);
    void on_comboBox_width_currentTextChanged(const QString &arg1);
    void on_comboBox_height_currentTextChanged(const QString &arg1);
    void calculateDAR(QString width, QString height);
    void on_comboBoxFrameRate_currentIndexChanged(int index);
    void repeat_handler();
    void disableHDR();
    void on_comboBox_codec_currentTextChanged(const QString &arg1);
    void on_comboBox_mode_currentTextChanged(const QString &arg1);
    void on_comboBox_preset_currentIndexChanged(int index);
    void on_comboBox_pass_currentIndexChanged(int index);
    void on_comboBox_container_currentTextChanged();
    void on_lineEdit_bitrate_editingFinished();
    void on_comboBox_audio_codec_currentTextChanged(const QString &arg1);
    void on_comboBox_audio_bitrate_currentTextChanged();
    void on_comboBox_master_disp_currentTextChanged(const QString &arg1);

private:

    Ui::Preset *ui;

    QVector<QString> *_new_param;

    QTimer *timer;

    int _repeat;

    float _aspectRatio;

    /**************** Geometry **************************/

    bool        _expandWindowsState,
                _clickPressedFlag;

    int         _oldPosX,
                _oldPosY,
                _oldWidth,
                _oldHeight;

    QPoint      _mouseClickCoordinate,
                _globalMouseClickCoordinate;

    QByteArray  *_ptr_presetWindowGeometry;

    QVector<bool> _clickPressedToResizeFlag;

    void on_expandWindow_clicked();
    void lockSignals(bool status);

};

#endif // PRESET_H
