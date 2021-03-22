#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGlobal>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QCloseEvent>
#include <QShowEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeDatabase>
#include <QMimeData>
#include <QTableWidgetItem>
#include <QUrl>
#include <QList>
#include <QMenu>
#include <QProcess>
#include <QTimer>
#include <QFileDialog>
#include <QPixmap>
#include <QHBoxLayout>
#include <QLabel>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <math.h>
#include <QMovie>
//#include <QDebug>

extern QString _cur_param[23];
extern QVector <QVector <QString> > _preset_table;
enum columnIndex {
    FILENAME,       FORMAT,         RESOLUTION,     DURATION,           FPS,
    AR,             STATUS,         BITRATE,        SUBSAMPLING,        BITDEPTH,
    COLORSPACE,     COLORRANGE,     COLORPRIM,      COLORMATRIX,        TRANSFER,
    MAXLUM,         MINLUM,         MAXCLL,         MAXFALL,            MASTERDISPLAY,
    PATH,           T_DUR,          T_CHROMACOORD,  T_WHITECOORD,       T_STREAMSIZE,
    T_WIDTH,        T_HEIGHT,       T_VIDEOTITLE,   T_VIDEOMOVIENAME,   T_VIDEOYEAR,
    T_VIDEOAUTHOR,  T_VIDEOPERF,    T_VIDEODESCR,   T_AUDIO_1,          T_AUDIO_2,
    T_AUDIO_3,      T_AUDIO_4,      T_AUDIO_5,      T_AUDIO_6,          T_AUDIO_7,
    T_AUDIO_8,      T_AUDIO_9,      T_AUDIOLANG_1,  T_AUDIOLANG_2,      T_AUDIOLANG_3,
    T_AUDIOLANG_4,  T_AUDIOLANG_5,  T_AUDIOLANG_6,  T_AUDIOLANG_7,      T_AUDIOLANG_8,
    T_AUDIOLANG_9,  T_AUDIOTITLE_1, T_AUDIOTITLE_2, T_AUDIOTITLE_3,     T_AUDIOTITLE_4,
    T_AUDIOTITLE_5, T_AUDIOTITLE_6, T_AUDIOTITLE_7, T_AUDIOTITLE_8,     T_AUDIOTITLE_9,
    T_AUDIOCHECK_1, T_AUDIOCHECK_2, T_AUDIOCHECK_3, T_AUDIOCHECK_4,     T_AUDIOCHECK_5,
    T_AUDIOCHECK_6, T_AUDIOCHECK_7, T_AUDIOCHECK_8, T_AUDIOCHECK_9,     T_SUBTITLE_1,
    T_SUBTITLE_2,   T_SUBTITLE_3,   T_SUBTITLE_4,   T_SUBTITLE_5,       T_SUBTITLE_6,
    T_SUBTITLE_7,   T_SUBTITLE_8,   T_SUBTITLE_9,   T_SUBLANG_1,        T_SUBLANG_2,
    T_SUBLANG_3,    T_SUBLANG_4,    T_SUBLANG_5,    T_SUBLANG_6,        T_SUBLANG_7,
    T_SUBLANG_8,    T_SUBLANG_9,    T_TITLESUB_1,   T_TITLESUB_2,       T_TITLESUB_3,
    T_TITLESUB_4,   T_TITLESUB_5,   T_TITLESUB_6,   T_TITLESUB_7,       T_TITLESUB_8,
    T_TITLESUB_9,   T_SUBCHECK_1,   T_SUBCHECK_2,   T_SUBCHECK_3,       T_SUBCHECK_4,
    T_SUBCHECK_5,   T_SUBCHECK_6,   T_SUBCHECK_7,   T_SUBCHECK_8,       T_SUBCHECK_9,
    T_STARTTIME,    T_ENDTIME
};
enum curParamIndex {
    PRESET_NAME,    CODEC,          MODE,
    CONTAINER,      BQR,            MAXRATE,
    BUFSIZE,        RESIZE_ENABLED, RESIZE_CHECKSTATE,
    WIDTH,          HEIGHT,         PASS,
    PRESET,         COLOR_RANGE,    MIN_LUM,
    MAX_LUM,        MAX_CLL,        MAX_FALL,
    MASTER_DISPLAY, CHROMA_COORD,   WHITE_COORD,
    AUDIO_CODEC,    AUDIO_BITRATE
};
enum curHDRIndex {
    CUR_COLOR_RANGE,    CUR_COLOR_PRIMARY,  CUR_COLOR_MATRIX,
    CUR_TRANSFER,       CUR_MAX_LUM,        CUR_MIN_LUM,
    CUR_MAX_CLL,        CUR_MAX_FALL,       CUR_MASTER_DISPLAY,
    CUR_CHROMA_COORD,   CUR_WHITE_COORD
};
enum videoMetadataIndex {
    VIDEO_TITLE,        VIDEO_AUTHOR,       VIDEO_YEAR,
    VIDEO_PERFORMER,    VIDEO_DESCRIPTION,  VIDEO_MOVIENAME
};

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    // ******************** Settings *************************//

    QFile _stn_file;

    QString _output_folder;

    QString _temp_folder;

    int _pos_top;

    int _pos_cld;

    int _timer_interval;

    bool _batch_mode;

    bool _showHDR_mode;

    bool _protection;

private slots:

    void setParameters();

    void dragEnterEvent(QDragEnterEvent* event);

    void dragMoveEvent(QDragMoveEvent* event);

    void dragLeaveEvent(QDragLeaveEvent* event);

    void dropEvent(QDropEvent* event);

    void showEvent(QShowEvent *event);

    bool eventFilter(QObject *watched, QEvent *event);

    void closeEvent(QCloseEvent *event);

    void on_actionAbout_clicked();

    void on_actionSettings_clicked();

    void on_actionAdd_clicked();

    void on_actionRemove_clicked();

    void on_actionPreset_clicked();

    void on_actionEncode_clicked();

    void on_actionStop_clicked();

    void repeat_handler();

    void on_tableWidget_itemSelectionChanged();

    void make_preset();

    void pause();

    void resume();

    void add_metadata();

    void complete();

    void progress_1();

    void progress_2();

    void cancel();

    void error_1();

    void encode();

    void on_actionDonate_clicked();

    void on_closeWindow_clicked();

    void on_hideWindow_clicked();

    void on_expandWindow_clicked();

    void on_buttonSortDown_clicked();

    void on_buttonSortUp_clicked();

    void on_buttonRightWindow_clicked();

    void on_buttonLeftWindow_clicked();

    void on_buttonCloseRightWindow_clicked();

    void on_buttonCloseLeftWindow_clicked();

    void on_lineEditTitleVideo_editingFinished();

    void on_lineEditAuthorVideo_editingFinished();

    void on_lineEditYearVideo_editingFinished();

    void on_lineEditPerfVideo_editingFinished();

    void on_lineEditMovieNameVideo_editingFinished();

    void on_lineEditDescriptionVideo_editingFinished();

    void on_checkBoxAudio_1_clicked();

    void on_checkBoxAudio_2_clicked();

    void on_checkBoxAudio_3_clicked();

    void on_checkBoxAudio_4_clicked();

    void on_checkBoxAudio_5_clicked();

    void on_checkBoxAudio_6_clicked();

    void on_checkBoxAudio_7_clicked();

    void on_checkBoxAudio_8_clicked();

    void on_checkBoxAudio_9_clicked();

    void on_lineEditLangAudio_1_editingFinished();

    void on_lineEditLangAudio_2_editingFinished();

    void on_lineEditLangAudio_3_editingFinished();

    void on_lineEditLangAudio_4_editingFinished();

    void on_lineEditLangAudio_5_editingFinished();

    void on_lineEditLangAudio_6_editingFinished();

    void on_lineEditLangAudio_7_editingFinished();

    void on_lineEditLangAudio_8_editingFinished();

    void on_lineEditLangAudio_9_editingFinished();

    void on_lineEditTitleAudio_1_editingFinished();

    void on_lineEditTitleAudio_2_editingFinished();

    void on_lineEditTitleAudio_3_editingFinished();

    void on_lineEditTitleAudio_4_editingFinished();

    void on_lineEditTitleAudio_5_editingFinished();

    void on_lineEditTitleAudio_6_editingFinished();

    void on_lineEditTitleAudio_7_editingFinished();

    void on_lineEditTitleAudio_8_editingFinished();

    void on_lineEditTitleAudio_9_editingFinished();

    void on_checkBoxSubtitle_1_clicked();

    void on_checkBoxSubtitle_2_clicked();

    void on_checkBoxSubtitle_3_clicked();

    void on_checkBoxSubtitle_4_clicked();

    void on_checkBoxSubtitle_5_clicked();

    void on_checkBoxSubtitle_6_clicked();

    void on_checkBoxSubtitle_7_clicked();

    void on_checkBoxSubtitle_8_clicked();

    void on_checkBoxSubtitle_9_clicked();

    void on_lineEditLangSubtitle_1_editingFinished();

    void on_lineEditLangSubtitle_2_editingFinished();

    void on_lineEditLangSubtitle_3_editingFinished();

    void on_lineEditLangSubtitle_4_editingFinished();

    void on_lineEditLangSubtitle_5_editingFinished();

    void on_lineEditLangSubtitle_6_editingFinished();

    void on_lineEditLangSubtitle_7_editingFinished();

    void on_lineEditLangSubtitle_8_editingFinished();

    void on_lineEditLangSubtitle_9_editingFinished();

    void on_lineEditTitleSubtitle_1_editingFinished();

    void on_lineEditTitleSubtitle_2_editingFinished();

    void on_lineEditTitleSubtitle_3_editingFinished();

    void on_lineEditTitleSubtitle_4_editingFinished();

    void on_lineEditTitleSubtitle_5_editingFinished();

    void on_lineEditTitleSubtitle_6_editingFinished();

    void on_lineEditTitleSubtitle_7_editingFinished();

    void on_lineEditTitleSubtitle_8_editingFinished();

    void on_lineEditTitleSubtitle_9_editingFinished();

    void on_horizontalSlider_valueChanged(int value);

    void on_buttonFramePrevious_clicked();

    void on_buttonFrameNext_clicked();

    void on_buttonSetStartTime_clicked();

    void on_buttonSetEndTime_clicked();

    void showMetadataEditor();

    void showAudioStreamsSelection();

    void showSubtitlesSelection();

    void showSplitVideo();

    void repeatHandler_Type_2();

private:

    Ui::MainWindow *ui;

    // ******************* Constants *****************************//

    static const int AMOUNT_AUDIO_STREAMS = 9;

    static const int AMOUNT_SUBTITLES = 9;

    static const int NUMBER_PRESETS = 23;

    static constexpr float MAXIMUM_ALLOWED_TIME = 359999.0f;

    // ******************** Top label ****************************//

    QHBoxLayout *raiseLayout;

    QLabel *raiseThumb;

    // **************** Progress animation ***********************//

    QMovie *animation;

    // ***************** Top menu actions ************************//

    QAction *add_files;

    QAction *remove_files;

    QAction *close_prog;


    QAction *select_preset;

    QAction *encode_files;

    QAction *stop_encode;


    QAction *edit_metadata;

    QAction *select_audio;

    QAction *select_subtitles;

    QAction *split_video;


    QAction *settings;


    QAction *about;

    QAction *donate;


    QMenu* menuFiles;

    QMenu* menuEdit;

    QMenu* menuTools;

    QMenu* menuPreferences;

    QMenu* menuAbout;

    // ***************** Table menu actions ************************//

    QMenu* itemMenu;

    // ********************** Processes ****************************//

    QProcess *process_1;

    QProcess *process_5;

    // *********************** Timers ******************************//

    QTimer *timer;

    QTimer *timerCallSetThumbnail;

    // ******************** Initialization *************************//

    QFile _prs_file;

    QFile _wind_file;

    QString _openDir;

    int _theme;

    // ************************** Metadata ************************//

    QString _hdr[11];

    QString _videoMetadata[6];

    int _audioStreamCheckState[AMOUNT_AUDIO_STREAMS];

    QString _audioLang[AMOUNT_AUDIO_STREAMS];

    QString _audioTitle[AMOUNT_AUDIO_STREAMS];

    int _subtitleCheckState[AMOUNT_SUBTITLES];

    QString _subtitleLang[AMOUNT_SUBTITLES];

    QString _subtitleTitle[AMOUNT_SUBTITLES];

    // ************************* Encoding ***************************//

    QString _message;

    QString _error_message;


    QString _curFilename;

    QString _curPath;

    QString _temp_file;

    QString _input_file;

    QString _output_file;

    QString _settings_path;

    QString _thumb_path;

    QString _settings_file;

    QString _preset_file;

    QString _window_file;


    QString _preset_0;

    QString _preset_pass1;

    QString _preset;

    QString _preset_mkvmerge;


    QString _fmt;

    QString _width;

    QString _height;

    QString _fps;

    QString _stream_size;

    double _dur;

    int _fr_count;


    time_t _loop_start;

    time_t _strt_t;

    double _curTime;

    double _startTime;

    double _endTime;


    int _row;

    QString _status_encode_btn;

    bool _flag_two_pass;

    bool _flag_hdr;

    bool _calling_pr_1;

    bool _mux_mode;

    // *********************** Geometry ***********************************//

    bool _windowActivated = false;

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

    // ****************************************************************************//

    void openFiles(const QStringList &file_name_open);

    void get_current_data();

    void get_output_filename();

    void restore_initial_state();

    void set_defaults();

    void call_task_complete(const QString &_message, const bool &_timer_mode);

    bool call_dialog(const QString &_message);

    void setTheme(int &ind_theme);

    QString styleCreator(const QString &list);

    QString timeConverter(double &time);

    QString timeConverter(float &time);

    void setThumbnail(QString curFilename, double time, QString quality);

    void provideContextMenu(const QPoint &position);
};

#endif // MAINWINDOW_H
