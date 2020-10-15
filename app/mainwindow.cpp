#define _UNICODE
#include <QFileDialog>
#include <QPixmap>
#include <QProcess>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <math.h>
#include <MediaInfo/MediaInfo.h>
//#include <libavcodec/avcodec.h>
//#include <libde265/de265.h>
//#include <x265.h>
//#include <x264.h>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "settings.h"
#include "smartsettings.h"
#include "taskcomplete.h"

using namespace MediaInfoLib;

QProcess *procedure_1 = new QProcess;
QProcess *procedure_2 = new QProcess;
QProcess *procedure_3 = new QProcess;
QProcess *procedure_4 = new QProcess;
QProcess *procedure_5 = new QProcess;
QString _message = "";
QString input_file = "";
QString output_file = "";
QString temp_folder = "";
QString temp_file = "";
QString fmt = "";
QString width_ = "";
QString height_ = "";
QString fps = "";
QString stream_size = "";
QString hdr1, hdr2, hdr3, hdr4, hdr5, hdr6, hdr7, hdr8, hdr9, hdr10, hdr11;
QString preset_0, preset, preset_mkvmerge;
bool calling_pr_1, calling_pr_2, calling_pr_3, calling_pr_4;
float dur = 0;
float k;
int fr_count = 0;
int loop_start;
extern bool flag_no_smart;
extern int BR;
extern int MR;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_53->hide();
    ui->label_54->hide();
    ui->label_55->hide();
    ui->progressBar->hide();
    procedure_1->setProcessChannelMode(QProcess::MergedChannels);
    procedure_2->setProcessChannelMode(QProcess::MergedChannels);
    procedure_3->setProcessChannelMode(QProcess::MergedChannels);
    procedure_4->setProcessChannelMode(QProcess::MergedChannels);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionAbout_clicked()  // About
{
    About about;
    about.setModal(true);
    about.exec();
}

void MainWindow::on_actionSettings_clicked()  // Settings
{
    Settings settings;
    settings.setModal(true);
    settings.exec();
}

void MainWindow::on_turnRightStack_1_clicked()
{
    ui->stackedWidget_1->setCurrentIndex(1);
}

void MainWindow::on_turnLeftStack_1_clicked()
{
    ui->stackedWidget_1->setCurrentIndex(0);
}

void MainWindow::on_turnRightStack_2_clicked()
{
    int a = ui->stackedWidget_2->currentIndex();
    if (a != 3) {
        ui->stackedWidget_2->setCurrentIndex(a + 1);
    } else {
        ui->stackedWidget_2->setCurrentIndex(3);
    };
}

void MainWindow::on_turnLeftStack_2_clicked()
{
    int a = ui->stackedWidget_2->currentIndex();
    if (a != 0) {
        ui->stackedWidget_2->setCurrentIndex(a - 1);
    } else {
        ui->stackedWidget_2->setCurrentIndex(0);
    };
}

void MainWindow::on_pushButton_1_clicked()  // Open file
{
    ui->lineEdit_1->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->lineEdit_5->clear();
    ui->lineEdit_6->clear();
    ui->lineEdit_7->clear();
    ui->lineEdit_8->clear();
    ui->comboBox_9->clear();
    ui->comboBox_10->clear();
    ui->comboBox_11->clear();
    ui->comboBox_15->clear();
    ui->comboBox_16->clear();
    ui->label_format->setText("n/a");
    ui->label_duration->setText("n/a");
    ui->label_resolution->setText("n/a");
    ui->label_subsampling->setText("n/a");
    ui->label_bitrate->setText("n/a");
    ui->label_fps->setText("n/a");
    ui->label_aspect_ratio->setText("n/a");
    ui->label_color_space->setText("n/a");
    ui->label_bit_depth->setText("n/a");
    ui->label_audio_1->clear();
    ui->label_audio_2->clear();
    ui->label_audio_3->clear();
    ui->label_audio_4->clear();
    ui->label_audio_5->clear();
    ui->label_audio_6->clear();
    ui->label_audio_7->clear();
    ui->label_audio_8->clear();
    ui->label_audio_9->clear();
    ui->label_53->hide();
    ui->label_54->hide();
    ui->label_55->hide();
    ui->progressBar->hide();
    QString file_name_open;
    try {
        file_name_open = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::currentPath(), tr("Video Files: *.avi, "
                                                                                                     "*.m2ts, *.m4v, *.mkv, "
                                                                                                     "*.mov, *.mp4, *.mpeg, "
                                                                                                     "*.mpg, *.mxf, *.ts, "
                                                                                                     "*.webm (*.avi *.m2ts "
                                                                                                     "*.m4v *.mkv *.mov "
                                                                                                     "*.mp4 *.mpeg *.mpg "
                                                                                                     "*.mxf *.ts *.webm);;All files (*.*)"));
    }  catch (...) {
        _message = "File cannot be opened.";
        Taskcomplete taskcomplete;
        taskcomplete.setModal(true);
        taskcomplete.exec();
        return;
    };
    if (file_name_open.isEmpty())
    {
        return;
    } else {
        QString file_name = file_name_open;
        ui->lineEdit_1->setText(file_name);
        MediaInfo MI;
        std::wstring file_wstr = file_name.toStdWString();
        MI.Open(file_wstr);
        std::wstring stream_size_wstr = MI.Get(Stream_Video, 0, L"StreamSize");
        std::wstring fmt_wstr = MI.Get(Stream_General, 0, L"Format");
        std::wstring dur_wstr = MI.Get(Stream_Video, 0, L"Duration");
        try {
            dur = 0.001 * std::stoi(dur_wstr);
        }  catch (...) {
            dur = 0;
            std::cout << "No duration info!" << std::endl;
        };
        int h = trunc(dur / 3600);
        int m = trunc((dur - (h * 3600)) / 60);
        int s = trunc(dur - (h * 3600) - (m * 60));
        std::wstring chroma_subsampling_wstr = MI.Get(Stream_Video, 0, L"ChromaSubsampling");
        std::wstring bitrate_wstr = MI.Get(Stream_Video, 0, L"BitRate");
        int bit_rate = 0;
        try {
            bit_rate = 0.001 * std::stoi(bitrate_wstr);
        }  catch (...) {
            std::cout << "No bitrate info!" << std::endl;
        };
        std::wstring aspect_ratio_wstr = MI.Get(Stream_Video, 0, L"DisplayAspectRatio");
        std::wstring color_space_wstr = MI.Get(Stream_Video, 0, L"ColorSpace");
        std::wstring bit_depth_wstr = MI.Get(Stream_Video, 0, L"BitDepth");
        std::wstring width_wstr = MI.Get(Stream_Video, 0, L"Width");
        std::wstring height_wstr = MI.Get(Stream_Video, 0, L"Height");
        std::wstring fps_wstr = MI.Get(Stream_Video, 0, L"FrameRate");
        std::wstring color_range_wstr = MI.Get(Stream_Video, 0, L"colour_range");
        std::wstring color_primaries_wstr = MI.Get(Stream_Video, 0, L"colour_primaries");
        std::wstring matrix_coefficients_wstr = MI.Get(Stream_Video, 0, L"matrix_coefficients");
        std::wstring transfer_characteristics_wstr = MI.Get(Stream_Video, 0, L"transfer_characteristics");
        std::wstring mastering_display_luminance_wstr = MI.Get(Stream_Video, 0, L"MasteringDisplay_Luminance");
        stream_size = QString::fromStdWString(stream_size_wstr);
        QString mastering_display_luminance_qstr = QString::fromStdWString(mastering_display_luminance_wstr);
        QString mastering_display_luminance_rep = mastering_display_luminance_qstr.replace("min: ", "").replace("max: ", "").replace(" cd/m2", "");
        QString min_luminance = mastering_display_luminance_rep.split(", ")[0];
        QString max_luminance = mastering_display_luminance_rep.replace(min_luminance, "").replace(", ", "");
        std::wstring maxCll_wstr = MI.Get(Stream_Video, 0, L"MaxCLL");
        std::wstring maxFall_wstr = MI.Get(Stream_Video, 0, L"MaxFALL");
        std::wstring mastering_display_color_primaries_wstr = MI.Get(Stream_Video, 0, L"MasteringDisplay_ColorPrimaries");
        int i = 0;
        int smplrt_int = 0;
        std::wstring format_wstr;
        std::wstring smplrt_wstr;
        QString format;
        QString smplrt = "";
        while (i <= 8) {
            format_wstr = MI.Get(Stream_Audio, i, L"Format");
            smplrt_wstr = MI.Get(Stream_Audio, i, L"SamplingRate");
            format = QString::fromStdWString(format_wstr);
            if (format == "") {
                break;
            };
            try {
                smplrt_int = std::stoi(smplrt_wstr) / 1000;
            }  catch (...) {
                smplrt_int = 0;
            };
            if (smplrt_int != 0) {
                smplrt = QString::number(smplrt_int);
            } else {
                smplrt = "";
            };
            if (i == 0) {
                ui->label_audio_1->setText(format + "  " + smplrt + " KHz");
            };
            if (i == 1) {
                ui->label_audio_2->setText(format + "  " + smplrt + " KHz");
            };
            if (i == 2) {
                ui->label_audio_3->setText(format + "  " + smplrt + " KHz");
            };
            if (i == 3) {
                ui->label_audio_4->setText(format + "  " + smplrt + " KHz");
            };
            if (i == 4) {
                ui->label_audio_5->setText(format + "  " + smplrt + " KHz");
            };
            if (i == 5) {
                ui->label_audio_6->setText(format + "  " + smplrt + " KHz");
            };
            if (i == 6) {
                ui->label_audio_7->setText(format + "  " + smplrt + " KHz");
            };
            if (i == 7) {
                ui->label_audio_8->setText(format + "  " + smplrt + " KHz");
            };
            if (i == 8) {
                ui->label_audio_9->setText(format + "  " + smplrt + " KHz");
            };
            i++;
        };
        QString color_primaries = QString::fromStdWString(color_primaries_wstr);
        QString matrix_coefficients = QString::fromStdWString(matrix_coefficients_wstr);
        QString transfer_characteristics = QString::fromStdWString(transfer_characteristics_wstr);
        QString mastering_display_color_primaries = QString::fromStdWString(mastering_display_color_primaries_wstr);
        QString color_range = QString::fromStdWString(color_range_wstr);
        QString maxCll = QString::fromStdWString(maxCll_wstr);
        QString maxFall = QString::fromStdWString(maxFall_wstr);
        width_ = QString::fromStdWString(width_wstr);
        height_ = QString::fromStdWString(height_wstr);
        QString h_qstr = QString::number(h);
        QString m_qstr = QString::number(m);
        QString s_qstr = QString::number(s);
        QString chroma_subsampling = QString::fromStdWString(chroma_subsampling_wstr);
        QString bit_rate_qstr = QString::number(bit_rate);
        QString aspect_ratio = QString::fromStdWString(aspect_ratio_wstr);
        QString color_space = QString::fromStdWString(color_space_wstr);
        QString bit_depth = QString::fromStdWString(bit_depth_wstr);
        fmt = QString::fromStdWString(fmt_wstr);
        fps = QString::fromStdWString(fps_wstr);
        ui->comboBox_9->addItems({color_primaries.replace(".", "")});
        ui->comboBox_10->addItems({matrix_coefficients.replace(" ", "").replace(".", "").replace("on-", "").replace("onstant", "")});
        ui->comboBox_11->addItems({transfer_characteristics.replace(".", "")});
        int len = mastering_display_color_primaries.length();
        if (len > 10) {
            QStringList mdcp = mastering_display_color_primaries.split(",");
            QString r = mdcp[0].replace("R: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0").replace("00", "0");
            QString g = mdcp[1].replace(" G: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0").replace("00", "0");
            QString b = mdcp[2].replace(" B: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0").replace("00", "0");
            QString w = mdcp[3].replace(" White point: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0").replace("00", "0");
            QString chr_coord = r + "," + g + "," + b;
            QString white_coord = w;
            hdr10 = chr_coord;
            hdr11 = white_coord;
            ui->comboBox_15->addItems({"Undefined", "Display P3", "DCI P3", "BT.2020", "BT.709"});
        } else {
            ui->comboBox_15->addItems({mastering_display_color_primaries, "Display P3", "DCI P3", "BT.2020", "BT.709"});
        };
        ui->comboBox_16->addItems({color_range, "Limited", "Full"});
        ui->lineEdit_3->setText(maxCll.replace(" cd/m2", ""));
        ui->lineEdit_4->setText(maxFall.replace(" cd/m2", ""));
        ui->lineEdit_5->setText(max_luminance);
        ui->lineEdit_6->setText(min_luminance);
        ui->label_format->setText(fmt);
        ui->label_resolution->setText(width_ + "x" + height_);
        ui->label_duration->setText(h_qstr + "h " + m_qstr + "m " + s_qstr + "s");
        ui->label_fps->setText(fps);
        ui->label_subsampling->setText(chroma_subsampling);
        ui->label_bitrate->setText(bit_rate_qstr + " kbps");
        ui->label_aspect_ratio->setText(aspect_ratio);
        ui->label_color_space->setText(color_space);
        ui->label_bit_depth->setText(bit_depth + " bit");
        hdr1 = ui->comboBox_16->currentText();
        hdr2 = ui->comboBox_9->currentText();
        hdr3 = ui->comboBox_10->currentText();
        hdr4 = ui->comboBox_11->currentText();
        hdr5 = ui->lineEdit_5->text();
        hdr6 = ui->lineEdit_6->text();
        hdr7 = ui->lineEdit_3->text();
        hdr8 = ui->lineEdit_4->text();
        hdr9 = ui->comboBox_15->currentText();
        QStringList cmd;
        QString tmb_file = QDir::homePath() + "/.ce_thumbnail.jpg";
        cmd << "-hide_banner" << "-ss" << "5" << "-i" << file_name << "-vframes" << "1" << "-q:v" << "5" << "-y" << tmb_file;
        procedure_5->start("ffmpeg", cmd);
        procedure_5->waitForFinished();
        QPixmap pixmap = QPixmap(tmb_file);
        ui->labelThumb->setPixmap(pixmap);
        MI.Close();
    };
}

void MainWindow::on_pushButton_2_clicked()  // Save file
{
    ui->lineEdit_2->clear();
    QString prefix = ui->comboBox_2->currentText();
    QString file_name_save = QFileDialog::getSaveFileName(this, tr("Save As"), "Untitled." + prefix.toLower(), tr("All Files (*.*)"));
    ui->lineEdit_2->setText(file_name_save);
    ui->lineEdit_2->end(false);
}

void MainWindow::on_comboBox_2_currentTextChanged()  // Change prefix
{
    QString prefix = ui->comboBox_2->currentText();
    if (ui->lineEdit_2->text() != "") {
        QString file_name_save = ui->lineEdit_2->text();
        int pos_ext = file_name_save.lastIndexOf(".");
        int len_str = file_name_save.length();
        if (pos_ext == -1) {
            ui->lineEdit_2->setText(file_name_save + "." + prefix.toLower());
        } else {
            if ((len_str - pos_ext) < 6) {
                ui->lineEdit_2->setText(file_name_save.left(pos_ext + 1) + prefix.toLower());
            } else {
                ui->lineEdit_2->setText(file_name_save + "." + prefix.toLower());
            };
        };
    };
}

void MainWindow::on_pushButton_3_clicked() // smart_detect
{
    input_file = ui->lineEdit_1->text();
    output_file = ui->lineEdit_2->text();
    if ((input_file == "") or (output_file == "")) {
        _message = "Select first input and output file!";
        Taskcomplete taskcomplete;
        taskcomplete.setModal(true);
        taskcomplete.exec();
        return;
    };
    if ((ui->comboBox_5->currentText() == "Smart detect") or (ui->comboBox_6->currentText() == "Smart detect")) {
        long stream_size_int = stream_size.toLong();
        int width_int = width_.toInt();
        int height_int = height_.toInt();
        float fps_fl = fps.toFloat();
        float k_0 = (stream_size_int * 8) / (width_int * height_int * fps_fl * dur);
        int k_1 = round(k_0*1000);
        float k_2 = k_1;
        k = k_2/1000;
        if (k == 0) {
            _message = "It is impossible to determine the settings for the Smart detect. Select the bitrate settings manually.";
            Taskcomplete taskcomplete;
            taskcomplete.setModal(true);
            taskcomplete.exec();
            return;
        } else {
            SmartSettings smartsettings;
            smartsettings.setModal(true);
            smartsettings.exec();                                   // Calling smartsettings and wait for return //
            if (flag_no_smart == false) {
                return;
            };
            ui->comboBox_5->clear();
            ui->comboBox_6->clear();
            QString BR_qstr = QString::number(BR);
            QString MR_qstr = QString::number(MR);
            ui->comboBox_5->addItems({BR_qstr + "k", "Smart detect", "65M", "60M", "55M", "50M", "40M", "30M", "20M", "10M", "5M"});
            ui->comboBox_6->addItems({MR_qstr + "k", "Smart detect", "65M", "60M", "55M", "50M", "40M", "30M", "20M", "10M", "5M"});
            make_preset();
        };

    } else {
        make_preset();
    };
}

void MainWindow::on_comboBox_1_currentTextChanged() // settings_menu
{
    ui->comboBox_16->clear();
    ui->comboBox_9->clear();
    ui->comboBox_10->clear();
    ui->comboBox_11->clear();
    ui->lineEdit_5->clear();
    ui->lineEdit_6->clear();
    ui->lineEdit_3->clear();
    ui->lineEdit_4->clear();
    ui->comboBox_15->clear();
    ui->comboBox_2->setEnabled(true);
    ui->comboBox_3->setEnabled(true);
    ui->comboBox_4->setEnabled(true);
    ui->comboBox_12->setEnabled(true);
    ui->comboBox_2->clear();
    ui->comboBox_3->clear();
    ui->comboBox_4->clear();
    ui->comboBox_12->clear();
    //try:
    if (ui->comboBox_1->currentText() == "H265  10 bit") {
        ui->comboBox_2->addItems({"MKV", "MOV", "MP4"});
        ui->comboBox_3->addItems({"CPU", "NVENC"});
        ui->comboBox_4->addItems({"VBR", "CQP"});
        ui->comboBox_12->addItems({"AAC", "AC3", "DTS", "From source"});
        ui->comboBox_16->addItems({hdr1, "Limited", "Full"});
        ui->comboBox_9->addItems({hdr2});
        ui->comboBox_10->addItems({hdr3});
        ui->comboBox_11->addItems({hdr4});
        ui->lineEdit_5->setText(hdr5);
        ui->lineEdit_6->setText(hdr6);
        ui->lineEdit_3->setText(hdr7);
        ui->lineEdit_4->setText(hdr8);
        ui->comboBox_15->addItems({hdr9, "Display P3", "DCI P3", "BT.2020", "BT.709"});
    };
    if (ui->comboBox_1->currentText() == "H265  8 bit") {
        ui->comboBox_2->addItems({"MKV", "MOV", "MP4", "M2TS", "TS"});
        ui->comboBox_3->addItems({"CPU", "NVENC"});
        ui->comboBox_4->addItems({"VBR", "CQP"});
        ui->comboBox_12->addItems({"AAC", "AC3", "DTS", "From source"});
        ui->comboBox_16->addItems({"Unsprt"});
        ui->comboBox_9->addItems({"Unsprt"});
        ui->comboBox_10->addItems({"Unsprt"});
        ui->comboBox_11->addItems({"Unsprt"});
        ui->lineEdit_5->setText("Unsprt");
        ui->lineEdit_6->setText("Unsprt");
        ui->lineEdit_3->setText("Unsprt");
        ui->lineEdit_4->setText("Unsprt");
        ui->comboBox_15->addItems({"Unsprt"});
    };
    if (ui->comboBox_1->currentText() == "H264  8 bit") {
        ui->comboBox_2->addItems({"MKV", "MOV", "MP4", "M2TS", "TS"});
        ui->comboBox_3->addItems({"CPU", "NVENC"});
        ui->comboBox_4->addItems({"VBR", "CQP"});
        ui->comboBox_12->addItems({"AAC", "AC3", "DTS", "From source"});
        ui->comboBox_16->addItems({"Unsprt"});
        ui->comboBox_9->addItems({"Unsprt"});
        ui->comboBox_10->addItems({"Unsprt"});
        ui->comboBox_11->addItems({"Unsprt"});
        ui->lineEdit_5->setText("Unsprt");
        ui->lineEdit_6->setText("Unsprt");
        ui->lineEdit_3->setText("Unsprt");
        ui->lineEdit_4->setText("Unsprt");
        ui->comboBox_15->addItems({"Unsprt"});
    };
    if (ui->comboBox_1->currentText() == "VP9  10 bit") {
        ui->comboBox_2->addItems({"WebM", "MKV"});
        ui->comboBox_3->addItems({"CPU"});
        ui->comboBox_4->addItems({"ABR", "CRF"});
        ui->comboBox_12->addItems({"Opus", "Vorbis", "From source"});
        ui->comboBox_3->setEnabled(false);
        ui->comboBox_16->addItems({hdr1, "Limited", "Full"});
        ui->comboBox_9->addItems({hdr2});
        ui->comboBox_10->addItems({hdr3});
        ui->comboBox_11->addItems({hdr4});
        ui->lineEdit_5->setText(hdr5);
        ui->lineEdit_6->setText(hdr6);
        ui->lineEdit_3->setText(hdr7);
        ui->lineEdit_4->setText(hdr8);
        ui->comboBox_15->addItems({hdr9, "Display P3", "DCI P3", "BT.2020", "BT.709"});
    };
    if (ui->comboBox_1->currentText() == "AV1  10 bit") {
        ui->comboBox_2->addItems({"WebM", "MKV"});
        ui->comboBox_3->addItems({"CPU"});
        ui->comboBox_4->addItems({"ABR", "CRF"});
        ui->comboBox_12->addItems({"Opus", "Vorbis", "From source"});
        ui->comboBox_3->setEnabled(false);
        ui->comboBox_16->addItems({hdr1, "Limited", "Full"});
        ui->comboBox_9->addItems({hdr2});
        ui->comboBox_10->addItems({hdr3});
        ui->comboBox_11->addItems({hdr4});
        ui->lineEdit_5->setText(hdr5);
        ui->lineEdit_6->setText(hdr6);
        ui->lineEdit_3->setText(hdr7);
        ui->lineEdit_4->setText(hdr8);
        ui->comboBox_15->addItems({hdr9, "Display P3", "DCI P3", "BT.2020", "BT.709"});
    };
    if (ui->comboBox_1->currentText() == "ProRes HQ" or ui->comboBox_1->currentText() == "ProRes 4444"
                                  or ui->comboBox_1->currentText() == "DNxHR HQX") {
        ui->comboBox_2->addItems({"MOV"});
        ui->comboBox_3->addItems({"CPU"});
        ui->comboBox_4->addItems({"Auto"});
        ui->comboBox_12->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
        ui->comboBox_2->setEnabled(false);
        ui->comboBox_3->setEnabled(false);
        ui->comboBox_4->setEnabled(false);
        // ui->comboBox_12->setEnabled(false);
        ui->comboBox_16->addItems({hdr1, "Limited", "Full"});
        ui->comboBox_9->addItems({hdr2});
        ui->comboBox_10->addItems({hdr3});
        ui->comboBox_11->addItems({hdr4});
        ui->lineEdit_5->setText(hdr5);
        ui->lineEdit_6->setText(hdr6);
        ui->lineEdit_3->setText(hdr7);
        ui->lineEdit_4->setText(hdr8);
        ui->comboBox_15->addItems({hdr9, "Display P3", "DCI P3", "BT.2020", "BT.709"});
    };
    if (ui->comboBox_1->currentText() == "From source") {
        ui->comboBox_2->addItems({"MKV", "MOV", "MP4", "M2TS", "TS"});
        ui->comboBox_3->addItems({"CPU"});
        ui->comboBox_4->addItems({"Auto"});
        ui->comboBox_12->addItems({"AAC", "AC3", "DTS", "Vorbis", "Opus", "From source"});
        ui->comboBox_3->setEnabled(false);
        ui->comboBox_4->setEnabled(false);
        ui->comboBox_16->addItems({hdr1, "Limited", "Full"});
        ui->comboBox_9->addItems({hdr2});
        ui->comboBox_10->addItems({hdr3});
        ui->comboBox_11->addItems({hdr4});
        ui->lineEdit_5->setText(hdr5);
        ui->lineEdit_6->setText(hdr6);
        ui->lineEdit_3->setText(hdr7);
        ui->lineEdit_4->setText(hdr8);
        ui->comboBox_15->addItems({hdr9, "Display P3", "DCI P3", "BT.2020", "BT.709"});
    };
    //except:
    //    pass
}

void MainWindow::on_comboBox_4_currentTextChanged() // settings_menu_mode
{
    ui->comboBox_5->setEnabled(true);
    ui->comboBox_6->setEnabled(true);
    ui->comboBox_5->clear();
    ui->comboBox_6->clear();
    if (ui->comboBox_4->currentText() == "Auto") {
        ui->label_credits_19->setText("Bitrate:");
        ui->label_credits_20->show();
        ui->comboBox_6->show();
        ui->comboBox_5->addItems({"Auto"});
        ui->comboBox_6->addItems({"Auto"});
        ui->comboBox_5->setEnabled(false);
        ui->comboBox_6->setEnabled(false);
    };
    if (ui->comboBox_4->currentText() == "CQP") {
        ui->label_credits_19->setText("Quant:");
        ui->label_credits_20->hide();
        ui->comboBox_6->hide();
        ui->comboBox_5->addItems({"51", "45", "40", "35", "30", "25", "24", "23", "22", "21", "20", "19", "18",
                                  "17", "16", "15", "10", "5", "1"});
        ui->comboBox_5->setCurrentIndex(10);
    };
    if (ui->comboBox_4->currentText() == "VBR") {
        ui->label_credits_19->setText("Bitrate:");
        ui->label_credits_20->show();
        ui->comboBox_6->show();
        ui->comboBox_5->addItems({"Smart detect", "65M", "60M", "55M", "50M", "40M", "30M", "20M", "10M", "5M"});
        ui->comboBox_6->addItems({"Smart detect", "65M", "60M", "55M", "50M", "40M", "30M", "20M", "10M", "5M"});
        ui->comboBox_5->setCurrentIndex(0);
        ui->comboBox_6->setCurrentIndex(0);
    };
    if (ui->comboBox_4->currentText() == "ABR") {
        ui->label_credits_19->setText("Bitrate:");
        ui->label_credits_20->hide();
        ui->comboBox_6->hide();
        ui->comboBox_5->addItems({"65M", "60M", "55M", "50M", "40M", "30M", "20M", "10M", "5M"});
        ui->comboBox_5->setCurrentIndex(3);
    };
    if (ui->comboBox_4->currentText() == "CRF") {
        ui->label_credits_19->setText("Quality:");
        ui->label_credits_20->hide();
        ui->comboBox_6->hide();
        ui->comboBox_5->addItems({"51", "45", "40", "35", "30", "25", "24", "23", "22", "21", "20", "19", "18",
                                  "17", "16", "15", "10", "5", "1"});
        ui->comboBox_5->setCurrentIndex(10);
    };
}

void MainWindow::on_comboBox_15_currentTextChanged() // settings_hdr
{
    ui->lineEdit_7->clear();
    ui->lineEdit_8->clear();
    if (ui->comboBox_15->currentText() == "Unsprt") {
        ui->lineEdit_7->setText("Unsprt");
        ui->lineEdit_8->setText("Unsprt");
    };
    if (ui->comboBox_15->currentText() == "Undefined") {
        ui->lineEdit_7->setText(hdr10);
        ui->lineEdit_8->setText(hdr11);
    };
    if (ui->comboBox_15->currentText() == "Display P3") {
        ui->lineEdit_7->setText("0.680,0.320,0.265,0.690,0.150,0.060");
        ui->lineEdit_8->setText("0.3127,0.3290");
    };
    if (ui->comboBox_15->currentText() == "DCI P3") {
        ui->lineEdit_7->setText("0.680,0.320,0.265,0.690,0.150,0.060");
        ui->lineEdit_8->setText("0.314,0.3510");
    };
    if (ui->comboBox_15->currentText() == "BT.2020") {
        ui->lineEdit_7->setText("0.708,0.292,0.170,0.797,0.131,0.046");
        ui->lineEdit_8->setText("0.3127,0.3290");
    };
    if (ui->comboBox_15->currentText() == "BT.709") {
        ui->lineEdit_7->setText("0.640,0.330,0.30,0.60,0.150,0.060");
        ui->lineEdit_8->setText("0.3127,0.3290");
    };
}

void MainWindow::on_comboBox_12_currentTextChanged() // settings_menu_audio
{
    ui->comboBox_13->setEnabled(true);
    ui->comboBox_13->clear();
    if (ui->comboBox_12->currentText() == "From source") {
        ui->comboBox_13->addItems({"From source"});
        ui->comboBox_13->setEnabled(false);
    };
    if (ui->comboBox_12->currentText() == "AAC") {
        ui->comboBox_13->addItems({"384k", "320k", "256k", "192k", "128k", "96k"});
        ui->comboBox_13->setCurrentIndex(2);
    };
    if (ui->comboBox_12->currentText() == "AC3") {
        ui->comboBox_13->addItems({"640k", "448k", "384k", "256k"});
        ui->comboBox_13->setCurrentIndex(1);
    };
    if (ui->comboBox_12->currentText() == "DTS") {
        ui->comboBox_13->addItems({"3840k", "3072k", "2048k", "1920k", "1536k", "1472k", "1344k", "1280k", "1152k",
                                   "1024k", "960k", "768k", "640k", "576k", "512k", "448k", "384k"});
        ui->comboBox_13->setCurrentIndex(4);
    };
    if (ui->comboBox_12->currentText() == "Vorbis") {
        ui->comboBox_13->addItems({"448k", "384k", "256k", "128k", "96k", "64k"});
        ui->comboBox_13->setCurrentIndex(2);
    };
    if (ui->comboBox_12->currentText() == "Opus") {
        ui->comboBox_13->addItems({"448k", "384k", "256k", "128k", "96k", "64k"});
        ui->comboBox_13->setCurrentIndex(2);
    };
    if (ui->comboBox_12->currentText() == "PCM 16 bit") {
        ui->comboBox_13->addItems({"Auto"});
        ui->comboBox_13->setEnabled(false);
    };
    if (ui->comboBox_12->currentText() == "PCM 24 bit") {
        ui->comboBox_13->addItems({"Auto"});
        ui->comboBox_13->setEnabled(false);
    };
    if (ui->comboBox_12->currentText() == "PCM 32 bit") {
        ui->comboBox_13->addItems({"Auto"});
        ui->comboBox_13->setEnabled(false);
    };
}

void MainWindow::make_preset() // make_preset
{
    std::cout << "Make preset..." << std::endl; // Debug information //
    procedure_1->disconnect();
    procedure_2->disconnect();
    procedure_3->disconnect();
    procedure_4->disconnect();
    calling_pr_1 = true;
    calling_pr_2 = true;
    calling_pr_3 = true;
    calling_pr_4 = true;
    QString apreset = "";
    QString vbitrate = ui->comboBox_5->currentText();
    QString maxrate = ui->comboBox_6->currentText();
    QString quality = ui->comboBox_5->currentText();
    QString color_range_curr = ui->comboBox_16->currentText();
    QString colorprim_curr = ui->comboBox_9->currentText().toLower();
    QString colormatrix_curr = ui->comboBox_10->currentText().toLower();
    QString transfer_curr = ui->comboBox_11->currentText();
    QString max_lum_curr = ui->lineEdit_5->text();
    QString min_lum_curr = ui->lineEdit_6->text();
    QString max_cll_curr = ui->lineEdit_3->text();
    QString max_fall_curr = ui->lineEdit_4->text();
    QString chroma_coord_curr = ui->lineEdit_7->text();
    QString white_coord_curr = ui->lineEdit_8->text();
    QString color_range = "";
    QString colorprim = "";
    QString colormatrix = "";
    QString transfer = "";
    QString max_lum = "";
    QString min_lum = "";
    QString max_cll = "";
    QString max_fall = "";
    QString chroma_coord = "";
    QString white_coord = "";

    if (color_range_curr == "Limited") {
        color_range = "-color_range tv ";
    };
    if (color_range_curr == "Full") {
        color_range = "-color_range pc ";
    };
    if (colorprim_curr == "bt709") {
        colorprim = "-color_primaries bt709 ";
    };
    if (colorprim_curr == "bt2020") {
        colorprim = "-color_primaries bt2020 ";
    };
    if (colormatrix_curr == "bt709") {
        colormatrix = "-colorspace bt709 ";
    };
    if (colormatrix_curr == "bt2020nc") {
        colormatrix = "-colorspace bt2020nc ";
    };
    if (colormatrix_curr == "bt2020c") {
        colormatrix = "-colorspace bt2020c ";
    };
    if (transfer_curr == "BT.709") {
        transfer = "-color_trc bt709 ";
    };
    if (transfer_curr == "PQ") {
        transfer = "-color_trc smpte2084 ";
    };
    if (transfer_curr == "HLG") {
        transfer = "-color_trc arib-std-b67 ";
    };
    if (transfer_curr == "BT.2020 (10-bit)") {
        transfer = "-color_trc bt2020-10 ";
    };
    if (max_lum_curr != "None") {
        max_lum = QString("-s max-luminance=%1 ").arg(max_lum_curr);
    };
    if ((max_lum_curr == "") or (max_lum_curr == "None")) {
        max_lum = "-d max-luminance ";
    };
    if (min_lum_curr != "None") {
        min_lum = QString("-s min-luminance=%1 ").arg(min_lum_curr);
    };
    if ((min_lum_curr == "") or (min_lum_curr == "None")) {
        min_lum = "-d min-luminance ";
    };
    if (max_cll_curr != "None") {
        max_cll = QString("-s max-content-light=%1 ").arg(max_cll_curr);
    };
    if ((max_cll_curr == "") or (max_cll_curr == "None")) {
        max_cll = "-d max-content-light ";
    };
    if (max_fall_curr != "None") {
        max_fall = QString("-s max-frame-light=%1 ").arg(max_fall_curr);
    };
    if ((max_fall_curr == "") or (max_fall_curr == "None")) {
        max_fall = "-d max-frame-light ";
    };
    if (chroma_coord_curr != "None") {
        QString chroma_coord_curr_red_x = "";
        QString chroma_coord_curr_red_y = "";
        QString chroma_coord_curr_green_x = "";
        QString chroma_coord_curr_green_y = "";
        QString chroma_coord_curr_blue_x = "";
        QString chroma_coord_curr_blue_y = "";
        QStringList chr = chroma_coord_curr.split(",");
        if (chr.size() == 6) {
            chroma_coord_curr_red_x = chr[0];
            chroma_coord_curr_red_y = chr[1];
            chroma_coord_curr_green_x = chr[2];
            chroma_coord_curr_green_y = chr[3];
            chroma_coord_curr_blue_x = chr[4];
            chroma_coord_curr_blue_y = chr[5];
            chroma_coord = QString("-s chromaticity-coordinates-red-x=%1 -s chromaticity-coordinates-red-y=%2 "
                                   "-s chromaticity-coordinates-green-x=%3 -s chromaticity-coordinates-green-y=%4 "
                                   "-s chromaticity-coordinates-blue-x=%5 -s chromaticity-coordinates-blue-y=%6 ")
                    .arg(chroma_coord_curr_red_x).arg(chroma_coord_curr_red_y).arg(chroma_coord_curr_green_x)
                    .arg(chroma_coord_curr_green_y).arg(chroma_coord_curr_blue_x).arg(chroma_coord_curr_blue_y);
        };
    };
    if ((chroma_coord_curr == "") or (chroma_coord_curr == "None")) {
        chroma_coord = "-d chromaticity-coordinates-red-x -d chromaticity-coordinates-red-y "
                       "-d chromaticity-coordinates-green-x -d chromaticity-coordinates-green-y "
                       "-d chromaticity-coordinates-blue-x -d chromaticity-coordinates-blue-y ";
    };
    if (white_coord_curr != "None") {
        QString white_coord_curr_x = "";
        QString white_coord_curr_y = "";
        QStringList wht = white_coord_curr.split(",");
        if (wht.size() == 2) {
            white_coord_curr_x = wht[0];
            white_coord_curr_y = wht[1];
            white_coord = QString("-s white-coordinates-x=%1 -s white-coordinates-y=%2 ").arg(white_coord_curr_x).arg(white_coord_curr_y);
        };
    };
    if ((white_coord_curr == "") or (white_coord_curr == "None")) {
        white_coord = "-d white-coordinates-x -d white-coordinates-y ";
    };

    QString abitrate = ui->comboBox_13->currentText();
    if (ui->comboBox_12->currentText() == "From source") {
        apreset = "-c:a copy";
    };
    if (ui->comboBox_12->currentText() == "AAC") {
        apreset = QString("-c:a aac -b:a %1").arg(abitrate);
    };
    if (ui->comboBox_12->currentText() == "AC3") {
        apreset = QString("-c:a ac3 -b:a %1").arg(abitrate);
    };
    if (ui->comboBox_12->currentText() == "DTS") {
        apreset = QString("-strict -2 -c:a dca -b:a %1 -ar 48000").arg(abitrate);
    };
    if (ui->comboBox_12->currentText() == "Vorbis") {
        apreset = QString("-c:a libvorbis -b:a %1").arg(abitrate);
    };
    if (ui->comboBox_12->currentText() == "Opus") {
        apreset = QString("-c:a libopus -b:a %1").arg(abitrate);
    };
    if (ui->comboBox_12->currentText() == "PCM 16 bit") {
        apreset = "-c:a pcm_s16le";
    };
    if (ui->comboBox_12->currentText() == "PCM 24 bit") {
        apreset = "-c:a pcm_s24le";
    };
    if (ui->comboBox_12->currentText() == "PCM 32 bit") {
        apreset = "-c:a pcm_s32le";
    };

    preset_mkvmerge = QString("%1%2%3%4%5%6 ").arg(max_cll).arg(max_fall).arg(max_lum).arg(min_lum).arg(chroma_coord).arg(white_coord);

    if ((ui->comboBox_1->currentText() == "H265  10 bit") and (ui->comboBox_3->currentText() == "NVENC")) {
        if (ui->comboBox_4->currentText() == "CQP") {
            preset = QString("-map_metadata -1 -map 0:v:0 -b:v 0 -pix_fmt p010le -c:v hevc_nvenc -preset slow "
                             "-rc vbr_hq -cq %1 -qmin %2 -qmax %3 -rc-lookahead 32 -profile:v main10 "
                             "%4%5%6%7-max_muxing_queue_size 1024 -map 0:a %8 -f matroska")
                    .arg(quality).arg(quality).arg(quality).arg(color_range).arg(colorprim).arg(colormatrix).arg(transfer).arg(apreset);
        };
        if (ui->comboBox_4->currentText() == "VBR") {
            preset = QString("-map_metadata -1 -map 0:v:0 -pix_fmt p010le -b:v %1 -maxrate:v %2 -bufsize %3 "
                             "-c:v hevc_nvenc -preset slow -rc vbr_hq -2pass 1 -rc-lookahead 32 -profile:v main10 "
                             "%4%5%6%7-max_muxing_queue_size 1024 -map 0:a %8 -f matroska")
                    .arg(vbitrate).arg(maxrate).arg(maxrate).arg(color_range).arg(colorprim).arg(colormatrix).arg(transfer).arg(apreset);
        };
        preset_0 = "-hide_banner -hwaccel cuda";
        encode_mux_file();
    };
    if ((ui->comboBox_1->currentText() == "H265  10 bit") and (ui->comboBox_3->currentText() == "CPU")) {
        if (ui->comboBox_4->currentText() == "CQP") {
            preset = QString("-map_metadata -1 -map 0:v:0 -b:v 0 -pix_fmt yuv420p10le -c:v libx265 -preset slow "
                             "-rc vbr_hq -cq %1 -qmin %2 -qmax %3 -rc-lookahead 32 -profile:v main10 "
                             "%4%5%6%7-max_muxing_queue_size 1024 -map 0:a %8 -f matroska")
                    .arg(quality).arg(quality).arg(quality).arg(color_range).arg(colorprim).arg(colormatrix).arg(transfer).arg(apreset);
        };
        if (ui->comboBox_4->currentText() == "VBR") {
            preset = QString("-map_metadata -1 -map 0:v:0 -pix_fmt yuv420p10le -b:v %1 -maxrate:v %2 -bufsize %3 "
                             "-c:v libx265 -preset slow -rc vbr_hq -rc-lookahead 32 -profile:v main10 "
                             "%4%5%6%7-max_muxing_queue_size 1024 -map 0:a %8 -f matroska")
                    .arg(vbitrate).arg(maxrate).arg(maxrate).arg(color_range).arg(colorprim).arg(colormatrix).arg(transfer).arg(apreset);
        };
        preset_0 = "-hide_banner";
        encode_mux_file();
    };
    if (ui->comboBox_1->currentText() == "VP9  10 bit") {
        if (ui->comboBox_4->currentText() == "CRF") {
            preset = QString("-map_metadata -1 -map 0:v:0 -pix_fmt yuv420p10le -c:v libvpx-vp9 -speed 4 "
                             "-profile:v 2 -crf %1 -b:v 0 %2%3%4%5-map 0:a %6 -f matroska")
                    .arg(quality).arg(color_range).arg(colorprim).arg(colormatrix).arg(transfer).arg(apreset);
        };
        if (ui->comboBox_4->currentText() == "ABR") {
            preset = QString("-map_metadata -1 -map 0:v:0 -pix_fmt yuv420p10le -c:v libvpx-vp9 -speed 4 "
                             "-profile:v 2 -b:v %1 %2%3%4%5-map 0:a %6 -f matroska")
                    .arg(vbitrate).arg(color_range).arg(colorprim).arg(colormatrix).arg(transfer).arg(apreset);
        };
        preset_0 = "-hide_banner";
        encode_mux_file();
    };
    if (ui->comboBox_1->currentText() == "AV1  10 bit") {
        if (ui->comboBox_4->currentText() == "CRF") {
            preset = QString("-map_metadata -1 -map 0:v:0 -c:v librav1e -crf %1 -b:v 0 -strict -2 "
                             "%2%3%4%5-map 0:a %6 -f matroska")
                    .arg(quality).arg(color_range).arg(colorprim).arg(colormatrix).arg(transfer).arg(apreset);
        };
        if (ui->comboBox_4->currentText() == "ABR") {
            preset = QString("-map_metadata -1 -map 0:v:0 -c:v librav1e -b:v %1 -strict -2 "
                             "%2%3%4%5-map 0:a %6 -f matroska")
                    .arg(vbitrate).arg(color_range).arg(colorprim).arg(colormatrix).arg(transfer).arg(apreset);
        };
        preset_0 = "-hide_banner";
        encode_mux_file();
    };
    if ((ui->comboBox_1->currentText() == "H265  8 bit") and (ui->comboBox_3->currentText() == "NVENC")) {
        if (ui->comboBox_4->currentText() == "CQP") {
            preset = QString("-map_metadata -1 -map 0:v:0 -b:v 0 -pix_fmt yuv420p -c:v hevc_nvenc -preset slow "
                             "-rc vbr_hq -cq %1 -qmin %2 -qmax %3 -rc-lookahead 32 -profile:v main "
                             "-max_muxing_queue_size 1024 -map 0:a %4 -f matroska")
                    .arg(quality).arg(quality).arg(quality).arg(apreset);
        };
        if (ui->comboBox_4->currentText() == "VBR") {
            preset = QString("-map_metadata -1 -map 0:v:0 -pix_fmt yuv420p -b:v %1 -maxrate:v %2 -bufsize %3 "
                             "-c:v hevc_nvenc -preset slow -rc vbr_hq -2pass 1 -rc-lookahead 32 "
                             "-profile:v main -max_muxing_queue_size 1024 -map 0:a %4 -f matroska")
                    .arg(vbitrate).arg(maxrate).arg(maxrate).arg(apreset);
        };
        preset_0 = "-hide_banner -hwaccel cuda";
        encode_file();
    };
    if ((ui->comboBox_1->currentText() == "H265  8 bit") and (ui->comboBox_3->currentText() == "CPU")) {
        if (ui->comboBox_4->currentText() == "CQP") {
            preset = QString("-map_metadata -1 -map 0:v:0 -b:v 0 -pix_fmt yuv420p -c:v libx265 -preset slow "
                             "-rc vbr_hq -cq %1 -qmin %2 -qmax %3 -rc-lookahead 32 -profile:v main "
                             "-max_muxing_queue_size 1024 -map 0:a %4 -f matroska")
                    .arg(quality).arg(quality).arg(quality).arg(apreset);
        };
        if (ui->comboBox_4->currentText() == "VBR") {
            preset = QString("-map_metadata -1 -map 0:v:0 -pix_fmt yuv420p -b:v %1 -maxrate:v %2 "
                             "-bufsize %3 -c:v libx265 -preset slow -rc vbr_hq -rc-lookahead 32 "
                             "-profile:v main -max_muxing_queue_size 1024 -map 0:a %4 -f matroska")
                    .arg(vbitrate).arg(maxrate).arg(maxrate).arg(apreset);
        };
        preset_0 = "-hide_banner";
        encode_file();
    };
    if ((ui->comboBox_1->currentText() == "H264  8 bit") and (ui->comboBox_3->currentText() == "NVENC")) {
        if (ui->comboBox_4->currentText() == "CQP") {
            preset = QString("-map_metadata -1 -map 0:v:0 -b:v 0 -pix_fmt yuv420p -c:v h264_nvenc -preset slow "
                             "-rc vbr_hq -cq %1 -qmin %2 -qmax %3 -rc-lookahead 32 -profile:v high "
                             "-max_muxing_queue_size 1024 -map 0:a %4 -f matroska")
                    .arg(quality).arg(quality).arg(quality).arg(apreset);
        };
        if (ui->comboBox_4->currentText() == "VBR") {
            preset = QString("-map_metadata -1 -map 0:v:0 -pix_fmt yuv420p -b:v %1 -maxrate:v %2 "
                             "-bufsize %3 -c:v h264_nvenc -preset slow -rc vbr_hq -2pass 1 -rc-lookahead 32 "
                             "-profile:v high -max_muxing_queue_size 1024 -map 0:a %4 -f matroska")
                    .arg(vbitrate).arg(maxrate).arg(maxrate).arg(apreset);
        };
        preset_0 = "-hide_banner -hwaccel cuda";
        encode_file();
    };
    if ((ui->comboBox_1->currentText() == "H264  8 bit") and (ui->comboBox_3->currentText() == "CPU")) {
        if (ui->comboBox_4->currentText() == "CQP") {
            preset = QString("-map_metadata -1 -map 0:v:0 -b:v 0 -pix_fmt yuv420p -c:v libx264 -preset slow "
                             "-rc vbr_hq -cq %1 -qmin %2 -qmax %3 -rc-lookahead 32 -profile:v high "
                             "-max_muxing_queue_size 1024 -map 0:a %4 -f matroska")
                    .arg(quality).arg(quality).arg(quality).arg(apreset);
        };
        if (ui->comboBox_4->currentText() == "VBR") {
            preset = QString("-map_metadata -1 -map 0:v:0 -pix_fmt yuv420p -b:v %1 -maxrate:v %2 "
                             "-bufsize %3 -c:v libx264 -preset slow -rc vbr_hq -2pass 1 -rc-lookahead 32 "
                             "-profile:v high -max_muxing_queue_size 1024 -map 0:a %4 -f matroska")
                    .arg(vbitrate).arg(maxrate).arg(maxrate).arg(apreset);
        };
        preset_0 = "-hide_banner";
        encode_file();
    };
    if (ui->comboBox_1->currentText() == "ProRes HQ") {
        preset = QString("-map 0:v:0 -pix_fmt yuv422p10le -c:v prores_ks -profile:v 3 %1%2%3%4-map 0:a:0 %5")
                .arg(color_range).arg(colorprim).arg(colormatrix).arg(transfer).arg(apreset);
        preset_0 = "-hide_banner";
        encode_mux_file();
    };
    if (ui->comboBox_1->currentText() == "ProRes 4444") {
        preset = QString("-map 0:v:0 -pix_fmt yuv444p10 -c:v prores_ks -profile:v 4 %1%2%3%4-map 0:a:0 %5")
                .arg(color_range).arg(colorprim).arg(colormatrix).arg(transfer).arg(apreset);
        preset_0 = "-hide_banner";
        encode_mux_file();
    };
    if (ui->comboBox_1->currentText() == "DNxHR HQX") {
        preset = QString("-map 0:v:0 -pix_fmt yuv422p10le -c:v dnxhd -profile:v dnxhr_hqx -bits_per_raw_sample 10 "
                         "%1%2%3%4-map 0:a:0 %5").arg(color_range).arg(colorprim).arg(colormatrix).arg(transfer).arg(apreset);
        preset_0 = "-hide_banner";
        encode_mux_file();
    };
    if (ui->comboBox_1->currentText() == "From source") {
        preset = QString("-map 0:v:0 -movflags +write_colr -c:v copy -map 0:a %1").arg(apreset);
        preset_0 = "-hide_banner";
        encode_mux_file();
    };
}

void MainWindow::encode_mux_file() // encode_mux_file
{
    std::cout << "Encode mux file ..." << std::endl;  //  Debug info //
    std::cout << "Preset: " << preset.toStdString() << std::endl;  //  Debug info //
    std::cout << "Preset mkvpropedit: " << preset_mkvmerge.toStdString() << std::endl;  //  Debug info //
    connect(procedure_1, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_1()));
    connect(procedure_1, SIGNAL(finished(int)), this, SLOT(error_1()));
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
                    temp_folder = output_file + "_temp";
                    temp_file = temp_folder + "/temp.mkv";
                };
                if ((a.find("temp_folder:") != std::string::npos) && (a.find("[default]") == std::string::npos)) {
                    QStringList b = line[0].split(":");
                    QString c = b[1].replace("[", "").replace("]\n", "");
                    temp_folder = c + "/_temp";
                    temp_file = temp_folder + "/temp.mkv";
                };
            };
        };
        f.close();
        if (j == 0) {
            _message = "Please set up a temporary folder!\n";
            Taskcomplete taskcomplete;
            taskcomplete.setModal(true);
            taskcomplete.exec();
            return;
        };
    } else {
        _message = "Please set up a temporary folder!\n";
        Taskcomplete taskcomplete;
        taskcomplete.setModal(true);
        taskcomplete.exec();
        return;
    };
    if (!QDir(temp_folder).exists()) {
        QDir().mkdir(temp_folder);
    };
    int dur_mod = round(dur);
    float fps_mod = fps.toFloat();
    fr_count = round(dur_mod * fps_mod);
    if (fr_count == 0) {
        _message = "Select the correct input and output file!";
        Taskcomplete taskcomplete;
        taskcomplete.setModal(true);
        taskcomplete.exec();
        return;
    };
    ui->pushButton_1->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->label_53->show();
    ui->label_53->setText("Encoding:");
    ui->label_54->show();
    ui->label_55->show();
    ui->progressBar->show();
    ui->progressBar->setValue(0);
    loop_start = time (NULL);
    QStringList arguments;
    arguments << preset_0.split(" ") << "-i" << input_file << preset.split(" ") << "-y" << temp_file;
    procedure_1->start("ffmpeg", arguments);
    if (!procedure_1->waitForStarted()) {
        std::cout << "cmd command not found!!!" << std::endl;
        disconnect(procedure_1, SIGNAL(finished(int)), this, SLOT(error_1()));
        _message = "An unknown error occurred!\n Possible FFMPEG not installed.\n";
        ui->pushButton_1->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        Taskcomplete taskcomplete;
        taskcomplete.setModal(true);
        taskcomplete.exec();
    };
}

void MainWindow::add_metadata()
{
    std::cout << "Add metadata ..." << std::endl;  //  Debug info //
    disconnect(procedure_1, SIGNAL(finished(int)), this, SLOT(add_metadata()));
    connect(procedure_2, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_2()));
    connect(procedure_2, SIGNAL(finished(int)), this, SLOT(error_2()));
    ui->label_53->setText("Add data:");
    ui->progressBar->setValue(0);
    QStringList arguments;
    arguments << "--edit" << "track:1" << preset_mkvmerge.split(" ") << temp_file;
    procedure_2->start("mkvpropedit", arguments);
    if (!procedure_2->waitForStarted()) {
        std::cout << "cmd command not found!!!" << std::endl;
        disconnect(procedure_2, SIGNAL(finished(int)), this, SLOT(error_2()));
        _message = "An unknown error occured!\n";
        ui->pushButton_1->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        Taskcomplete taskcomplete;
        taskcomplete.setModal(true);
        taskcomplete.exec();
    };
}

void MainWindow::mux() // mux
{
    std::cout << "Muxing ..." << std::endl;  //  Debug info //
    disconnect(procedure_2, SIGNAL(finished(int)), this, SLOT(mux()));
    connect(procedure_3, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_3()));
    connect(procedure_3, SIGNAL(finished(int)), this, SLOT(error_3()));
    ui->label_53->setText("Muxing:");
    ui->progressBar->setValue(0);
    QStringList arguments;
    arguments << "-hide_banner" << "-i" << temp_file << "-map" << "0:0" << "-movflags" << "+write_colr"
              << "-c:v" << "copy" << "-map" << "0:a" << "-c:a" << "copy" << "-y" << output_file;
    procedure_3->start("ffmpeg", arguments);
    if (!procedure_3->waitForStarted()) {
        std::cout << "cmd command not found!!!" << std::endl;
        disconnect(procedure_3, SIGNAL(finished(int)), this, SLOT(error_3()));
        _message = "An unknown error occured!\n";
        ui->pushButton_1->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        Taskcomplete taskcomplete;
        taskcomplete.setModal(true);
        taskcomplete.exec();
    };
}

void MainWindow::complete_mux() // complete_mux
{
    std::cout << "Complete mux ..." << std::endl;  //  Debug info //
    disconnect(procedure_3, SIGNAL(finished(int)), this, SLOT(complete_mux()));
    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    QDir().remove(temp_file);
//    os.rmdir(temp_folder)
//    except:
//        message = "Task completed!\nPlease, clear temporary folder manually.\n"
//        self.task_complete()
    _message = "Task completed!\n";
    Taskcomplete taskcomplete;
    taskcomplete.setModal(true);
    taskcomplete.exec();
}

void MainWindow::encode_file() // encode_file
{
    std::cout << "Encode file ..." << std::endl;  //  Debug info //
    std::cout << "Preset: " << preset.toStdString() << std::endl;  //  Debug info //
    connect(procedure_4, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_4()));
    connect(procedure_4, SIGNAL(finished(int)), this, SLOT(error_4()));
    int dur_mod = round(dur);
    float fps_mod = fps.toFloat();
    fr_count = round(dur_mod * fps_mod);
    if (fr_count == 0) {
        _message = "Select the correct input and output file!";
        Taskcomplete taskcomplete;
        taskcomplete.setModal(true);
        taskcomplete.exec();
        return;
    };
    ui->pushButton_1->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->label_53->show();
    ui->label_53->setText("Encoding:");
    ui->label_54->show();
    ui->label_55->show();
    ui->progressBar->show();
    ui->progressBar->setValue(0);
    loop_start = time (NULL);
    QStringList arguments;
    arguments << preset_0.split(" ") << "-i" << input_file << preset.split(" ") << "-y" << output_file;
    procedure_4->start("ffmpeg", arguments);
    if (!procedure_4->waitForStarted()) {
        std::cout << "cmd command not found!!!" << std::endl;
        disconnect(procedure_4, SIGNAL(finished(int)), this, SLOT(error_4()));
        _message = "An unknown error occurred!\n Possible FFMPEG not installed.\n";
        ui->pushButton_1->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        Taskcomplete taskcomplete;
        taskcomplete.setModal(true);
        taskcomplete.exec();
    };
}

void MainWindow::complete()
{
    std::cout << "Complete ..." << std::endl;  //  Debug info //
    disconnect(procedure_4, SIGNAL(finished(int)), this, SLOT(complete()));
    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    _message = "Task completed!\n";
    Taskcomplete taskcomplete;
    taskcomplete.setModal(true);
    taskcomplete.exec();
}

void MainWindow::error_1()
{
    std::cout << "Error_1 ..." << std::endl;  //  Debug info //
    disconnect(procedure_1, SIGNAL(finished(int)), this, SLOT(error_1()));
    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    _message = "An error occured!\nPossible reasons:\n - incorrect encoder settings or error in " \
               "the input file,\n - no enough space on the disk.";
    Taskcomplete taskcomplete;
    taskcomplete.setModal(true);
    taskcomplete.exec();
}

void MainWindow::error_2()
{
    std::cout << "Error_2 ..." << std::endl;  //  Debug info //
    disconnect(procedure_2, SIGNAL(finished(int)), this, SLOT(error_2()));
    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    _message = "An error occured!\nPossible reasons:\n - incorrect encoder settings or error in " \
               "the input file,\n - no enough space on the disk.";
    Taskcomplete taskcomplete;
    taskcomplete.setModal(true);
    taskcomplete.exec();
}

void MainWindow::error_3()
{
    std::cout << "Error_3 ..." << std::endl;  //  Debug info //
    disconnect(procedure_3, SIGNAL(finished(int)), this, SLOT(error_3()));
    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    _message = "An error occured!\nPossible reasons:\n - incorrect encoder settings or error in " \
               "the input file,\n - no enough space on the disk.";
    Taskcomplete taskcomplete;
    taskcomplete.setModal(true);
    taskcomplete.exec();
}

void MainWindow::error_4()
{
    std::cout << "Error_4 ..." << std::endl;  //  Debug info //
    disconnect(procedure_4, SIGNAL(finished(int)), this, SLOT(error_4()));
    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    _message = "An error occured!\nPossible reasons:\n - incorrect encoder settings or error in " \
               "the input file,\n - no enough space on the disk.";
    Taskcomplete taskcomplete;
    taskcomplete.setModal(true);
    taskcomplete.exec();
}

void MainWindow::cancel()
{
    std::cout << "Cancel ..." << std::endl;  //  Debug info //
    ui->pushButton_1->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    procedure_1->disconnect();
    procedure_2->disconnect();
    procedure_3->disconnect();
    procedure_4->disconnect();
    _message = "All processes are cancelled!\n";
    Taskcomplete taskcomplete;
    taskcomplete.setModal(true);
    taskcomplete.exec();
}

void MainWindow::on_pushButton_4_clicked() // Stop
{
    std::cout << "Call Stop ..." << std::endl;  //  Debug info //
    procedure_1->disconnect();
    procedure_2->disconnect();
    procedure_3->disconnect();
    procedure_4->disconnect();
    connect(procedure_1, SIGNAL(finished(int)), this, SLOT(cancel()));
    connect(procedure_2, SIGNAL(finished(int)), this, SLOT(cancel()));
    connect(procedure_3, SIGNAL(finished(int)), this, SLOT(cancel()));
    connect(procedure_4, SIGNAL(finished(int)), this, SLOT(cancel()));
    procedure_1->kill();
    procedure_2->kill();
    procedure_3->kill();
    procedure_4->kill();
}

void MainWindow::progress_1()
{
    QString line = procedure_1->readAllStandardOutput();
    QString line_mod6 = line.replace("   ", " ").replace("  ", " ").replace("  ", " ").replace("= ", "=");
    int pos_st = line_mod6.indexOf("frame=");
    if (pos_st == 0) {
        QStringList data = line_mod6.split(" ");
        QString frame_qstr = data[0].replace("frame=", "");
        int frame = frame_qstr.toInt();
        int iter_start = time (NULL);
        int timer = iter_start - loop_start;
        float full_time = (timer * fr_count) / frame;
        float rem_time = full_time - timer;
        if (rem_time < 0) {
            rem_time = 0;
        };
        int h = trunc(rem_time / 3600);
        int m = trunc((rem_time - (h * 3600)) / 60);
        int s = trunc(rem_time - (h * 3600) - (m * 60));
        int percent = 0.9 + (frame * 100) / fr_count;
        if (percent > 100) {
            percent = 100;
        };
        ui->progressBar->setValue(percent);
        QString hrs = QString::number(h);
        QString min = QString::number(m);
        QString sec = QString::number(s);
        std::ostringstream sstr;
        sstr << std::setw(2) << std::setfill('0') << hrs.toStdString() << ":"
             << std::setw(2) << std::setfill('0') << min.toStdString() << ":"
             << std::setw(2) << std::setfill('0') << sec.toStdString();
        std::string tm = sstr.str();
        ui->label_55->setText(QString::fromStdString(tm));
        if ((percent >= 50) && (calling_pr_1 == true)) {
             disconnect(procedure_1, SIGNAL(finished(int)), this, SLOT(error_1()));
             connect(procedure_1, SIGNAL(finished(int)), this, SLOT(add_metadata()));
             calling_pr_1 = false;
        };
    };
}

void MainWindow::progress_2()
{
    QString line = procedure_2->readAllStandardOutput();
    int pos_st = line.indexOf("Done.");
    int pos_nf = line.indexOf("Nothing to do.");
    if ((pos_st != -1) or (pos_nf != -1)) {
        int percent = 100;
        ui->progressBar->setValue(percent);
        if ((percent == 100) && (calling_pr_2 == true)) {
            disconnect(procedure_2, SIGNAL(finished(int)), this, SLOT(error_2()));
            connect(procedure_2, SIGNAL(finished(int)), this, SLOT(mux()));
            loop_start = time (NULL);
            calling_pr_2 = false;
        };
    };
}

void MainWindow::progress_3()
{
    QString line = procedure_3->readAllStandardOutput();
    QString line_mod6 = line.replace("   ", " ").replace("  ", " ").replace("  ", " ").replace("= ", "=");
    int pos_st = line_mod6.indexOf("frame=");
    if (pos_st == 0) {
        QStringList data = line_mod6.split(" ");
        QString frame_qstr = data[0].replace("frame=", "");
        int frame = frame_qstr.toInt();
        int iter_start = time (NULL);
        int timer = iter_start - loop_start;
        float full_time = (timer * fr_count) / frame;
        float rem_time = full_time - timer;
        if (rem_time < 0) {
            rem_time = 0;
        };
        int h = trunc(rem_time / 3600);
        int m = trunc((rem_time - (h * 3600)) / 60);
        int s = trunc(rem_time - (h * 3600) - (m * 60));
        int percent = 0.9 + (frame * 100) / fr_count;
        if (percent > 100) {
            percent = 100;
        };
        ui->progressBar->setValue(percent);
        QString hrs = QString::number(h);
        QString min = QString::number(m);
        QString sec = QString::number(s);
        std::ostringstream sstr;
        sstr << std::setw(2) << std::setfill('0') << hrs.toStdString() << ":"
             << std::setw(2) << std::setfill('0') << min.toStdString() << ":"
             << std::setw(2) << std::setfill('0') << sec.toStdString();
        std::string tm = sstr.str();
        ui->label_55->setText(QString::fromStdString(tm));
        if ((percent >= 50) && (calling_pr_3 == true)) {
             disconnect(procedure_3, SIGNAL(finished(int)), this, SLOT(error_3()));
             connect(procedure_3, SIGNAL(finished(int)), this, SLOT(complete_mux()));
             calling_pr_3 = false;
        };
    };
}

void MainWindow::progress_4()
{
    QString line = procedure_4->readAllStandardOutput();
    QString line_mod6 = line.replace("   ", " ").replace("  ", " ").replace("  ", " ").replace("= ", "=");
    int pos_st = line_mod6.indexOf("frame=");
    if (pos_st == 0) {
        QStringList data = line_mod6.split(" ");
        QString frame_qstr = data[0].replace("frame=", "");
        int frame = frame_qstr.toInt();
        int iter_start = time (NULL);
        int timer = iter_start - loop_start;
        float full_time = (timer * fr_count) / frame;
        float rem_time = full_time - timer;
        if (rem_time < 0) {
            rem_time = 0;
        };
        int h = trunc(rem_time / 3600);
        int m = trunc((rem_time - (h * 3600)) / 60);
        int s = trunc(rem_time - (h * 3600) - (m * 60));
        int percent = 0.9 + (frame * 100) / fr_count;
        if (percent > 100) {
            percent = 100;
        };
        ui->progressBar->setValue(percent);
        QString hrs = QString::number(h);
        QString min = QString::number(m);
        QString sec = QString::number(s);
        std::ostringstream sstr;
        sstr << std::setw(2) << std::setfill('0') << hrs.toStdString() << ":"
             << std::setw(2) << std::setfill('0') << min.toStdString() << ":"
             << std::setw(2) << std::setfill('0') << sec.toStdString();
        std::string tm = sstr.str();
        ui->label_55->setText(QString::fromStdString(tm));
        if ((percent >= 50) && (calling_pr_4 == true)) {
             disconnect(procedure_4, SIGNAL(finished(int)), this, SLOT(error_4()));
             connect(procedure_4, SIGNAL(finished(int)), this, SLOT(complete()));
             calling_pr_4 = false;
        };
    };
}
