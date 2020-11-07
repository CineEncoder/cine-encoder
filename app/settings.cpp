#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <QCloseEvent>
//#include <iostream>
#include "settings.h"
#include "ui_settings.h"
#include "taskcomplete.h"

extern QFile _stn_file;
extern QString _message;
extern QString _output_folder;
extern QString _temp_folder;
extern bool _batch_mode;
QString _curr_output_folder;
QString _curr_temp_folder;
bool _curr_batch_mode;
bool _flag_save;

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui_settings(new Ui::Settings)
{
    ui_settings->setupUi(this);
    //ui_settings->checkBox_2->hide();
    ui_settings->lineEdit_9->setText(_temp_folder);
    ui_settings->lineEdit_10->setText(_output_folder);
    if (_batch_mode == true) {
        ui_settings->checkBox_1->setChecked(true);
    };
    _curr_output_folder = _output_folder;
    _curr_temp_folder = _temp_folder;
    _curr_batch_mode = _batch_mode;
    _flag_save = false;
}

Settings::~Settings()
{
    delete ui_settings;
}

void Settings::closeEvent(QCloseEvent *close_settings)  // Show prompt when close app
{
    close_settings->ignore();
    if (_flag_save == false) {
        _output_folder = _curr_output_folder;
        _temp_folder = _curr_temp_folder;
        _batch_mode = _curr_batch_mode;
    };
    close_settings->accept();
}

void Settings::on_pushButton_7_clicked() // Close settings window
{
    this->close();
}

void Settings::on_pushButton_8_clicked() // Reset settings
{
    ui_settings->lineEdit_9->clear();
    ui_settings->lineEdit_10->clear();
    ui_settings->checkBox_1->setChecked(false);
    _temp_folder = "";
    _output_folder = "";
    _batch_mode = false;
}

void Settings::on_pushButton_6_clicked() // Save settings
{
    if (_stn_file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QString line_0 = "temp_folder:" + _temp_folder + "\n";
        _stn_file.write(line_0.toUtf8());
        QString line_1 = "output_folder:" + _output_folder + "\n";
        _stn_file.write(line_1.toUtf8());
        if (_batch_mode == true) {
            _stn_file.write("batch_mode:true\n");
        } else {
            _stn_file.write("batch_mode:false\n");
        };
        _stn_file.close();
        _flag_save = true;
        this->close();
    } else {
        _message = "Settings file not found!\n";
        Taskcomplete taskcomplete(this);
        taskcomplete.setModal(true);
        taskcomplete.exec();
    };
}

void Settings::on_pushButton_5_clicked() // Select temp folder
{
    QString temp_folder_name = QFileDialog::getExistingDirectory(this, tr("Select temp folder"), QDir::currentPath());
    if (temp_folder_name.isEmpty()) {
        return;
    };
    ui_settings->lineEdit_9->setText(temp_folder_name);
    _temp_folder = temp_folder_name;
}

void Settings::on_pushButton_4_clicked()  // Select output folder
{
    QString output_folder_name = QFileDialog::getExistingDirectory(this, tr("Select output folder"), QDir::currentPath());
    if (output_folder_name.isEmpty()) {
        return;
    };
    ui_settings->lineEdit_10->setText(output_folder_name);
    _output_folder = output_folder_name;
}

void Settings::on_checkBox_1_clicked()  // Batch mode select
{
    int stts_1 = ui_settings->checkBox_1->checkState();
    if (stts_1 == 2) {
        _batch_mode = true;
    } else {
        _batch_mode = false;
    };
}

