#include <QFile>
#include <QFileDialog>
#include <QDir>
#include <iostream>
#include "settings.h"
#include "ui_settings.h"
#include "taskcomplete.h"

extern QString _message;


Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui_settings(new Ui::Settings)
{
    ui_settings->setupUi(this);
    QString Home_Path = QDir::homePath();
    QFile f(Home_Path + "/.ce_settings"); // Read settings from file
    if (f.exists() && f.open(QIODevice::ReadOnly)) {
        int j = 0;
        QStringList line;
        while(!f.atEnd())
        {
            j ++;
            line << f.readLine();
            if (j == 1) {
                std::string a = line[0].toStdString();
                if ((a.find("temp_folder:") != std::string::npos) && (a.find("[default]") != std::string::npos)) {
                    ui_settings->lineEdit_9->clear();
                };
                if ((a.find("temp_folder:") != std::string::npos) && (a.find("[default]") == std::string::npos)) {
                    QStringList b = line[0].split(":");
                    QString c = b[1].replace("[", "").replace("]\n", "");
                    ui_settings->lineEdit_9->setText(c);
                };
            };
        };
        if (j == 0) {
            _message = "Settings file error!\n";
            Taskcomplete taskcomplete;
            taskcomplete.setModal(true);
            taskcomplete.exec();
        };
        f.close();
    } else {
        _message = "Please select a temporary folder!\n";
        Taskcomplete taskcomplete;
        taskcomplete.setModal(true);
        taskcomplete.exec();
    };
}

Settings::~Settings()
{
    delete ui_settings;
}

void Settings::on_pushButton_7_clicked() // Close settings window
{
    close();
}

void Settings::on_pushButton_8_clicked() // Reset settings
{
    ui_settings->lineEdit_9->clear();
}

void Settings::on_pushButton_6_clicked() // Save settings
{
    QString temp_folder_name = ui_settings->lineEdit_9->text();
    QString Home_Path = QDir::homePath();
    QFile f(Home_Path + "/.ce_settings"); // Writing settings to file
    if (f.open(QIODevice::WriteOnly)) {
        if (temp_folder_name == "") {
            f.write("temp_folder:[default]\n");
        } else {
            QString line = "temp_folder:[" + temp_folder_name + "]\n";
            f.write(line.toUtf8());
        };
        f.close();
        close();
    } else {
        _message = "Settings file not found!\n";
        Taskcomplete taskcomplete;
        taskcomplete.setModal(true);
        taskcomplete.exec();
    };
}

void Settings::on_pushButton_5_clicked() // Select temp folder
{
    ui_settings->lineEdit_9->clear();
    QString temp_folder_name = QFileDialog::getExistingDirectory(this, tr("Select temp folder"), QDir::currentPath());
    ui_settings->lineEdit_9->setText(temp_folder_name);
}
