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
#include <QFileSystemModel>
#include <QPixmap>
#include <QProcess>
#include <QSplitter>
#include <QLabel>
#include <QTimer>
#include <QMenuBar>
#include <ctime>
#include "encoder.h"
#include "constants.h"
#include "popupmessage.h"
#include "widgets/qanimatedsvg.h"
#include "widgets/qstreamview.h"

#ifdef WM_CUSTOM
    #include "basewindow_cwm.h"
#else
    #ifdef Q_OS_WIN
        #include "platform_win/basewindow.h"
    #else
        #include "platform_unix/basewindow.h"
    #endif
#endif


typedef QVector<QVector<QString>> TableString;

using namespace Constants;

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

private:
    void setFloating(const int index, const QPoint &offset, const QSize &size);
    void createConnections();
    void onActionAbout();
    void onActionDonate();
    void openFiles(const QStringList &file_name_open);
    void get_current_data();
    void get_output_filename();
    void setStatus(const QString &status);
    void setWidgetsEnabled(bool);
    void setProgressEnabled(bool);
    void showInfoMessage(const QString &message, const bool timer_mode = false);
    bool showDialogMessage(const QString &message);
    void showPopup(const QString &text, PopupMessage::Icon icon = PopupMessage::Icon::Info);
    void addReport(const QString &log, ReportLog::Icon icon = ReportLog::Icon::Info);
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
    void setDefaultPresets();
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
    void saveXMLSettingsFile();
    int doesParamsContain(QString findMe);
    void readXMLSettingsFile(QString xmlFileName);

    // ====================================
    Ui::Widget *ui;
    Encoder    *m_pEncoder;
    QVector<QString> m_newParams,
                     m_curParams;
    TableString      m_preset_table;
    QPixmap          m_preview_pixmap;

    // ============= Dock area =============
    QMainWindow *m_pDocksContainer;
    QWidget     *m_pCentralDock;
    QSplitter   *m_pSpl,
                *m_pSplSource;
    QDockWidget *m_pDocks[DOCKS_COUNT];

    // ============= Menu bar ==============
    QMenuBar    *m_pMenuBar;

    // ============= Top label =============
    QLabel      *m_pTableLabel,
                *m_pAudioLabel,
                *m_pSubtitleLabel;

    // ============= Progress animation =============
    QAnimatedSvg    *m_pAnimation;

    // ============= Tray menu actions =============
    QSystemTrayIcon *m_pTrayIcon;

    // ============= Top menu actions =============
    QAction     *m_pActAddFiles,
                *m_pActRemoveFile,
                *m_pActRemoveAllFiles,
                *m_pActCloseWindow,
                *m_pActSelectPreset,
                *m_pActStart,
                *m_pActStop,
                *m_pActEditMetadata,
                *m_pActSelectAudio,
                *m_pActSelectSubtitles,
                *m_pActSplitVideo,
                *m_pActSettings,
                *m_pActResetView,
                *m_pActAbout,
                *m_pActDonate,
                *m_pActAddToTask;

    // ============= Menu actions =============
    QMenu       *m_pItemMenu,
                *m_pFilesItemMenu,
                *m_pSectionMenu,
                *m_pPresetMenu;

    // ============= Processes =============
    QProcess    *m_pProcessThumbCreation;

    // ============= Timers =============
    QTimer      *m_pTimer,
                *m_pTimerSetThumbnail;

    // ============= Initialization =============
    QString     m_openDir;
    int         m_theme;

    // ============== Report ==============
    QVector<ReportLog> m_reportLog;

    // ============= Metadata =============
    QString       m_hdr[AMOUNT_HDR_PARAMS];
    QVector<Data> m_data;

    // ============= Encoding =============
    bool        m_batch_mode;

    int         m_fr_count,
                m_status_encode_btn,
                m_row;

    time_t      m_strt_t;

    double      m_dur,
                m_curTime,
                m_startTime,
                m_endTime;

    QString     m_curFilename,
                m_curPath,
                m_temp_file,
                m_input_file,
                m_output_file,
                m_fmt,
                m_width,
                m_height,
                m_fps,
                m_stream_size,
                extension;

    // ============= Settings ================
    bool        m_hideInTrayFlag,
                m_showHdrFlag,
                m_multiInstances,
                m_protectFlag,
                m_subtitles_background;

    int         m_fontSize,
                m_subtitles_fontSize,
                m_prefxType,
                m_suffixType,
                m_pos_top,
                m_pos_cld,
                m_timerInterval,
                m_subtitles_background_alpha,
                m_subtitles_location,
                m_threads;

    QString     m_language,
                m_output_folder,
                m_temp_folder,
                m_prefixName,
                m_suffixName,
                m_font,
                m_subtitles_font;

    QColor      m_subtitles_color,
                m_subtitles_background_color;

    // ============= Geometry =============
    bool        m_windowActivated;
    bool        m_expandWindowsState;
    int         m_rowHeight;

    QFileSystemModel *m_pDirModel,
                     *m_pFileModel;

private slots:
    void setTrayIcon();
    void setExpandIcon();
    void onCloseWindow();
    void onHideWindow();
    void onReport();
    void onBack();
    void onForward();
    void onViewMode(uchar ind);
    void setBrowser();
    void onRestoreWindowState();
    void onSettings();
    void onAddFiles();
    void onRemoveFile();
    void onRemoveAllFiles();
    void onStart();
    void onStop();
    void onSort(const bool up);
    void onSortDown();
    void onSortUp();
    void setParameters();
    bool readXMLPresetFile(QString file);
    void setDocksParameters(const QList<int>& dockSizesX, const QList<int>& dockSizesY);

    void showEvent(QShowEvent*);
    void closeEvent(QCloseEvent*);
    void changeEvent(QEvent*);
    bool eventFilter(QObject*, QEvent*);
    void dragEnterEvent(QDragEnterEvent*);
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
    void onEncodingError(const QString &error_message, bool popup = false);
    void pause();
    void resume();

    // ============= Video Metadata ================
    void onClearMetadata();
    void onUndoMetadata();

    // ============ Streams Metadata ===============
    void onAddExtStream();
    void onDeselectTitles();
    void onClearTitles();
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
    void onTreeDirsClicked(const QModelIndex&);
    void onTreeDirsDblClicked(const QModelIndex&);
    void provideListContextMenu(const QPoint&);
    void onAddToTask();
    void onExtract(QStreamView::Content type, int num);
};

#endif // WIDGET_H
