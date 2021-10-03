/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: mainwindow.h
 MODIFIED: September, 2021
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QCheckBox>
#include <QMainWindow>
#include <QtGlobal>
#include <QSystemTrayIcon>
#include <QDesktopWidget>
#include <QPaintEvent>
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
#include <QListView>
#include <QUrl>
#include <QList>
#include <QMenu>
#include <QProcess>
#include <QTimer>
#include <QDate>
#include <QFileDialog>
#include <QPixmap>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <math.h>
#include <QMovie>
#include <QDockWidget>
#include <QFile>
#include <QTreeWidgetItem>
#include <QSizePolicy>
#include <QSettings>
#include <QMap>
#include <QTranslator>
#include "openingfiles.h"


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
    OUTPUT_PARAM,   CODEC,          MODE,
    CONTAINER,      BQR,            MAXRATE,
    BUFSIZE,        FRAME_RATE,     BLENDING,
    WIDTH,          HEIGHT,         PASS,
    PRESET,         COLOR_RANGE,    MIN_LUM,
    MAX_LUM,        MAX_CLL,        MAX_FALL,
    MASTER_DISPLAY, CHROMA_COORD,   WHITE_COORD,
    AUDIO_CODEC,    AUDIO_BITRATE,  MINRATE,
    LEVEL,          ASAMPLE_RATE,   ACHANNELS,
    MATRIX,         PRIMARY,        TRC,
    PRESET_NAME,    REP_PRIM,       REP_MATRIX,
    REP_TRC
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
enum dockIndex {
    PRESETS_DOCK, PREVIEW_DOCK, SOURCE_DOCK, OUTPUT_DOCK,
    STREAMS_DOCK, LOG_DOCK, METADATA_DOCK, SPLIT_DOCK
};


QT_BEGIN_NAMESPACE
namespace Ui
{
    class Widget;
}
QT_END_NAMESPACE


class Widget : public QWidget
{
    Q_OBJECT

public:

    Widget(QWidget *parent = nullptr);

    ~Widget();

    static const int PARAMETERS_COUNT = 34;

    // ******************** Settings *************************//

    QString _new_param[PARAMETERS_COUNT];

    QFile _stn_file;

    QByteArray _settingsWindowGeometry;

    QByteArray _presetWindowGeometry;

    bool _hideInTrayFlag;

    QString _desktopEnv;

    QString _language;

    QString _output_folder;

    QString _temp_folder;

    QString _prefixName;

    QString _suffixName;

    QString _font;

    int _fontSize;

    int _prefxType;

    int _suffixType;

    int _pos_top;

    int _pos_cld;

    int _timer_interval;

    bool _batch_mode;

    bool _showHDR_mode;

    bool _protection;

private slots:

    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);

    void setTrayIconActions();

    void showTrayIcon();

    void on_closeWindow_clicked();

    void setExpandIcon();

    void on_hideWindow_clicked();

    void on_expandWindow_clicked();

    void on_actionSettings_clicked();

    void on_actionAdd_clicked();

    void on_actionRemove_clicked();

    void on_actionEncode_clicked();

    void on_actionStop_clicked();

    void on_buttonSortDown_clicked();

    void on_buttonSortUp_clicked();

    void setParameters();

    void setDocksParameters(QList<int> dockSizesX, QList<int> dockSizesY);

    void showEvent(QShowEvent *event);

    void closeEvent(QCloseEvent *event);

    bool eventFilter(QObject *watched, QEvent *event);

    void dragEnterEvent(QDragEnterEvent* event);

    void dragMoveEvent(QDragMoveEvent* event);

    void dragLeaveEvent(QDragLeaveEvent* event);

    void dropEvent(QDropEvent* event);   

    void on_tableWidget_itemSelectionChanged();

    QString callFileDialog(const QString title);

    void make_preset();

    void pause();

    void resume();

    void add_metadata();

    void complete();

    void progress_1();

    void progress_2();

    void cancel();

    void error();

    void encode();

    void on_lineEditTitleVideo_editingFinished();

    void on_lineEditAuthorVideo_editingFinished();

    void on_lineEditYearVideo_editingFinished();

    void on_lineEditPerfVideo_editingFinished();

    void on_lineEditMovieNameVideo_editingFinished();

    void on_lineEditDescriptionVideo_editingFinished();

    void on_horizontalSlider_valueChanged(int value);

    void on_buttonFramePrevious_clicked();

    void on_buttonFrameNext_clicked();

    void on_buttonSetStartTime_clicked();

    void on_buttonSetEndTime_clicked();

    void showMetadataEditor();

    void showAudioStreamsSelection();

    void showSubtitlesSelection();

    void showVideoSplitter();

    void repeatHandler_Type_1();

    void repeatHandler_Type_2();

    void on_buttonApplyPreset_clicked();

    void on_actionRemove_preset_clicked();

    void on_actionEdit_preset_clicked();

    void add_section();

    void add_preset();

    void renameSectionPreset();

    void setPresetIcon(QTreeWidgetItem *item, bool collapsed);

    void on_treeWidget_itemCollapsed(QTreeWidgetItem *item);

    void on_treeWidget_itemExpanded(QTreeWidgetItem *item);

    void on_buttonHotInputFile_clicked();

    void on_buttonHotOutputFile_clicked();

    void on_buttonCloseTaskWindow_clicked();

    void paintEvent(QPaintEvent *event);

    void on_treeWidget_itemChanged(QTreeWidgetItem *item, int column);

    void on_comboBoxMode_currentIndexChanged(int index);

    void on_horizontalSlider_resize_valueChanged(int value);

    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);

    void desktopEnvDetection();

private:

    Ui::Widget *ui;

    QTranslator qtTranslator;

    QString _cur_param[PARAMETERS_COUNT];

    QVector <QVector <QString> > _preset_table;

    OpeningFiles openingFiles;

    // ******************* Constants *****************************//

    static const int NUMBER_PRESETS = 23;

    static const int AMOUNT_AUDIO_STREAMS = 9;

    static const int AMOUNT_SUBTITLES = 9;

    static constexpr float MAXIMUM_ALLOWED_TIME = 359999.0f;

    // ******************** Dock area ****************************//

    QMainWindow *window;

    QWidget *centralWidget;

    static const int DOCKS_COUNT = 8;

    QDockWidget *docks[DOCKS_COUNT];

    // ******************** Top label ****************************//

    QLabel *raiseThumb;

    QLabel *audioThumb;

    QLabel *subtitleThumb;

    // **************** Progress animation ***********************//

    QMovie *animation;

    // ***************** Tray menu actions ************************//

    QSystemTrayIcon *trayIcon;

    QMenu *trayIconMenu;

    QAction *minimizeAction;

    QAction *restoreAction;

    QAction *quitAction;

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

    QMenu* menuView;

    QMenu* menuPreferences;

    QMenu* menuAbout;

    // ***************** Table menu actions ************************//

    QMenu* itemMenu;

    // ****************** Tree menu actions ************************//

    QMenu* sectionMenu;

    QMenu* presetMenu;

    // ***************** Preset menu actions ***********************//

    QAction *addsection;

    QAction *addpreset;

    QMenu* menu;

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


    QSettings *_settings;

    QString _curFilename;

    QString _curPath;

    QString _temp_file;

    QString _input_file;

    QString _output_file;

    QString _settings_path;

    QString _thumb_path;

    QString _settings_file;

    QString _preset_file;


    QString _preset_0;

    QString _preset_pass1;

    QString _preset;

    QString _preset_mkvmerge;

    QString _sub_mux_param;


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

    int _rowSize;

    bool _windowActivated = false;

    bool _expandWindowsState;

    bool clickPressedFlag;

    bool clickPressed_Left_ResizeFlag;

    bool clickPressed_Left_Top_ResizeFlag;

    bool clickPressed_Top_ResizeFlag;

    bool clickPressed_Right_Top_ResizeFlag;

    bool clickPressed_Right_ResizeFlag;

    bool clickPressed_Right_Bottom_ResizeFlag;

    bool clickPressed_Bottom_ResizeFlag;

    bool clickPressed_Left_Bottom_ResizeFlag;

    QPoint mouseClickCoordinate;

    QPoint mouseCoordinate;

    int oldWidth;

    int oldHeight;

    int curWidth;

    int curHeight;

    // ****************************************************************************//

    void createConnections();

    void on_actionAbout_clicked();

    void on_actionDonate_clicked();

    void showOpeningFiles(bool status);

    void showOpeningFiles(QString text);

    void showOpeningFiles(int percent);

    void openFiles(const QStringList &file_name_open);

    void get_current_data();

    void get_output_filename();

    void setStatus(QString status);

    void restore_initial_state();

    void call_task_complete(const QString &_message, const bool &_timer_mode);

    bool call_dialog(const QString &_message);

    void setTheme(int &ind_theme);

    QString styleCreator(const QString &list);

    QString timeConverter(double &time);

    QString timeConverter(float &time);

    QString setThumbnail(QString curFilename, double time, QString quality, int destination);

    void provideContextMenu(const QPoint &position);

    void providePresetContextMenu(const QPoint &position);

    void resizeTableRows(int rows_height);

    /************************* Preset Window ******************************/

    void set_defaults();

    void setItemStyle(QTreeWidgetItem *item);

    void updateCurPresetPos(int &index_top, int &index_child);

    void updateInfoFields(QString &codec_qstr, QString &mode_qstr, QString &container_qstr,
                          QString &bqr_qstr, QString &pass_qstr, QString &preset_qstr,
                          QString &acodec_qstr, QTreeWidgetItem *item, bool defaultNameFlag);

    void updatePresetTable();

    QString updateFieldCodec(int &codec);

    QString updateFieldMode(int &codec, int &mode);

    QString updateFieldPreset(int &codec, int &preset);

    QString updateFieldPass(int &codec, int &pass);

    QString updateFieldAcodec(int &codec, int &acodec);

    QString updateFieldContainer(int &codec, int &container);
};

#endif // WIDGET_H
