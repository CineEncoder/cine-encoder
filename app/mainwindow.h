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
    bool        m_hideInTrayFlag,
                m_showHdrFlag,
                m_protectFlag;

    int         m_fontSize,
                m_prefxType,
                m_suffixType,
                m_pos_top,
                m_pos_cld,
                m_timerInterval;

    QString     _language,
                _output_folder,
                _temp_folder,
                _prefixName,
                _suffixName,
                _font;

    QVector<QString> _new_param;

private slots:
    void setTrayIcon();
    void setExpandIcon();
    void onCloseWindow();
    void onHideWindow();
    void onRestoreWindowState();
    void onSettings();
    void onAddFiles();
    void onRemoveFile();
    void onStart();
    void onStop();
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

    void onTableSelectionChanged();

    // ============= Encoder ================
    void initEncoding();
    void onEncodingMode(const QString &mode);
    void onEncodingStarted();
    void onEncodingInitError(const QString &_message);
    void onEncodingProgress(const int percent, const float rem_time);
    void onEncodingLog(const QString &log);
    void onEncodingCompleted();
    void onEncodingAborted();
    void onEncodingError(const QString &error_message);
    void pause();
    void resume();

    // ============= Video Metadata ================
    void onClearMetadata();
    void onUndoMetadata();

    // ============ Streams Metadata ===============
    void onClearATitles();
    void onClearSTitles();
    void onUndoTitles();

    // ============== Split Video ==================
    void onSliderTimelineChanged(int value);
    void onFramePrev();
    void onFrameNext();
    void onSetStartTime();
    void onSetEndTime();

    void showMetadataEditor();
    void showAudioStreams();
    void showSubtitles();
    void showVideoSplitter();
    void repeatHandler_Type_1();
    void repeatHandler_Type_2();
    void onApplyPreset();
    void onRemovePreset();
    void onEditPreset();
    void onAddSection();
    void onAddPreset();
    void onRenamePreset();
    void setPresetIcon(QTreeWidgetItem *item, bool collapsed);
    void onTreeCollapsed(QTreeWidgetItem *item);
    void onTreeExpanded(QTreeWidgetItem *item);
    void onTreeChanged(QTreeWidgetItem *item, int column);
    void onTreeDblClicked(QTreeWidgetItem *item, int column);
    void onSliderResizeChanged(int value);
    void onSetOutFolder();
    void paintEvent(QPaintEvent *event);
    void onComboModeChanged(int index);
    void onResetLabels();

private:
    Ui::Widget *ui;
    Encoder *m_pEncoder;
    QVector<QString> _cur_param;
    QVector<QVector<QString>> _preset_table;
    QPixmap preview_pixmap;

    // ============= Dock area =============
    QMainWindow *m_pDocksContainer;
    QWidget     *m_pCentralDock;
    QDockWidget *m_pDocks[DOCKS_COUNT];

    // ============= Top label =============
    QLabel      *m_pTableLabel,
                *m_pAudioLabel,
                *m_pSubtitleLabel;

    // ============= Progress animation =============
    QMovie      *animation;

    // ============= Tray menu actions =============
    QSystemTrayIcon *trayIcon;

    // ============= Top menu actions =============
    QAction     *actAddFiles,
                *actRemoveFile,
                *actCloseWindow;

    QAction     *select_preset,
                *actStart,
                *actStop;

    QAction     *actEditMetadata,
                *actSelectAudio,
                *actSelectSubtitles,
                *actSplitVideo;

    QAction     *actSettings,
                *actResetView,
                *actAbout,
                *actDonate;

    // ============= Table menu actions =============
    QMenu       *itemMenu;

    // ============= Tree menu actions =============
    QMenu       *sectionMenu,
                *presetMenu;

    // ============= Processes =============
    QProcess    *m_pProcessThumbCreation;

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
    bool        _expandWindowsState;
    int         m_rowHeight;

    // ====================================
    void setFloating(const int index, const QPoint &offset, const QSize &size);
    void createConnections();
    void onActionAbout();
    void onActionDonate();
    void openFiles(const QStringList &file_name_open);
    void get_current_data();
    void get_output_filename();
    void setStatus(const QString &status);
    void restore_initial_state();
    void showInfoMessage(const QString &message, const bool timer_mode = false);
    bool showDialogMessage(const QString &message);

    void setTheme(const int ind_theme);
    QString setThumbnail(QString curFilename,
                         const double time,
                         const int quality,
                         const int destination);

    void provideContextMenu(const QPoint &position);
    void providePresetContextMenu(const QPoint &position);
    void resizeTableRows(const int rows_height);
    void resetView();

    // ============= Preset Window =============
    void set_defaults();
    void setItemStyle(QTreeWidgetItem *item);
    void updateCurPresetPos(const int index_top, const int index_child);
    void updateInfoFields(const QString &codec_qstr,
                          const QString &mode_qstr,
                          const QString &container_qstr,
                          const QString &bqr_qstr,
                          const QString &pass_qstr,
                          const QString &preset_qstr,
                          const QString &acodec_qstr,
                          QTreeWidgetItem *item,
                          const bool defaultNameFlag);

    void updatePresetTable();
};

#endif // WIDGET_H
