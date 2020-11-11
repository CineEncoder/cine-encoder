#include <QTimer>
#include <iostream>
#include "preset.h"
#include "ui_preset.h"

extern QString _cur_param[23];

Preset::Preset(QWidget *parent) :
    QDialog(parent),
    ui_preset(new Ui::Preset)
{
    ui_preset->setupUi(this);
}

Preset::~Preset()
{
    delete ui_preset;
}

void Preset::setParameters()  // Set parameters
{
    _repeat = 0;
    QTimer *timer = new QTimer(this);
    timer->setInterval(450);
    connect(timer, SIGNAL(timeout()), this, SLOT(repeat_handler()));
    timer->start();
    ui_preset->lineEdit_presetname->setText(_cur_param[0]);
    if (_cur_param[0] != "New preset") {
        ui_preset->comboBox_codec->setCurrentIndex(_cur_param[1].toInt());
        ui_preset->comboBox_mode->setCurrentIndex(_cur_param[2].toInt());
        ui_preset->comboBox_container->setCurrentIndex(_cur_param[3].toInt());
        ui_preset->comboBox_bitrate->setCurrentText(_cur_param[4]);
        ui_preset->comboBox_maxrate->setCurrentText(_cur_param[5]);
        ui_preset->comboBox_bufsize->setCurrentText(_cur_param[6]);
        ui_preset->checkBox_2->setEnabled(bool(_cur_param[7].toUShort()));
        ui_preset->checkBox_2->setChecked(_cur_param[8].toInt());
        ui_preset->comboBox_width->setCurrentText(_cur_param[9]);
        ui_preset->comboBox_height->setCurrentText(_cur_param[10]);
        ui_preset->comboBox_pass->setCurrentIndex(_cur_param[11].toInt());
        ui_preset->comboBox_preset->setCurrentIndex(_cur_param[12].toInt());
        ui_preset->comboBox_color_range->setCurrentIndex(_cur_param[13].toInt());
        ui_preset->lineEdit_min_lum->setText(_cur_param[14]);
        ui_preset->lineEdit_max_lum->setText(_cur_param[15]);
        ui_preset->lineEdit_max_cll->setText(_cur_param[16]);
        ui_preset->lineEdit_max_fall->setText(_cur_param[17]);
        ui_preset->comboBox_master_disp->setCurrentIndex(_cur_param[18].toInt());
        ui_preset->lineEdit_chroma_coord->setText(_cur_param[19]);
        ui_preset->lineEdit_white_coord->setText(_cur_param[20]);
        ui_preset->comboBox_audio_codec->setCurrentIndex(_cur_param[21].toInt());
        ui_preset->comboBox_audio_bitrate->setCurrentIndex(_cur_param[22].toInt());
    };
}

void Preset::on_comboBox_codec_currentTextChanged(const QString &arg1)  //************* Change current codec ***************//
{
    std::cout << "Change current codec..." << std::endl;
    ui_preset->checkBox_2->setEnabled(true);
    ui_preset->comboBox_container->setEnabled(true);
    ui_preset->comboBox_mode->setEnabled(true);
    ui_preset->comboBox_pass->setEnabled(true);
    ui_preset->comboBox_preset->setEnabled(true);
    ui_preset->comboBox_color_range->setEnabled(true);
    ui_preset->lineEdit_min_lum->setEnabled(true);
    ui_preset->lineEdit_max_lum->setEnabled(true);
    ui_preset->lineEdit_max_cll->setEnabled(true);
    ui_preset->lineEdit_max_fall->setEnabled(true);
    ui_preset->comboBox_master_disp->setEnabled(true);
    ui_preset->comboBox_container->clear();
    ui_preset->comboBox_mode->clear();
    ui_preset->comboBox_pass->clear();
    ui_preset->comboBox_preset->clear();
    ui_preset->comboBox_audio_codec->clear();
    if (arg1 == "x265  10 bit") {
        ui_preset->comboBox_container->addItems({"MKV", "MOV", "MP4"});
        ui_preset->comboBox_container->setCurrentIndex(2);
        ui_preset->comboBox_mode->addItems({"Constant Bitrate", "Average Bitrate", "Variable Bitrate", "Constant Rate Factor", "Constant QP"});
        ui_preset->comboBox_pass->addItems({"1 Pass", "2 Pass"});
        ui_preset->comboBox_profile->setCurrentIndex(2);
        ui_preset->comboBox_preset->addItems({"Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow", "Slower", "Veryslow"});
        ui_preset->comboBox_preset->setCurrentIndex(6);
        ui_preset->comboBox_pixfmt->setCurrentIndex(2);
        ui_preset->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", "From source"});
    };
    if (arg1 == "x265  8 bit") {
        ui_preset->comboBox_container->addItems({"MKV", "MOV", "MP4", "M2TS", "TS"});
        ui_preset->comboBox_container->setCurrentIndex(2);
        ui_preset->comboBox_mode->addItems({"Constant Bitrate", "Average Bitrate", "Variable Bitrate", "Constant Rate Factor", "Constant QP"});
        ui_preset->comboBox_pass->addItems({"1 Pass", "2 Pass"});
        ui_preset->comboBox_profile->setCurrentIndex(1);
        ui_preset->comboBox_preset->addItems({"Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow", "Slower", "Veryslow"});
        ui_preset->comboBox_preset->setCurrentIndex(6);
        ui_preset->comboBox_pixfmt->setCurrentIndex(3);
        ui_preset->comboBox_color_range->setCurrentIndex(0);
        ui_preset->lineEdit_min_lum->clear();
        ui_preset->lineEdit_max_lum->clear();
        ui_preset->lineEdit_max_cll->clear();
        ui_preset->lineEdit_max_fall->clear();
        ui_preset->comboBox_master_disp->setCurrentIndex(0);
        ui_preset->comboBox_color_range->setEnabled(false);
        ui_preset->lineEdit_min_lum->setEnabled(false);
        ui_preset->lineEdit_max_lum->setEnabled(false);
        ui_preset->lineEdit_max_cll->setEnabled(false);
        ui_preset->lineEdit_max_fall->setEnabled(false);
        ui_preset->comboBox_master_disp->setEnabled(false);
        ui_preset->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", "From source"});
    };
    if (arg1 == "x264  8 bit") {
        ui_preset->comboBox_container->addItems({"MKV", "MOV", "MP4", "M2TS", "TS"});
        ui_preset->comboBox_container->setCurrentIndex(2);
        ui_preset->comboBox_mode->addItems({"Constant Bitrate", "Average Bitrate", "Variable Bitrate", "Constant Rate Factor", "Constant QP"});
        ui_preset->comboBox_pass->addItems({"1 Pass", "2 Pass"});
        ui_preset->comboBox_profile->setCurrentIndex(0);
        ui_preset->comboBox_preset->addItems({"Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow", "Slower", "Veryslow"});
        ui_preset->comboBox_preset->setCurrentIndex(6);
        ui_preset->comboBox_pixfmt->setCurrentIndex(3);
        ui_preset->comboBox_color_range->setCurrentIndex(0);
        ui_preset->lineEdit_min_lum->clear();
        ui_preset->lineEdit_max_lum->clear();
        ui_preset->lineEdit_max_cll->clear();
        ui_preset->lineEdit_max_fall->clear();
        ui_preset->comboBox_master_disp->setCurrentIndex(0);
        ui_preset->comboBox_color_range->setEnabled(false);
        ui_preset->lineEdit_min_lum->setEnabled(false);
        ui_preset->lineEdit_max_lum->setEnabled(false);
        ui_preset->lineEdit_max_cll->setEnabled(false);
        ui_preset->lineEdit_max_fall->setEnabled(false);
        ui_preset->comboBox_master_disp->setEnabled(false);
        ui_preset->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", "From source"});
    };
    if (arg1 == "x265  NVENC 10 bit") {
        ui_preset->comboBox_container->addItems({"MKV", "MOV", "MP4"});
        ui_preset->comboBox_container->setCurrentIndex(2);
        ui_preset->comboBox_mode->addItems({"Variable Bitrate"});
        ui_preset->comboBox_pass->addItems({"2 Pass Optimisation"});
        ui_preset->comboBox_profile->setCurrentIndex(2);
        ui_preset->comboBox_preset->addItems({"Slow"});
        //ui_preset->comboBox_preset->setCurrentIndex(6);
        ui_preset->comboBox_pixfmt->setCurrentIndex(4);
        ui_preset->comboBox_preset->setEnabled(false);
        ui_preset->comboBox_mode->setEnabled(false);
        ui_preset->comboBox_pass->setEnabled(false);
        ui_preset->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", "From source"});
    };
    if (arg1 == "x265  NVENC 8 bit") {
        ui_preset->comboBox_container->addItems({"MKV", "MOV", "MP4", "M2TS", "TS"});
        ui_preset->comboBox_container->setCurrentIndex(2);
        ui_preset->comboBox_mode->addItems({"Variable Bitrate"});
        ui_preset->comboBox_pass->addItems({"2 Pass Optimisation"});
        ui_preset->comboBox_profile->setCurrentIndex(1);
        ui_preset->comboBox_preset->addItems({"Slow"});
        //ui_preset->comboBox_preset->setCurrentIndex(6);
        ui_preset->comboBox_pixfmt->setCurrentIndex(3);
        ui_preset->comboBox_color_range->setCurrentIndex(0);
        ui_preset->lineEdit_min_lum->clear();
        ui_preset->lineEdit_max_lum->clear();
        ui_preset->lineEdit_max_cll->clear();
        ui_preset->lineEdit_max_fall->clear();
        ui_preset->comboBox_master_disp->setCurrentIndex(0);
        ui_preset->comboBox_preset->setEnabled(false);
        ui_preset->comboBox_mode->setEnabled(false);
        ui_preset->comboBox_pass->setEnabled(false);
        ui_preset->comboBox_color_range->setEnabled(false);
        ui_preset->lineEdit_min_lum->setEnabled(false);
        ui_preset->lineEdit_max_lum->setEnabled(false);
        ui_preset->lineEdit_max_cll->setEnabled(false);
        ui_preset->lineEdit_max_fall->setEnabled(false);
        ui_preset->comboBox_master_disp->setEnabled(false);
        ui_preset->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", "From source"});
    };
    if (arg1 == "x264  NVENC 8 bit") {
        ui_preset->comboBox_container->addItems({"MKV", "MOV", "MP4", "M2TS", "TS"});
        ui_preset->comboBox_container->setCurrentIndex(2);
        ui_preset->comboBox_mode->addItems({"Variable Bitrate"});
        ui_preset->comboBox_pass->addItems({"2 Pass Optimisation"});
        ui_preset->comboBox_profile->setCurrentIndex(0);
        ui_preset->comboBox_preset->addItems({"Slow"});
        //ui_preset->comboBox_preset->setCurrentIndex(6);
        ui_preset->comboBox_pixfmt->setCurrentIndex(3);
        ui_preset->comboBox_color_range->setCurrentIndex(0);
        ui_preset->lineEdit_min_lum->clear();
        ui_preset->lineEdit_max_lum->clear();
        ui_preset->lineEdit_max_cll->clear();
        ui_preset->lineEdit_max_fall->clear();
        ui_preset->comboBox_master_disp->setCurrentIndex(0);
        ui_preset->comboBox_preset->setEnabled(false);
        ui_preset->comboBox_mode->setEnabled(false);
        ui_preset->comboBox_pass->setEnabled(false);
        ui_preset->comboBox_color_range->setEnabled(false);
        ui_preset->lineEdit_min_lum->setEnabled(false);
        ui_preset->lineEdit_max_lum->setEnabled(false);
        ui_preset->lineEdit_max_cll->setEnabled(false);
        ui_preset->lineEdit_max_fall->setEnabled(false);
        ui_preset->comboBox_master_disp->setEnabled(false);
        ui_preset->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", "From source"});
    };
    if (arg1 == "VP9  10 bit") {
        ui_preset->comboBox_container->addItems({"WebM", "MKV"});
        ui_preset->comboBox_mode->addItems({"Average Bitrate", "Constant Rate Factor"});
        ui_preset->comboBox_pass->addItems({"1 Pass", "2 Pass"});
        ui_preset->comboBox_profile->setCurrentIndex(3);
        ui_preset->comboBox_preset->addItems({"Auto"});
        ui_preset->comboBox_pixfmt->setCurrentIndex(2);
        ui_preset->comboBox_preset->setEnabled(false);
        ui_preset->comboBox_audio_codec->addItems({"Opus", "Vorbis", "From source"});
    };
    if (arg1 == "AV1  10 bit") {
        ui_preset->comboBox_container->addItems({"WebM", "MKV"});
        ui_preset->comboBox_mode->addItems({"Average Bitrate", "Constant Rate Factor"});
        ui_preset->comboBox_pass->addItems({"1 Pass"});
        ui_preset->comboBox_profile->setCurrentIndex(7);
        ui_preset->comboBox_preset->addItems({"Auto"});
        ui_preset->comboBox_pixfmt->setCurrentIndex(5);
        ui_preset->comboBox_preset->setEnabled(false);
        ui_preset->comboBox_pass->setEnabled(false);
        ui_preset->comboBox_audio_codec->addItems({"Opus", "Vorbis", "From source"});
    };
    if (arg1 == "ProRes HQ") {
        ui_preset->checkBox_2->setChecked(false);
        ui_preset->checkBox_2->setEnabled(false);
        ui_preset->comboBox_container->addItems({"MOV"});
        ui_preset->comboBox_mode->addItems({"Auto"});
        ui_preset->comboBox_pass->addItems({"Auto"});
        ui_preset->comboBox_profile->setCurrentIndex(4);
        ui_preset->comboBox_preset->addItems({"Auto"});
        ui_preset->comboBox_pixfmt->setCurrentIndex(1);
        ui_preset->comboBox_preset->setEnabled(false);
        ui_preset->comboBox_container->setEnabled(false);
        ui_preset->comboBox_mode->setEnabled(false);
        ui_preset->comboBox_pass->setEnabled(false);
        ui_preset->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
    };
    if (arg1 == "ProRes 4444") {
        ui_preset->checkBox_2->setChecked(false);
        ui_preset->checkBox_2->setEnabled(false);
        ui_preset->comboBox_container->addItems({"MOV"});
        ui_preset->comboBox_mode->addItems({"Auto"});
        ui_preset->comboBox_pass->addItems({"Auto"});
        ui_preset->comboBox_profile->setCurrentIndex(5);
        ui_preset->comboBox_preset->addItems({"Auto"});
        ui_preset->comboBox_pixfmt->setCurrentIndex(0);
        ui_preset->comboBox_preset->setEnabled(false);
        ui_preset->comboBox_container->setEnabled(false);
        ui_preset->comboBox_mode->setEnabled(false);
        ui_preset->comboBox_pass->setEnabled(false);
        ui_preset->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
    };
    if (arg1 == "DNxHR HQX") {
        ui_preset->checkBox_2->setChecked(false);
        ui_preset->checkBox_2->setEnabled(false);
        ui_preset->comboBox_container->addItems({"MOV"});
        ui_preset->comboBox_mode->addItems({"Auto"});
        ui_preset->comboBox_pass->addItems({"Auto"});
        ui_preset->comboBox_profile->setCurrentIndex(6);
        ui_preset->comboBox_preset->addItems({"Auto"});
        ui_preset->comboBox_pixfmt->setCurrentIndex(1);
        ui_preset->comboBox_preset->setEnabled(false);
        ui_preset->comboBox_container->setEnabled(false);
        ui_preset->comboBox_mode->setEnabled(false);
        ui_preset->comboBox_pass->setEnabled(false);
        ui_preset->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
    };
    if (arg1 == "From source") {
        ui_preset->checkBox_2->setChecked(false);
        ui_preset->checkBox_2->setEnabled(false);
        ui_preset->comboBox_container->addItems({"MKV", "MOV", "MP4", "M2TS", "TS"});
        ui_preset->comboBox_container->setCurrentIndex(2);
        ui_preset->comboBox_mode->addItems({"Auto"});
        ui_preset->comboBox_pass->addItems({"Auto"});
        ui_preset->comboBox_profile->setCurrentIndex(7);
        ui_preset->comboBox_preset->addItems({"Auto"});
        ui_preset->comboBox_pixfmt->setCurrentIndex(5);
        ui_preset->comboBox_preset->setEnabled(false);
        ui_preset->comboBox_mode->setEnabled(false);
        ui_preset->comboBox_pass->setEnabled(false);
        ui_preset->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", "Vorbis", "Opus", "From source"});
    };
}

void Preset::on_checkBox_2_stateChanged(int arg1)  //************************************* Enable resize *******************//
{
    std::cout << "Change state resize..." << std::endl;
    if (arg1 == 2) {
        ui_preset->comboBox_width->setEnabled(true);
        ui_preset->comboBox_height->setEnabled(true);
    } else {
        ui_preset->comboBox_width->clearEditText();
        ui_preset->comboBox_height->clearEditText();
        ui_preset->comboBox_width->clear();
        ui_preset->comboBox_height->clear();
        ui_preset->comboBox_width->addItems({"Source"});
        ui_preset->comboBox_height->addItems({"Source"});
        ui_preset->comboBox_width->setEnabled(false);
        ui_preset->comboBox_height->setEnabled(false);
    };
    _repeat++;
}

void Preset::on_comboBox_mode_currentTextChanged(const QString &arg1)  //*************** Change curret mode ****************//
{
    std::cout << "Change current mode..." << std::endl;
    ui_preset->comboBox_bitrate->setEnabled(true);
    ui_preset->comboBox_maxrate->setEnabled(true);
    ui_preset->comboBox_bufsize->setEnabled(true);
    ui_preset->comboBox_bitrate->clear();
    ui_preset->comboBox_maxrate->clear();
    ui_preset->comboBox_bufsize->clear();
    if (arg1 == "Auto") {
        ui_preset->label_bitrate->setText("Bitrate");
        ui_preset->label_maxrate->setText("Maxrate");
        ui_preset->label_bufsize->setText("Bufsize");
        ui_preset->label_maxrate->show();
        ui_preset->label_bufsize->show();
        ui_preset->comboBox_maxrate->show();
        ui_preset->comboBox_bufsize->show();
        ui_preset->comboBox_bitrate->addItems({"Auto"});
        ui_preset->comboBox_maxrate->addItems({"Auto"});
        ui_preset->comboBox_bufsize->addItems({"Auto"});
        ui_preset->comboBox_bitrate->setEnabled(false);
        ui_preset->comboBox_maxrate->setEnabled(false);
        ui_preset->comboBox_bufsize->setEnabled(false);
    };
    if (arg1 == "Constant Bitrate") {
        ui_preset->label_bitrate->setText("Bitrate");
        ui_preset->label_maxrate->hide();
        ui_preset->label_bufsize->hide();
        ui_preset->comboBox_maxrate->hide();
        ui_preset->comboBox_bufsize->hide();
        ui_preset->comboBox_bitrate->addItems({"65M", "60M", "55M", "50M", "40M", "30M", "20M", "10M", "5M"});
        ui_preset->comboBox_bitrate->setCurrentIndex(3);
    };
    if (arg1 == "Average Bitrate") {
        ui_preset->label_bitrate->setText("Bitrate");
        ui_preset->label_maxrate->hide();
        ui_preset->label_bufsize->hide();
        ui_preset->comboBox_maxrate->hide();
        ui_preset->comboBox_bufsize->hide();
        ui_preset->comboBox_bitrate->addItems({"65M", "60M", "55M", "50M", "40M", "30M", "20M", "10M", "5M"});
        ui_preset->comboBox_bitrate->setCurrentIndex(3);
    };
    if (arg1 == "Variable Bitrate") {
        ui_preset->label_bitrate->setText("Bitrate");
        ui_preset->label_maxrate->setText("Maxrate");
        ui_preset->label_bufsize->setText("Bufsize");
        ui_preset->label_maxrate->show();
        ui_preset->label_bufsize->show();
        ui_preset->comboBox_maxrate->show();
        ui_preset->comboBox_bufsize->show();
        ui_preset->comboBox_bitrate->addItems({"65M", "60M", "55M", "50M", "40M", "30M", "20M", "10M", "5M"});
        ui_preset->comboBox_maxrate->addItems({"65M", "60M", "55M", "50M", "40M", "30M", "20M", "10M", "5M"});
        ui_preset->comboBox_bufsize->addItems({"65M", "60M", "55M", "50M", "40M", "30M", "20M", "10M", "5M"});
        ui_preset->comboBox_bitrate->setCurrentIndex(3);
        ui_preset->comboBox_maxrate->setCurrentIndex(3);
        ui_preset->comboBox_bufsize->setCurrentIndex(3);
    };
    if (arg1 == "Constant Rate Factor") {
        ui_preset->label_bitrate->setText("Rate factor");
        ui_preset->label_maxrate->hide();
        ui_preset->label_bufsize->hide();
        ui_preset->comboBox_maxrate->hide();
        ui_preset->comboBox_bufsize->hide();
        ui_preset->comboBox_bitrate->addItems({"51", "45", "40", "35", "30", "25", "24", "23", "22", "21", "20",
                                               "19", "18", "17", "16", "15", "10", "5", "1"});
        ui_preset->comboBox_bitrate->setCurrentIndex(10);
    };
    if (arg1 == "Constant QP") {
        ui_preset->label_bitrate->setText("Quantizer");
        ui_preset->label_maxrate->hide();
        ui_preset->label_bufsize->hide();
        ui_preset->comboBox_maxrate->hide();
        ui_preset->comboBox_bufsize->hide();
        ui_preset->comboBox_bitrate->addItems({"51", "45", "40", "35", "30", "25", "24", "23", "22", "21", "20",
                                               "19", "18", "17", "16", "15", "10", "5", "1"});
        ui_preset->comboBox_bitrate->setCurrentIndex(10);
    };
}

void Preset::on_comboBox_audio_codec_currentTextChanged(const QString &arg1) //***** Change current audio codec ************//
{
    std::cout << "Change current audio codec..." << std::endl;
    ui_preset->comboBox_audio_bitrate->setEnabled(true);
    ui_preset->comboBox_audio_bitrate->clear();
    if (arg1 == "From source") {
        ui_preset->comboBox_audio_bitrate->addItems({"From source"});
        ui_preset->comboBox_audio_bitrate->setEnabled(false);
    };
    if (arg1 == "AAC") {
        ui_preset->comboBox_audio_bitrate->addItems({"384k", "320k", "256k", "192k", "128k", "96k"});
        ui_preset->comboBox_audio_bitrate->setCurrentIndex(2);
    };
    if (arg1 == "AC3") {
        ui_preset->comboBox_audio_bitrate->addItems({"640k", "448k", "384k", "256k"});
        ui_preset->comboBox_audio_bitrate->setCurrentIndex(1);
    };
    if (arg1 == "DTS") {
        ui_preset->comboBox_audio_bitrate->addItems({"3840k", "3072k", "2048k", "1920k", "1536k", "1472k", "1344k", "1280k", "1152k",
                                                     "1024k", "960k", "768k", "640k", "576k", "512k", "448k", "384k"});
        ui_preset->comboBox_audio_bitrate->setCurrentIndex(4);
    };
    if (arg1 == "Vorbis") {
        ui_preset->comboBox_audio_bitrate->addItems({"448k", "384k", "256k", "128k", "96k", "64k"});
        ui_preset->comboBox_audio_bitrate->setCurrentIndex(2);
    };
    if (arg1 == "Opus") {
        ui_preset->comboBox_audio_bitrate->addItems({"448k", "384k", "256k", "128k", "96k", "64k"});
        ui_preset->comboBox_audio_bitrate->setCurrentIndex(2);
    };
    if (arg1 == "PCM 16 bit") {
        ui_preset->comboBox_audio_bitrate->addItems({"Auto"});
        ui_preset->comboBox_audio_bitrate->setEnabled(false);
    };
    if (arg1 == "PCM 24 bit") {
        ui_preset->comboBox_audio_bitrate->addItems({"Auto"});
        ui_preset->comboBox_audio_bitrate->setEnabled(false);
    };
    if (arg1 == "PCM 32 bit") {
        ui_preset->comboBox_audio_bitrate->addItems({"Auto"});
        ui_preset->comboBox_audio_bitrate->setEnabled(false);
    };
    _repeat++;
}

void Preset::on_comboBox_master_disp_currentTextChanged(const QString &arg1)  //*** Change current master display **********//
{
    std::cout << "Change current master display..." << std::endl;
    ui_preset->lineEdit_chroma_coord->clear();
    ui_preset->lineEdit_white_coord->clear();
    ui_preset->lineEdit_chroma_coord->setEnabled(true);
    ui_preset->lineEdit_white_coord->setEnabled(true);
    if (arg1 == "Unsprt") {
        ui_preset->lineEdit_chroma_coord->setText("Unsprt");
        ui_preset->lineEdit_white_coord->setText("Unsprt");
        ui_preset->lineEdit_chroma_coord->setEnabled(false);
        ui_preset->lineEdit_white_coord->setEnabled(false);
    };
    if (arg1 == "From source") {
        ui_preset->lineEdit_chroma_coord->setText("From source");
        ui_preset->lineEdit_white_coord->setText("From source");
        ui_preset->lineEdit_chroma_coord->setEnabled(false);
        ui_preset->lineEdit_white_coord->setEnabled(false);
    };
    if (arg1 == "Custom") {
        ui_preset->lineEdit_chroma_coord->setText("0.680,0.320,0.265,0.690,0.150,0.060");
        ui_preset->lineEdit_white_coord->setText("0.3127,0.3290");
    };
    if (arg1 == "Display P3") {
        ui_preset->lineEdit_chroma_coord->setText("0.680,0.320,0.265,0.690,0.150,0.060");
        ui_preset->lineEdit_white_coord->setText("0.3127,0.3290");
        ui_preset->lineEdit_chroma_coord->setEnabled(false);
        ui_preset->lineEdit_white_coord->setEnabled(false);
    };
    if (arg1 == "DCI P3") {
        ui_preset->lineEdit_chroma_coord->setText("0.680,0.320,0.265,0.690,0.150,0.060");
        ui_preset->lineEdit_white_coord->setText("0.314,0.3510");
        ui_preset->lineEdit_chroma_coord->setEnabled(false);
        ui_preset->lineEdit_white_coord->setEnabled(false);
    };
    if (arg1 == "BT.2020") {
        ui_preset->lineEdit_chroma_coord->setText("0.708,0.292,0.170,0.797,0.131,0.046");
        ui_preset->lineEdit_white_coord->setText("0.3127,0.3290");
        ui_preset->lineEdit_chroma_coord->setEnabled(false);
        ui_preset->lineEdit_white_coord->setEnabled(false);
    };
    if (arg1 == "BT.709") {
        ui_preset->lineEdit_chroma_coord->setText("0.640,0.330,0.30,0.60,0.150,0.060");
        ui_preset->lineEdit_white_coord->setText("0.3127,0.3290");
        ui_preset->lineEdit_chroma_coord->setEnabled(false);
        ui_preset->lineEdit_white_coord->setEnabled(false);
    };
}

void Preset::on_pushButton_6_clicked()  //************************************************* Apply preset *******************//
{
    _cur_param[0] = ui_preset->lineEdit_presetname->text();
    _cur_param[1] = QString::number(ui_preset->comboBox_codec->currentIndex());
    _cur_param[2] = QString::number(ui_preset->comboBox_mode->currentIndex());
    _cur_param[3] = QString::number(ui_preset->comboBox_container->currentIndex());
    _cur_param[4] = ui_preset->comboBox_bitrate->currentText();
    _cur_param[5] = ui_preset->comboBox_maxrate->currentText();
    _cur_param[6] = ui_preset->comboBox_bufsize->currentText();
    _cur_param[7] = QString::number(int(ui_preset->checkBox_2->isEnabled()));
    _cur_param[8] = QString::number(ui_preset->checkBox_2->checkState());
    _cur_param[9] = ui_preset->comboBox_width->currentText();
    _cur_param[10] = ui_preset->comboBox_height->currentText();
    _cur_param[11]= QString::number(ui_preset->comboBox_pass->currentIndex());
    _cur_param[12] = QString::number(ui_preset->comboBox_preset->currentIndex());
    _cur_param[13] = QString::number(ui_preset->comboBox_color_range->currentIndex());
    _cur_param[14] = ui_preset->lineEdit_min_lum->text();
    _cur_param[15] = ui_preset->lineEdit_max_lum->text();
    _cur_param[16] = ui_preset->lineEdit_max_cll->text();
    _cur_param[17] = ui_preset->lineEdit_max_fall->text();
    _cur_param[18] = QString::number(ui_preset->comboBox_master_disp->currentIndex());
    _cur_param[19] = ui_preset->lineEdit_chroma_coord->text();
    _cur_param[20] = ui_preset->lineEdit_white_coord->text();
    _cur_param[21] = QString::number(ui_preset->comboBox_audio_codec->currentIndex());
    _cur_param[22] = QString::number(ui_preset->comboBox_audio_bitrate->currentIndex());
    this->close();
}

void Preset::on_pushButton_7_clicked()  //********************************************* Close preset window ****************//
{
    this->close();
}

void Preset::change_preset_name()  // Call Change preset name
{
    std::cout << "Call change preset name..." << std::endl;
    int c1 = ui_preset->comboBox_codec->currentIndex();
    int c2 = ui_preset->comboBox_mode->currentIndex();
    int c11 = ui_preset->comboBox_pass->currentIndex();
    int c12 = ui_preset->comboBox_preset->currentIndex();
    int c21 = ui_preset->comboBox_audio_codec->currentIndex();
    int c22 = ui_preset->comboBox_audio_bitrate->currentIndex();
    QString w = ui_preset->comboBox_width->currentText();
    QString h = ui_preset->comboBox_height->currentText();
    QString container = ui_preset->comboBox_container->currentText();
    QString codec, res, mode, clrspace,preset, pass, hdr, acodec, abitrate;
    QString fps = "Fps: Source, ";
    if ((c1 == -1) or (c2 == -1) or (c11 == -1) or (c12 == -1) or (c21 == -1) or (c22 == -1)) {
        return;
    };
    QString arr_codec[12][3] = {
        {"HEVC, ",        "YUV, 4:2:0, 10 bit, ", "HDR: "},
        {"HEVC, ",        "YUV, 4:2:0, 8 bit, ",  ""},
        {"AVC, ",         "YUV, 4:2:0, 8 bit, ",  ""},
        {"HEVC, NVENC, ", "YUV, 4:2:0, 10 bit, ", "HDR: "},
        {"HEVC, NVENC, ", "YUV, 4:2:0, 8 bit, ",  ""},
        {"AVC, NVENC,  ", "YUV, 4:2:0, 8 bit, ",  ""},
        {"VP9, ",         "YUV, 4:2:0, 10 bit, ", "HDR: "},
        {"AV1, ",         "YUV, 4:2:0, 10 bit, ", "HDR: "},
        {"ProRes HQ, ",   "YUV, 4:2:2, 10 bit, ", "HDR: "},
        {"ProRes 4444, ", "YUV, 4:4:4, 10 bit, ", "HDR: "},
        {"DNxHR HQX, ",   "YUV, 4:2:2, 10 bit, ", "HDR: "},
        {"From source, ", "",                     "HDR: "}
    };
    codec = arr_codec[c1][0];
    clrspace = arr_codec[c1][1];
    if (w == "" or h == "") {
        res = "Res: Source, ";
    } else if (w == "Source" or h == "Source") {
        res = "Res: Source, ";
    } else if (w != "Source" && h != "Source") {
        res = "Res: " + w + "x" + h + ", ";
    };
    QString arr_mode[12][5] = {
        {"CBR", "ABR", "VBR", "CRF", "CQP"},
        {"CBR", "ABR", "VBR", "CRF", "CQP"},
        {"CBR", "ABR", "VBR", "CRF", "CQP"},
        {"VBR", "",    "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"ABR", "CRF", "",    "",    ""},
        {"ABR", "CRF", "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""}
    };
    if (arr_mode[c1][c2] != "") {
        mode = arr_mode[c1][c2] + " " + ui_preset->comboBox_bitrate->currentText() + ", ";
    };
    QString arr_preset[12][9] = {
        {"Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow", "Slower", "Veryslow"},
        {"Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow", "Slower", "Veryslow"},
        {"Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow", "Slower", "Veryslow"},
        {"Slow",      "",          "",         "",       "",     "",       "",     "",       ""},
        {"Slow",      "",          "",         "",       "",     "",       "",     "",       ""},
        {"Slow",      "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""}
    };
    if (arr_preset[c1][c12] != "") {
        preset = "Preset: " + arr_preset[c1][c12] + ", ";
    };
    QString arr_pass[12][2] = {
        {"1 Pass", "2 Pass"},
        {"1 Pass", "2 Pass"},
        {"1 Pass", "2 Pass"},
        {"2 Pass", ""},
        {"2 Pass", ""},
        {"2 Pass", ""},
        {"1 Pass", "2 Pass"},
        {"1 Pass", ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""}
    };
    if (arr_pass[c1][c11] != "") {
        pass = arr_pass[c1][c11] + ", ";
    };
    if (arr_codec[c1][2] != "") {
        hdr = arr_codec[c1][2] + "Enabled, ";
    };
    QString arr_acodec[12][6] = {
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"Opus",       "Vorbis",     "Source",     "",       "",     ""},
        {"Opus",       "Vorbis",     "Source",     "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"AAC",        "AC3",        "DTS",        "Vorbis", "Opus", "Source"}
    };
    if (arr_acodec[c1][c21] != "") {
        acodec = "Audio: " + arr_acodec[c1][c21] + ", ";
    };
    QString arr_bitrate[5][17] = {
        {"384k",  "320k",  "256k",  "192k",  "128k",  "96k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AAC
        {"640k",  "448k",  "384k",  "256k",  "",      "",      "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AC3
        {"3840k", "3072k", "2048k", "1920k", "1536k", "1472k", "1344k", "1280k", "1152k", "1024k", "960k", "768k", "640k", "576k", "512k", "448k", "384k"}, // DTS
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // Vorbis
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""} // Opus
    };
    if (arr_acodec[c1][c21] == "AAC") {
        abitrate = arr_bitrate[0][c22] + ", ";
    };
    if (arr_acodec[c1][c21] == "AC3") {
        abitrate = arr_bitrate[1][c22] + ", ";
    };
    if (arr_acodec[c1][c21] == "DTS") {
        abitrate = arr_bitrate[2][c22] + ", ";
    };
    if (arr_acodec[c1][c21] == "Vorbis") {
        abitrate = arr_bitrate[3][c22] + ", ";
    };
    if (arr_acodec[c1][c21] == "Opus") {
        abitrate = arr_bitrate[4][c22] + ", ";
    };
    QString cur_preset_name = codec + res + fps + clrspace + mode + preset + pass + hdr + acodec + abitrate + container;
    ui_preset->lineEdit_presetname->setText(cur_preset_name);
}

void Preset::on_comboBox_container_currentTextChanged()  // Signal Container currentText changed
{
    _repeat++;
}

void Preset::on_comboBox_bitrate_currentTextChanged()  // Signal Bitrate currentText changed
{
    _repeat++;
}

void Preset::on_comboBox_bitrate_editTextChanged()  // Signal Bitrate editText changed
{
    _repeat++;
}

void Preset::on_comboBox_width_editTextChanged()  // Signal Width editText changed
{
    _repeat++;
}

void Preset::on_comboBox_height_editTextChanged()  // Signal Height editText changed
{
    _repeat++;
}

void Preset::on_comboBox_preset_currentIndexChanged(int index) // Signal Preset index changed
{
    std::cout << "Index of Preset: " << index << std::endl;
    _repeat++;
}

void Preset::on_comboBox_pass_currentIndexChanged(int index)  // Signal Pass index changed
{
    std::cout << "Index of Pass: " << index << std::endl;
    _repeat++;
}

void Preset::on_comboBox_audio_bitrate_currentTextChanged()  // Signal Abitrate currentText changed
{
    _repeat++;
}

void Preset::repeat_handler()  // Repeat handler
{
    std::cout << "Call by timer... Repeat count: " << _repeat << std::endl;
    if (_repeat > 0) {
        _repeat = 0;
        change_preset_name();
    };
};


