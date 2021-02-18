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
//#include <QSysInfo>
//#include <QDebug>



extern QString _cur_param[23];
extern QVector <QVector <QString> > _preset_table;

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

private:

    Ui::MainWindow *ui;

    QHBoxLayout *raiseLayout = new QHBoxLayout();

    QLabel *raiseThumb = new QLabel(this);

    // ***************** Top menu actions ************************//

    QAction *add_files = new QAction(tr("Add files"), this);

    QAction *remove_files = new QAction(tr("Remove files"), this);

    QAction *close_prog = new QAction(tr("Close"), this);

    QAction *select_preset = new QAction(tr("Select preset"), this);

    QAction *encode_files = new QAction(tr("Encode/Pause"), this);

    QAction *stop_encode = new QAction(tr("Stop"), this);

    QAction *settings = new QAction(tr("Settings"), this);

    QAction *about = new QAction(tr("About"), this);

    QAction *donate = new QAction(tr("Donate"), this);

    // ************************************************************//

    QProcess *process_1 = new QProcess(this);

    QProcess *process_5 = new QProcess(this);

    QTimer *timer = new QTimer(this);

    QFile _prs_file;

    QFile _wind_file;

    QString _openDir;

    QString _hdr[11];

    QString _message;

    QString _error_message, _temp_file, _input_file, _output_file;

    QString _settings_path, _thumb_path, _settings_file, _preset_file, _window_file;

    QString _preset_0, _preset_pass1, _preset, _preset_mkvmerge;

    QString _fmt, _width, _height, _fps, _stream_size;

    QString _status_encode_btn;

    time_t _loop_start, _strt_t;

    float _dur;

    bool _flag_two_pass, _flag_hdr;

    bool _calling_pr_1, _mux_mode;

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

    int _theme;

    int _row, _fr_count;

    void openFiles(const QStringList &file_name_open);

    void get_current_data();

    void get_output_filename();

    void restore_initial_state();

    void set_defaults();

    void call_task_complete(const QString &_message, const bool &_timer_mode);

    bool call_dialog(const QString &_message);

    void setTheme(int &ind_theme);
};

#endif // MAINWINDOW_H
