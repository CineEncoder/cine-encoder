#ifndef PRESET_H
#define PRESET_H

#include <QDialog>
#include <QTimer>
#include <QListView>
#include <iostream>
#include <math.h>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QCloseEvent>

enum Profile {
    HIGH,           MAIN,           MAIN10,
    PROFILE_0,      PROFILE_1,      PROFILE_2,
    PROFILE_3,      PROFILE_4,      PROFILE_5,
    DNXHR_LB,       DNXHR_SQ,       DNXHR_HQ,
    DNXHR_HQX,      DNXHR_444,      PROFILE_AUTO
};
enum Pixformat {
    YUV444P10LE,    YUV422P10LE,    YUV420P10LE,
    YUV422p,        YUV420P,        P010LE,
    PIXFORMAT_AUTO
};
enum Parameters {
    _OUTPUT_PARAM,   _CODEC,          _MODE,
    _CONTAINER,      _BQR,            _MAXRATE,
    _BUFSIZE,        _FRAME_RATE,     _BLENDING,
    _WIDTH,          _HEIGHT,         _PASS,
    _PRESET,         _COLOR_RANGE,    _MIN_LUM,
    _MAX_LUM,        _MAX_CLL,        _MAX_FALL,
    _MASTER_DISPLAY, _CHROMA_COORD,   _WHITE_COORD,
    _AUDIO_CODEC,    _AUDIO_BITRATE,  _MINRATE,
    _LEVEL,          _ASAMPLE_RATE,   _ACHANNELS,
    _MATRIX,         _PRIMARY,        _TRC,
    _PRESET_NAME,    _REP_PRIM,       _REP_MATRIX,
    _REP_TRC
};

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

    void setParameters(QByteArray *ptr_presetWindowGeometry, QString *_old_param);

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

    Ui::Preset *ui_preset;

    //static const int PARAMETERS_COUNT = 30;

    static const int NUMBER_PRESETS = 23;

    QString *_new_param;

    QTimer *timer;

    int _repeat;

    float _aspectRatio;

    /**************** Geometry **************************/

    QByteArray *_ptr_presetWindowGeometry;

    int oldWidth;

    int oldHeight;

    int curWidth;

    int curHeight;

    QPoint mouseClickCoordinate;

    QPoint mouseCoordinate;

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

    void on_expandWindow_clicked();

    void lockSignals(bool status);
};

#endif // PRESET_H
