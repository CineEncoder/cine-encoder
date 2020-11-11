#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QFileDialog>


namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:

    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    void set_param(bool *ptr_batch_mode, QFile *ptr_stn_file,
                   QString *ptr_output_folder, QString *ptr_temp_folder,
                   bool *ptr_protection, int *ptr_timer_interval);

private slots:

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

    void on_checkBox_1_clicked();

    void on_pushButton_4_clicked();

    void closeEvent(QCloseEvent *event);

    void on_checkBox_3_clicked();

private:

    Ui::Settings *ui_settings;

    QString _message;

    QString _curr_output_folder;

    QString _curr_temp_folder;

    int _curr_timer_interval;

    bool _curr_batch_mode;

    bool _curr_protection;

    bool _flag_save;

    QFile *_ptr_stn_file;

    QString *_ptr_output_folder;

    QString *_ptr_temp_folder;

    bool *_ptr_batch_mode;

    bool *_ptr_protection;

    int *_ptr_timer_interval;

    void call_task_complete(QString _message, bool timer_mode);
};

#endif // SETTINGS_H
