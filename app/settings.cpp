#include <QCloseEvent>
//#include <iostream>
#include "settings.h"
#include "ui_settings.h"
#include "taskcomplete.h"

extern int _theme;

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui_settings(new Ui::Settings)
{
    ui_settings->setupUi(this);
}

Settings::~Settings()
{
    delete ui_settings;
}

void Settings::on_pushButton_7_clicked() // Close settings window
{
    this->close();
}

void Settings::closeEvent(QCloseEvent *close_settings)  // Show prompt when close app
{
    close_settings->ignore();
    if (_flag_save == false) {
        *_ptr_output_folder = _curr_output_folder;
        *_ptr_temp_folder = _curr_temp_folder;
        *_ptr_batch_mode = _curr_batch_mode;
        *_ptr_protection = _curr_protection;
        *_ptr_timer_interval = _curr_timer_interval;
        _theme = _curr_theme;
    };
    close_settings->accept();
}

void Settings::setParameters(bool *ptr_batch_mode, QFile *ptr_stn_file,
                             QString *ptr_output_folder, QString *ptr_temp_folder,
                             bool *ptr_protection, int *ptr_timer_interval)  // Set parameters
{
    _ptr_batch_mode = ptr_batch_mode;
    _ptr_stn_file = ptr_stn_file;
    _ptr_output_folder = ptr_output_folder;
    _ptr_temp_folder = ptr_temp_folder;
    _ptr_protection = ptr_protection;
    _ptr_timer_interval = ptr_timer_interval;

    ui_settings->lineEdit_9->setText(*_ptr_temp_folder);
    ui_settings->lineEdit_10->setText(*_ptr_output_folder);
    ui_settings->spinBox_3->setValue(*_ptr_timer_interval);
    if (*_ptr_batch_mode == true) {
        ui_settings->checkBox_1->setChecked(true);
    };
    if (*_ptr_protection == true) {
        ui_settings->checkBox_3->setChecked(true);
        ui_settings->spinBox_3->setEnabled(true);
    };
    ui_settings->comboBox_1->setCurrentIndex(_theme);
    _curr_output_folder = *_ptr_output_folder;
    _curr_temp_folder = *_ptr_temp_folder;
    _curr_batch_mode = *_ptr_batch_mode;
    _curr_protection = *_ptr_protection;
    _curr_timer_interval = *_ptr_timer_interval;
    _curr_theme = _theme;
    _flag_save = false;
}

void Settings::on_pushButton_8_clicked() // Reset settings
{
    ui_settings->lineEdit_9->clear();
    ui_settings->lineEdit_10->clear();
    ui_settings->checkBox_1->setChecked(false);
    ui_settings->checkBox_3->setChecked(false);
    ui_settings->spinBox_3->setEnabled(false);
    ui_settings->comboBox_1->setCurrentIndex(0);
    *_ptr_temp_folder = "";
    *_ptr_output_folder = "";
    *_ptr_protection = false;
    *_ptr_batch_mode = false;
    _theme = 0;
}

void Settings::on_pushButton_6_clicked() // Save settings
{
    if ((*_ptr_stn_file).open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString line_0 = "temp_folder:" + *_ptr_temp_folder + "\n";
        (*_ptr_stn_file).write(line_0.toUtf8());
        QString line_1 = "output_folder:" + *_ptr_output_folder + "\n";
        (*_ptr_stn_file).write(line_1.toUtf8());
        if (*_ptr_batch_mode == true) {
            (*_ptr_stn_file).write("batch_mode:true\n");
        } else {
            (*_ptr_stn_file).write("batch_mode:false\n");
        };
        if (*_ptr_protection == true) {
            (*_ptr_stn_file).write("protection:true\n");
        } else {
            (*_ptr_stn_file).write("protection:false\n");
        };
        *_ptr_timer_interval = ui_settings->spinBox_3->value();
        QString line_2 = "timer_interval:" + QString::number(*_ptr_timer_interval) + "\n";
        (*_ptr_stn_file).write(line_2.toUtf8());
        _theme = ui_settings->comboBox_1->currentIndex();
        QString line_3 = "theme:" + QString::number(_theme) + "\n";
        (*_ptr_stn_file).write(line_3.toUtf8());
        (*_ptr_stn_file).close();
        _flag_save = true;
        this->close();
    } else {
        _message = "Settings file not found!\n";
        call_task_complete(_message, false);
    };
}

void Settings::on_pushButton_5_clicked() // Select temp folder
{
    QString temp_folder_name = QFileDialog::getExistingDirectory(this, tr("Select temp folder"), QDir::currentPath());
    if (temp_folder_name.isEmpty()) {
        return;
    };
    ui_settings->lineEdit_9->setText(temp_folder_name);
    *_ptr_temp_folder = temp_folder_name;
}

void Settings::on_pushButton_4_clicked()  // Select output folder
{
    QString output_folder_name = QFileDialog::getExistingDirectory(this, tr("Select output folder"), QDir::currentPath());
    if (output_folder_name.isEmpty()) {
        return;
    };
    ui_settings->lineEdit_10->setText(output_folder_name);
    *_ptr_output_folder = output_folder_name;
}

void Settings::on_checkBox_1_clicked()  // Batch mode select
{
    int stts_1 = ui_settings->checkBox_1->checkState();
    if (stts_1 == 2) {
        *_ptr_batch_mode = true;
    } else {
        *_ptr_batch_mode = false;
    };
}

void Settings::on_checkBox_3_clicked()  // Protection mode select
{
    int stts_3 = ui_settings->checkBox_3->checkState();
    if (stts_3 == 2) {
        *_ptr_protection = true;
        ui_settings->spinBox_3->setEnabled(true);
    } else {
        *_ptr_protection = false;
        ui_settings->spinBox_3->setEnabled(false);
    };
}

void Settings::call_task_complete(QString _message, bool timer_mode)   // Call task complete
{
    Taskcomplete taskcomplete(this);
    taskcomplete.setMessage(_message, timer_mode);
    taskcomplete.setModal(true);
    taskcomplete.exec();
}
