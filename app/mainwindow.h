/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: mainwindow.h
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef WIDGET_H
#define WIDGET_H

#include <QtGlobal>
#include <QSystemTrayIcon>
#include <QTreeWidgetItem>
#include <QPixmap>
#include <QMovie>
#include <QProcess>
#include <QLabel>
#include <QTimer>
#include <ctime>
#include "encoder.h"
#include "constants.h"
#ifdef Q_OS_WIN
    #include "platform_win/basewindow.h"
#else
    #include "platform_unix/basewindow.h"
#endif

QT_BEGIN_NAMESPACE
namespace Ui {
    class Widget;
}
QT_END_NAMESPACE


class MainWindow : public BaseWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // ============= Settings ================
    bool        _hideInTrayFlag,
                _showHDR_mode,
                _protection;

    int         _fontSize,
                _prefxType,
                _suffixType,
                _pos_top,
                _pos_cld,
                _timer_interval;

    QString     _language,
                _output_folder,
                _temp_folder,
                _prefixName,
                _suffixName,
                _font;

    QVector<QString> _new_param;

private slots:
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
    void setTrayIconActions();
    void showTrayIcon();
    void setExpandIcon();
    void onCloseWindow();
    void onHideWindow();
    void onActionSettings();
    void onActionAdd();
    void onActionRemove();
    void onActionEncode();
    void onActionStop();
    void onSortDown();
    void onSortUp();
    void setParameters();
    void setDocksParameters(QList<int> dockSizesX, QList<int> dockSizesY);

    void showEvent(QShowEvent*);
    void closeEvent(QCloseEvent*);
    void changeEvent(QEvent*);
    bool eventFilter(QObject*, QEvent*);
    void dragEnterEvent(QDragEnterEvent*);
    void dragMoveEvent(QDragMoveEvent*);
    void dragLeaveEvent(QDragLeaveEvent*);
    void dropEvent(QDropEvent*);

    void onTableWidget_itemSelectionChanged();

    // ============= Encoder ================
    void initEncoding();
    void onEncodingMode(const QString &mode);
    void onEncodingStarted();
    void onEncodingInitError(const QString &_message);
    void onEncodingProgress(const int &percent, const float &rem_time);
    void onEncodingLog(const QString &log);
    void onEncodingCompleted();
    void onEncodingAborted();
    void onEncodingError(const QString &error_message);
    void pause();
    void resume();

    // ============= Video Metadata ================
    void onActionClearMetadata();
    void onActionUndoMetadata();

    // ============ Streams Metadata ===============
    void onActionClearAudioTitles();
    void onActionClearSubtitleTitles();
    void onActionUndoTitles();

    // ============== Split Video ==================
    void onSplitSlider_valueChanged(int value);
    void onButtonFramePrevious();
    void onButtonFrameNext();
    void onButtonSetStartTime();
    void onButtonSetEndTime();

    void showMetadataEditor();
    void showAudioStreamsSelection();
    void showSubtitlesSelection();
    void showVideoSplitter();
    void repeatHandler_Type_1();
    void repeatHandler_Type_2();
    void onApplyPreset();
    void onActionRemovePreset();
    void onActionEditPreset();
    void add_section();
    void add_preset();
    void renameSectionPreset();
    void setPresetIcon(QTreeWidgetItem *item, bool collapsed);
    void onTreeWidget_itemCollapsed(QTreeWidgetItem *item);
    void onTreeWidget_itemExpanded(QTreeWidgetItem *item);
    void onTreeWidget_itemChanged(QTreeWidgetItem *item, int column);
    void onTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void onIconResizeSlider_valueChanged(int value);
    void onButtonHotInputFile();
    void onButtonHotOutputFile();
    void onButtonCloseTaskWindow();
    void paintEvent(QPaintEvent *event);
    void onComboBoxMode_currentIndexChanged(int index);
    void onActionResetLabels();

private:
    Ui::Widget *ui;
    Encoder *m_pEncoder;
    QVector<QString> _cur_param;
    QVector<QVector<QString>> _preset_table;
    QPixmap preview_pixmap;

    // ============= Dock area =============
    QMainWindow *window;
    QWidget     *centralWidget;
    QDockWidget *docks[DOCKS_COUNT];

    // ============= Top label =============
    QLabel      *raiseThumb,
                *audioThumb,
                *subtitleThumb;

    // ============= Progress animation =============
    QMovie      *animation;

    // ============= Tray menu actions =============
    QSystemTrayIcon *trayIcon;

    QMenu       *trayIconMenu;

    QAction     *minimizeAction,
                *restoreAction,
                *quitAction;

    // ============= Top menu actions =============
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

    // ============= Table menu actions =============
    QMenu       *itemMenu;

    // ============= Tree menu actions =============
    QMenu       *sectionMenu,
                *presetMenu;

    // ============= Preset menu actions =============
    QAction     *addsection,
                *addpreset;

    QMenu       *menu;

    // ============= Processes =============
    QProcess    *processThumbCreation;

    // ============= Timers =============
    QTimer      *timer,
                *timerCallSetThumbnail;

    // ============= Initialization =============
    QFile       _wind_file;

    QString     _openDir;

    int         _theme;

    // ============= Metadata =============
    int         _audioStreamCheckState[AMOUNT_AUDIO_STREAMS],
                _subtitleCheckState[AMOUNT_SUBTITLES];

    QString     _hdr[11],
                _videoMetadata[6],
                _audioLang[AMOUNT_AUDIO_STREAMS],
                _audioTitle[AMOUNT_AUDIO_STREAMS],
                _subtitleLang[AMOUNT_SUBTITLES],
                _subtitleTitle[AMOUNT_SUBTITLES];

    // ============= Encoding =============
    bool        _batch_mode;

    int         _fr_count,
                _status_encode_btn,
                _row;

    time_t      _strt_t;

    double      _dur,
                _curTime,
                _startTime,
                _endTime;

    QString     _curFilename,
                _curPath,
                _temp_file,
                _input_file,
                _output_file,
                _fmt,
                _width,
                _height,
                _fps,
                _stream_size,
                extension;

    // ============= Geometry =============
    bool        _windowActivated,
                _expandWindowsState;

    int         m_rowHeight;

    // ====================================
    void createConnections();
    void onActionAbout();
    void onActionDonate();
    void openFiles(const QStringList &file_name_open);
    void get_current_data();
    void get_output_filename();
    void setStatus(QString status);
    void restore_initial_state();
    void showInfoMessage(const QString &message, const bool timer_mode = false);
    bool showDialogMessage(const QString &message);

    void setTheme(int &ind_theme);
    QString setThumbnail(QString curFilename,
                         const double &time,
                         const int &quality,
                         const int &destination);

    void provideContextMenu(const QPoint &position);
    void providePresetContextMenu(const QPoint &position);
    void resizeTableRows(int rows_height);
    void resetView();

    // ============= Preset Window =============
    void set_defaults();
    void setItemStyle(QTreeWidgetItem *item);
    void updateCurPresetPos(int &index_top, int &index_child);
    void updateInfoFields(QString &codec_qstr,
                          QString &mode_qstr,
                          QString &container_qstr,
                          QString &bqr_qstr,
                          QString &pass_qstr,
                          QString &preset_qstr,
                          QString &acodec_qstr,
                          QTreeWidgetItem *item,
                          bool defaultNameFlag);

    void updatePresetTable();
    QString updateFieldCodec(int &codec);
    QString updateFieldMode(int &codec, int &mode);
    QString updateFieldPreset(int &codec, int &preset);
    QString updateFieldPass(int &codec, int &pass);
    QString updateFieldAcodec(int &codec, int &acodec);
    QString updateFieldContainer(int &codec, int &container);

};

#endif // WIDGET_H
