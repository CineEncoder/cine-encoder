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
#include <QMovie>
#include <QDockWidget>
#include <QFile>
#include <QTreeWidgetItem>
#include <QSizePolicy>
#include <QSettings>
#include <QMap>
#include <QTranslator>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <math.h>
#include "openingfiles.h"
#include "encoder.h"
#include "constants.h"



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
    bool event(QEvent *event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dragMoveEvent(QDragMoveEvent* event);
    void dragLeaveEvent(QDragLeaveEvent* event);
    void dropEvent(QDropEvent* event);   

    void on_tableWidget_itemSelectionChanged();
    QString callFileDialog(const QString title);

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

    Encoder *encoder;

    QVector<QString> _cur_param;

    QVector<QVector<QString>> _preset_table;

    OpeningFiles openingFiles;

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

    QString     _message,
                _curFilename,
                _curPath,
                _temp_file,
                _input_file,
                _output_file,
                _settings_path,
                _thumb_path,
                _preset_file,
                _fmt,
                _width,
                _height,
                _fps,
                _stream_size,
                extension;

    QSettings   *_settings;

    // ============= Geometry =============

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

    // ====================================

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
    QString timeConverter(const float &time);
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
