#include "mainwindow.h"
#include "smartsettings.h"
#include "ui_smartsettings.h"
#include <iostream>


extern QString width_;
extern QString height_;
extern QString fps;
extern float dur;
extern float k;
float k1;
int smart_bitrate;
int BR;
int MR;
//extern QString _combobox_23_value;
//extern int sampling;
//extern int bit_depth;
bool flag_no_smart;
//int _combobox_24_index = 0;
//int _combobox_25_index = 0;
//int _combobox_26_index = 0;

SmartSettings::SmartSettings(QWidget *parent) :
    QDialog(parent),
    ui_smartsettings(new Ui::SmartSettings)
{
    ui_smartsettings->setupUi(this);
    flag_no_smart = false;
    QString k_qstr = QString::number(k);
    ui_smartsettings->lineEdit_BPP->setText(k_qstr);
    k1 = k;
    ui_smartsettings->doubleSpinBox_BPP_2->setValue(k1);
    int width_int = 0;
    int height_int = 0;
    float fps_qstr = fps.toFloat();
    width_int = width_.toInt();
    height_int = height_.toInt();
    smart_bitrate = (k1 * width_int * height_int * fps_qstr) / (1024);
    ui_smartsettings->doubleSpinBox_BR->setValue(smart_bitrate);
    int smart_bitrate_ = 1.1 * smart_bitrate;
    ui_smartsettings->doubleSpinBox_MR->setValue(smart_bitrate_);
    int p_int = (dur * smart_bitrate) / (8 * 1024);
    QString p_qstr = QString::number(p_int);
    ui_smartsettings->lineEdit_5->setText(p_qstr);
    //print(fmt);

}


SmartSettings::~SmartSettings()
{
    delete ui_smartsettings;
}

void SmartSettings::on_pushButton_11_clicked() // Close
{
    close();
}

void SmartSettings::on_horizontalSlider_valueChanged() // Slider_changed
{
    int corr_0 = ui_smartsettings->horizontalSlider->value();
    float corr_1 = corr_0;
    float corr = corr_1/500;
    ui_smartsettings->doubleSpinBox_BPP_2->setValue(k1 + corr);
    float fps_qstr = fps.toFloat();
    int width_int = 0;
    int height_int = 0;
    width_int = width_.toInt();
    height_int = height_.toInt();
    smart_bitrate = ((k1 + corr) * width_int * height_int * fps_qstr) / (1024);
    ui_smartsettings->doubleSpinBox_BR->setValue(smart_bitrate);
    int smart_bitrate_ = 1.1 * smart_bitrate;
    ui_smartsettings->doubleSpinBox_MR->setValue(smart_bitrate_);
    int q_int = (dur * smart_bitrate) / (8 * 1024);
    QString q = QString::number(q_int);
    ui_smartsettings->lineEdit_5->setText(q);
}

void SmartSettings::on_pushButton_10_clicked() // Execute_make_preset
{
    BR = ui_smartsettings->doubleSpinBox_BR->value();
    MR = ui_smartsettings->doubleSpinBox_MR->value();
    flag_no_smart = true;
    close();
}

//void SmartSettings::on_pushButton_10_clicked() // execute_make_preset
//{
//    int content_type = ui_smartsettings->comboBox_24->currentIndex();
//    int bit_depth_output = ui_smartsettings->comboBox_25->currentIndex();
//    if (_combobox_23_value == "FLAC") {
//        int compression = ui_smartsettings->comboBox_26->currentIndex();
//        _combobox_24_index = content_type + 1;
//        _combobox_26_index = bit_depth_output + 1;
//        _combobox_25_index = compression + 1;
//    };
//    if (_combobox_23_value == "WAV") {
//        _combobox_24_index = content_type + 1;
//        _combobox_26_index = bit_depth_output + 1;
//    };
//    flag_no_smart = true;
//    close();
//}

//void SmartSettings::on_comboBox_27_currentTextChanged() // change_optimize_for
//{
//    change_optimize_for();
//}



//void SmartSettings::change_optimize_for()
//{
//    QString optimize_for = ui_smartsettings->comboBox_27->currentText();
//    if (sampling <= 8000) {
//        ui_smartsettings->comboBox_24->setCurrentIndex(0);
//    };
//    if ((sampling <= 11025) && (sampling > 8000)) {
//        ui_smartsettings->comboBox_24->setCurrentIndex(1);
//    };
//    if ((sampling <= 16000) && (sampling > 11025)) {
//        ui_smartsettings->comboBox_24->setCurrentIndex(2);
//    };
//    if ((sampling <= 22050) && (sampling > 16000)) {
//        ui_smartsettings->comboBox_24->setCurrentIndex(3);
//    };
//    if ((sampling <= 24000) && (sampling > 22050)) {
//        ui_smartsettings->comboBox_24->setCurrentIndex(4);
//    };
//    if ((sampling <= 32000) && (sampling > 24000)) {
//        ui_smartsettings->comboBox_24->setCurrentIndex(5);
//    };
//    if ((sampling <= 44100) && (sampling > 32000)) {
//        ui_smartsettings->comboBox_24->setCurrentIndex(6);
//    };
//    if (optimize_for == "Listening in the car on high-quality equipment") {
//        if (sampling > 44100) {
//            ui_smartsettings->comboBox_24->setCurrentIndex(6);
//        };
//        ui_smartsettings->comboBox_25->setCurrentIndex(0);
//    };
//    if (optimize_for == "Listening at home on standard equipment") {
//        if (sampling > 44100) {
//            if (ui_smartsettings->checkBox->isChecked()) {
//                ui_smartsettings->comboBox_24->setCurrentIndex(7);
//            } else {
//                ui_smartsettings->comboBox_24->setCurrentIndex(6);
//            };
//        };
//        ui_smartsettings->comboBox_25->setCurrentIndex(0);
//    };
//    if (optimize_for == "Listening at home on high-quality equipment") {
//        if (sampling > 44100) {
//            ui_smartsettings->comboBox_24->setCurrentIndex(7);
//            if (ui_smartsettings->checkBox->isChecked()) {
//                if (bit_depth == 16) {
//                    ui_smartsettings->comboBox_25->setCurrentIndex(0);
//                };
//                if (bit_depth >= 24) {
//                    ui_smartsettings->comboBox_25->setCurrentIndex(1);
//                };
//            } else {
//                ui_smartsettings->comboBox_25->setCurrentIndex(0);
//            };
//        };
//    };
//    if (optimize_for == "Studio quality") {
//        if ((sampling <= 48000) && (sampling > 44100)) {
//            ui_smartsettings->comboBox_24->setCurrentIndex(7);
//        };
//        if ((sampling <= 88200) && (sampling > 48000)) {
//            ui_smartsettings->comboBox_24->setCurrentIndex(8);
//        };
//        if ((sampling <= 96000) && (sampling > 88200)) {
//            ui_smartsettings->comboBox_24->setCurrentIndex(9);
//        };
//        if ((sampling <= 176400) && (sampling > 96000)) {
//            ui_smartsettings->comboBox_24->setCurrentIndex(10);
//        };
//        if ((sampling <= 192000) && (sampling > 176400)) {
//            ui_smartsettings->comboBox_24->setCurrentIndex(11);
//        };
//        if (bit_depth == 16) {
//            ui_smartsettings->comboBox_25->setCurrentIndex(0);
//        };
//        if (bit_depth >= 24) {
//            ui_smartsettings->comboBox_25->setCurrentIndex(1);
//        };
//    };
//}






