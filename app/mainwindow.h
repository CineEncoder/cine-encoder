#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTimer>
#include <QFileDialog>


QT_BEGIN_NAMESPACE
namespace Ui {
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

    bool _protection;

    int _timer_interval;

private slots:

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


private:

    Ui::MainWindow *ui;

    QProcess *process_1 = new QProcess;

    QProcess *process_5 = new QProcess;

    QTimer *timer = new QTimer(this);

    QFile _prs_file;

    QString _hdr[11];

    QString _message;

    QString _error_message, _temp_file, _input_file, _output_file;

    QString _settings_path, _thumb_path, _settings_file, _preset_file;

    QString _preset_0, _preset_pass1, _preset, _preset_mkvmerge;

    QString _fmt, _width, _height, _fps, _stream_size;

    QString _status_encode_btn;

    time_t _loop_start, _strt_t;

    float _dur;

    bool _flag_two_pass, _flag_hdr;

    bool _calling_pr_1, _mux_mode;

    int _row, _fr_count;

    void get_current_data();

    void get_output_filename();

    void restore_initial_state();

    void set_defaults();

    void call_task_complete(QString _message, bool timer_mode);

    void set_theme(int ind_theme);
};

#endif // MAINWINDOW_H
