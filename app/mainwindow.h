#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtGlobal>
#include <QDesktopWidget>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QShowEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeDatabase>
#include <QMimeData>
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

    QString _output_folder, _temp_folder;

    int _pos_top, _pos_cld;

    bool _batch_mode;

    bool _showHDR_mode;

    bool _protection;

    int _timer_interval;

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

    static const int AMOUNT_AUDIO_SRTEAMS = 9;

    static const int AMOUNT_SUBTITLES = 9;

    const float MAXIMUM_ALLOWED_TIME = 359999.0f;

    // ******************** Top label ****************************//

    QHBoxLayout *raiseLayout = new QHBoxLayout();

    QLabel *raiseThumb = new QLabel(this);

    // **************** Progress animation ***********************//

    QMovie *animation = new QMovie(this);

    // ***************** Top menu actions ************************//

    QAction *add_files = new QAction(tr("Add files"), this);

    QAction *remove_files = new QAction(tr("Remove files"), this);

    QAction *close_prog = new QAction(tr("Close"), this);


    QAction *select_preset = new QAction(tr("Select preset"), this);

    QAction *encode_files = new QAction(tr("Encode/Pause"), this);

    QAction *stop_encode = new QAction(tr("Stop"), this);


    QAction *edit_metadata = new QAction(tr("Edit metadata"), this);

    QAction *select_audio = new QAction(tr("Select audio streams"), this);

    QAction *select_subtitles = new QAction(tr("Select subtitles"), this);

    QAction *split_video = new QAction(tr("Split video"), this);


    QAction *settings = new QAction(tr("Settings"), this);


    QAction *about = new QAction(tr("About"), this);

    QAction *donate = new QAction(tr("Donate"), this);

    // ********************** Processes ****************************//

    QProcess *process_1 = new QProcess(this);

    QProcess *process_5 = new QProcess(this);

    // *********************** Timers ******************************//

    QTimer *timer = new QTimer(this);

    QTimer *timerCallSetThumbnail = new QTimer(this);

    // ******************** Initialization *************************//

    QFile _prs_file;

    QFile _wind_file;

    QString _openDir;

    int _theme;

    // ************************** Metadata ************************//

    QString _hdr[11];

    QString _videoMetadata[6];

    int _audioStreamCheckState[AMOUNT_AUDIO_SRTEAMS];

    QString _audioLang[AMOUNT_AUDIO_SRTEAMS];

    QString _audioTitle[AMOUNT_AUDIO_SRTEAMS];

    int _subtitleCheckState[AMOUNT_SUBTITLES];

    QString _subtitleLang[AMOUNT_SUBTITLES];

    QString _subtitleTitle[AMOUNT_SUBTITLES];

    // ************************* Encoding ***************************//

    QString _message, _error_message;

    QString _curFilename;

    QString _curPath;

    QString _temp_file, _input_file, _output_file;

    QString _settings_path, _thumb_path, _settings_file, _preset_file, _window_file;

    QString _preset_0, _preset_pass1, _preset, _preset_mkvmerge;

    QString _fmt, _width, _height, _fps, _stream_size;

    QString _status_encode_btn;

    time_t _loop_start;

    time_t _strt_t;

    int _row;

    int _fr_count;

    double _curTime;

    double _startTime;

    double _endTime;

    double _dur;

    bool _flag_two_pass, _flag_hdr;

    bool _calling_pr_1, _mux_mode;

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
};

#endif // MAINWINDOW_H
