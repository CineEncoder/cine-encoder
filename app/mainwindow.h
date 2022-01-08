/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: mainwindow.h
 MODIFIED: December, 2021
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

    bool        _hideInTrayFlag,
                _showHDR_mode,
                _protection;

    int         _fontSize,
                _prefxType,
                _suffixType,
                _pos_top,
                _pos_cld,
                _timer_interval;

    QString     _desktopEnv,
                _language,
                _output_folder,
                _temp_folder,
                _prefixName,
                _suffixName,
                _font;

    QByteArray  _settingsWindowGeometry,
                _presetWindowGeometry;

    QVector<QString> _new_param;


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

    void on_actionClearMetadata_clicked();

    void on_actionUndoMetadata_clicked();

    void on_actionClearAudioTitles_clicked();

    void on_actionClearSubtitleTitles_clicked();

    void on_actionUndoTitles_clicked();

    void on_actionResetLabels_clicked();

private:

    Ui::Widget *ui;

    QVector<QString> _cur_param;

    QVector<QVector<QString>> _preset_table;

    OpeningFiles openingFiles;

    // ******************* Constants *****************************//

    static const int PRESETS_VERSION = 352;

    static const int SETTINGS_VERSION = 352;

    static const int NUMBER_PRESETS = 29;

    static const int AMOUNT_AUDIO_STREAMS = 9;

    static const int AMOUNT_SUBTITLES = 9;

    static constexpr float MAXIMUM_ALLOWED_TIME = 359999.0f;

    // ******************** Dock area ****************************//

    static const int DOCKS_COUNT = 8;

    QMainWindow *window;

    QWidget     *centralWidget;

    QDockWidget *docks[DOCKS_COUNT];

    // ******************** Top label ****************************//

    QLabel      *raiseThumb,
                *audioThumb,
                *subtitleThumb;

    // **************** Progress animation ***********************//

    QMovie      *animation;

    // ***************** Tray menu actions ************************//

    QSystemTrayIcon *trayIcon;

    QMenu       *trayIconMenu;

    QAction     *minimizeAction,
                *restoreAction,
                *quitAction;

    // ***************** Top menu actions ************************//

    QAction     *add_files,
                *remove_files,
                *close_prog;

    QAction     *select_preset,
                *encode_files,
                *stop_encode;

    QAction     *edit_metadata,
                *select_audio,
                *select_subtitles,
                *split_video;

    QAction     *settings,
                *reset_view,
                *about,
                *donate;

    QMenu       *menuFiles,
                *menuEdit,
                *menuTools,
                *menuView,
                *menuPreferences,
                *menuAbout;

    // ***************** Table menu actions ************************//

    QMenu       *itemMenu;

    // ****************** Tree menu actions ************************//

    QMenu       *sectionMenu,
                *presetMenu;

    // ***************** Preset menu actions ***********************//

    QAction     *addsection,
                *addpreset;

    QMenu       *menu;

    // ********************** Processes ****************************//

    QProcess    *processEncoding,
                *processThumbCreation;

    // *********************** Timers ******************************//

    QTimer      *timer,
                *timerCallSetThumbnail;

    // ******************** Initialization *************************//

    QFile       _wind_file;

    QString     _openDir;

    int         _theme;

    // ************************** Metadata ************************//

    int         _audioStreamCheckState[AMOUNT_AUDIO_STREAMS],
                _subtitleCheckState[AMOUNT_SUBTITLES];

    QString     _hdr[11],
                _videoMetadata[6],
                _audioLang[AMOUNT_AUDIO_STREAMS],
                _audioTitle[AMOUNT_AUDIO_STREAMS],
                _subtitleLang[AMOUNT_SUBTITLES],
                _subtitleTitle[AMOUNT_SUBTITLES];

    // ************************* Encoding ***************************//

    bool        _flag_two_pass,
                _flag_hdr,
                _calling_pr_1,
                _mux_mode,
                _batch_mode;

    int         _fr_count,
                _row;

    time_t      _loop_start,
                _strt_t;

    double      _dur,
                _curTime,
                _startTime,
                _endTime;

    QString     _message,
                _error_message,
                _curFilename,
                _curPath,
                _temp_file,
                _input_file,
                _output_file,
                _settings_path,
                _thumb_path,
                _preset_file,
                _preset_0,
                _preset_pass1,
                _preset,
                _preset_mkvmerge,
                _sub_mux_param,
                _fmt,
                _width,
                _height,
                _fps,
                _stream_size,
                _status_encode_btn;

    QSettings   *_settings;

    // *********************** Geometry ***********************************//

    enum Resize {
        LEFT,   LEFT_TOP,   LEFT_BOTTOM,   TOP,
        RIGHT,  RIGHT_TOP,  RIGHT_BOTTOM,  BOTTOM
    };

    bool        _windowActivated,
                _expandWindowsState,
                _clickPressedFlag;

    int         _rowSize,
                _oldPosX,
                _oldPosY,
                _oldWidth,
                _oldHeight;

    QPoint      _mouseClickCoordinate,
                _globalMouseClickCoordinate;

    QVector<bool> _clickPressedToResizeFlag;    

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

    void resetView();

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

    enum ColumnIndex {
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

    enum CurParamIndex {
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

    enum CurHDRIndex {
        CUR_COLOR_RANGE,    CUR_COLOR_PRIMARY,  CUR_COLOR_MATRIX,
        CUR_TRANSFER,       CUR_MAX_LUM,        CUR_MIN_LUM,
        CUR_MAX_CLL,        CUR_MAX_FALL,       CUR_MASTER_DISPLAY,
        CUR_CHROMA_COORD,   CUR_WHITE_COORD
    };

    enum VideoMetadataIndex {
        VIDEO_TITLE,        VIDEO_AUTHOR,       VIDEO_YEAR,
        VIDEO_PERFORMER,    VIDEO_DESCRIPTION,  VIDEO_MOVIENAME, ENUM_END_LABEL
    };

    enum DockIndex {
        PRESETS_DOCK, PREVIEW_DOCK, SOURCE_DOCK, OUTPUT_DOCK,
        STREAMS_DOCK, LOG_DOCK, METADATA_DOCK, SPLIT_DOCK
    };

    enum Theme {
        GRAY, DARK, WAVE, DEFAULT
    };
};

#endif // WIDGET_H
