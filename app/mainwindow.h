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
    void on_actionSettings_clicked();

    void on_actionAbout_clicked();

    void on_pushButton_1_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void make_preset();

    void on_turnRightStack_1_clicked();

    void on_turnLeftStack_1_clicked();

    void on_turnRightStack_2_clicked();

    void on_turnLeftStack_2_clicked();

    void on_comboBox_2_currentTextChanged();

    void on_comboBox_1_currentTextChanged();

    void on_comboBox_4_currentTextChanged();

    void on_comboBox_15_currentTextChanged();

    void on_comboBox_12_currentTextChanged();

    void encode_mux_file();

    void add_metadata();

    void mux();

    void complete_mux();

    void encode_file();

    void complete();

    void error_1();

    void error_2();

    void error_3();

    void error_4();

    void cancel();

    void on_pushButton_4_clicked();

    void progress_1();

    void progress_2();

    void progress_3();

    void progress_4();

private:
    Ui::MainWindow *ui;

};

#endif // MAINWINDOW_H
