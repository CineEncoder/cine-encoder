#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>



namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

    QString _message;

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();






private slots:
    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

    void on_checkBox_1_clicked();

    void on_pushButton_4_clicked();

    void closeEvent(QCloseEvent *event);

private:

    Ui::Settings *ui_settings;
    QString _curr_output_folder;
    QString _curr_temp_folder;
    bool _curr_batch_mode;
    bool _flag_save;
};

#endif // SETTINGS_H
