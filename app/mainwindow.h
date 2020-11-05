#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void closeEvent(QCloseEvent *event);

    void on_actionAbout_clicked();

    void on_actionSettings_clicked();

    void make_preset();

    void encode();

    void add_metadata();

    void complete();

    void on_actionAdd_clicked();

    void on_actionRemove_clicked();

    void on_actionPreset_clicked();

    void on_actionEncode_clicked();

    void pause();

    void resume();

    void on_actionStop_clicked();

    void cancel();

    void error_1();

    void progress_1();

    void progress_2();

    void get_current_data();

    void get_output_filename();

    void on_tableWidget_itemSelectionChanged();

    void set_defaults();

    void restore_initial_state();

    void call_task_complete();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
