/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: streamconverter.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "streamconverter.h"
#include "ui_streamconverter.h"
#include "tables.h"
#include "message.h"
#include "helper.h"
#include <QPushButton>
#include <QComboBox>
#include <QListView>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QTimer>
#include <iostream>
#include <math.h>

#define SLT(method) &StreamConverter::method

typedef void(StreamConverter::*FnVoidVoid)(void);
typedef void(StreamConverter::*FnVoidInt)(int);
typedef void(StreamConverter::*FnVoidStr)(const QString&);

using namespace Constants;

StreamConverter::StreamConverter(QWidget *parent,
                                 Mode mode,
                                 StreamData *data):
    BaseWindow(parent, true),
    ui(new Ui::StreamConverter),
    m_status_pause_btn(EncodingStatus::PAUSE),
    m_windowActivated(false),
    m_mode(mode),
    m_pData(data),
    m_aParam({}),
    m_sParam({})
{
    ui->setupUi(centralWidget());
    setTitleBar(ui->frame_top);
    QFont font;
    font.setPointSize(10);
    ui->label_title->setFont(font);
    ui->frameMiddle->setFocusPolicy(Qt::StrongFocus);

    const QString title = (data->cont_type == ContentType::Audio) ?
                tr("Audio") + " " + tr("Converter") :
                tr("Subtitle") + " " + tr("Converter");
    setWindowTitle(title);
    ui->label_title->setText(title);

    // Connections
    connect(ui->closeWindow, &QPushButton::clicked, this, &StreamConverter::onCloseWindow);
    connect(ui->buttonExport, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(1);
        ui->buttonReturn->hide();
        m_status_pause_btn = EncodingStatus::PAUSE;
        ui->buttonPause->setToolTip(tr("Pause").toUpper());
        ui->buttonPause->setText(tr("Pause").toUpper());
        onStart();
    });
    connect(ui->buttonPause, &QPushButton::clicked, this, [this]() {
        if (m_pEncoder->getEncodingState() == QProcess::NotRunning)
            return;
        switch (m_status_pause_btn) {
        case EncodingStatus::PAUSE: {
            Print("Status encode btn: pause");
            pause();
            m_status_pause_btn = EncodingStatus::RESUME;
            break;
        }
        case EncodingStatus::RESUME: {
            Print("Status encode btn: resume");
            resume();
            m_status_pause_btn = EncodingStatus::PAUSE;
            break;
        }
        default:
            return;
        }
        QString status[] = {tr("Pause"), tr("Pause").toUpper(), tr("Resume").toUpper()};
        ui->buttonPause->setToolTip(status[m_status_pause_btn]);
        ui->buttonPause->setText(status[m_status_pause_btn]);
    });
    connect(ui->buttonStop, &QPushButton::clicked, this, &StreamConverter::onStop);
    connect(ui->buttonReturn, &QPushButton::clicked, this, [this]() {
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [this](int index) {
        bool status = (index == 0) ? false : true;
        ui->buttonExport->setVisible(!status);
        ui->buttonPause->setVisible(status);
        ui->buttonStop->setVisible(status);
    });
    connect(ui->comboBox_audio_codec, &QComboBox::currentTextChanged,
            this, &StreamConverter::onComboBox_audio_codec_textChanged);
    connect(ui->comboBox_subt_codec, &QComboBox::currentTextChanged,
            this, &StreamConverter::onComboBox_subt_codec_textChanged);

    connect(this, &StreamConverter::destroyed, this, [this]() {
        SETTINGS(stn);
        stn.beginGroup("ConverterWidget");
        stn.setValue("ConverterWidget/geometry", this->saveGeometry());
        stn.endGroup();
    });

    m_pEncoder = new EncoderStream(this);
    connect(m_pEncoder, &EncoderStream::onEncodingStarted, this, SLT(onEncodingStarted));
    connect(m_pEncoder, &EncoderStream::onEncodingInitError, this, SLT(onEncodingInitError));
    connect(m_pEncoder, &EncoderStream::onEncodingProgress, this, SLT(onEncodingProgress));
    connect(m_pEncoder, &EncoderStream::onEncodingLog, this, SLT(onEncodingLog));
    connect(m_pEncoder, &EncoderStream::onEncodingCompleted, this, SLT(onEncodingCompleted));
    connect(m_pEncoder, &EncoderStream::onEncodingAborted, this, SLT(onEncodingAborted));
    connect(m_pEncoder, &EncoderStream::onEncodingError, this, SLT(onEncodingError));
}

StreamConverter::~StreamConverter()
{
    delete ui;
}

void StreamConverter::onCloseWindow()
{
    if (m_pEncoder->getEncodingState() != QProcess::NotRunning) {
        if (!showDialogMessage(tr("Stop encoding?"))) {
            return;
        }
        m_pEncoder->killEncoding();
    }
    closeDialog();
}

void StreamConverter::lockSignals(bool status)
{
    ui->comboBox_subt_codec->blockSignals(status);
    ui->comboBox_audio_codec->blockSignals(status);
}

void StreamConverter::showEvent(QShowEvent *event)
{
    BaseWindow::showEvent(event);
    if (!m_windowActivated) {
        m_windowActivated = true;

        QFont fnt = ui->tableWidget->horizontalHeader()->font();
        fnt.setItalic(true);
        fnt.setBold(true);

        //*********** Table parameters ****************//
        ui->tableWidget->setRowCount(0);
        ui->tableWidget->setShowGrid(false);
        ui->tableWidget->horizontalHeader()->setFont(fnt);
        ui->tableWidget->horizontalHeader()->setVisible(true);
        ui->tableWidget->verticalHeader()->setVisible(true);
        ui->tableWidget->setAlternatingRowColors(true);
        ui->tableWidget->setDropIndicatorShown(true);
        ui->tableWidget->setDragEnabled(true);
        ui->tableWidget->setDragDropOverwriteMode(true);
        ui->tableWidget->setDragDropMode(QAbstractItemView::DropOnly);
        ui->tableWidget->setDefaultDropAction(Qt::TargetMoveAction);
        ui->tableWidget->setColumnWidth(0, 250);
        ui->tableWidget->setColumnWidth(1, 80);
        ui->tableWidget->setColumnWidth(2, 85);
        ui->tableWidget->setColumnWidth(3, 70);
        ui->tableWidget->setColumnWidth(4, 70);
        ui->tableWidget->setColumnWidth(5, 90);

        ui->closeTWindow->hide();
        ui->closeTWindow_2->hide();
        ui->resetLabels->hide();
        ui->noName_2->hide();

        if (m_pData->cont_type == ContentType::Audio) {
            ui->stackedWidget_export->setCurrentIndex(0);
        } else {
            ui->stackedWidget_export->setCurrentIndex(1);
        }

        if (m_mode == StreamConverter::Mode::Extract) {
            ui->frameTask->hide();
            ui->frame_bottom->hide();
        } else {
            ui->buttonExport->hide();
        }
        ui->buttonPause->hide();
        ui->buttonStop->hide();

        SETTINGS(stn);
        if (stn.childGroups().contains("ConverterWidget")) {
            stn.beginGroup("ConverterWidget");
            restoreGeometry(stn.value("ConverterWidget/geometry", geometry()).toByteArray());
            stn.endGroup();
        } else {
            QSizeF size(this->size());
            QPoint center = QPointF(size.width()/2, size.height()/2).toPoint();
            move(parentWidget()->geometry().center() - center);
        }

        // Fill interface
        auto comboBoxes = findChildren<QComboBox*>();
        foreach (auto combo, comboBoxes) {
            QListView *_view = new QListView(combo);
            _view->setTextElideMode(Qt::ElideMiddle);
            combo->setView(_view);
        }

        Tables t;
        lockSignals(true);
        ui->comboBox_audio_container->clear();
        ui->comboBox_audio_bitrate->clear();
        ui->comboBox_audio_channels->clear();
        ui->comboBox_audio_sampling->clear();
        ui->comboBox_audio_codec->clear();
        ui->comboBox_subt_codec->clear();
        ui->comboBox_subt_container->clear();
        ui->comboBox_audio_channels->addItems({tr("Source"), tr("trim to 1 ch"), tr("trim to 2 ch")});

        int asampling_size = sizeof(t.arr_sampling)/sizeof(QString);
        Q_LOOP(i, 0, asampling_size)
            ui->comboBox_audio_sampling->addItem(t.arr_sampling[i]);

        int acodec_size = sizeof(t.arr_acodec_sep)/sizeof(QString);
        Q_LOOP(i, 0, acodec_size)
            ui->comboBox_audio_codec->addItem(t.arr_acodec_sep[i]);

        int scodec_size = sizeof(t.arr_scodec_sep)/sizeof(QString);
        Q_LOOP(i, 0, scodec_size)
            ui->comboBox_subt_codec->addItem(t.arr_scodec_sep[i]);

        lockSignals(false);
        ui->comboBox_audio_codec->setCurrentIndex(1);
        ui->comboBox_audio_codec->setCurrentIndex(0);
        ui->comboBox_subt_codec->setCurrentIndex(1);
        ui->comboBox_subt_codec->setCurrentIndex(0);
    }
}

void StreamConverter::closeEvent(QCloseEvent *event)
{
    onCloseWindow();
    event->ignore();
}

bool StreamConverter::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            ui->frameMiddle->setFocus();
            return true;
        }
    }
    return BaseWindow::eventFilter(watched, event);
}

void StreamConverter::setWidgetsEnabled(bool status)
{

}

/************************************************
** Audio
************************************************/

void StreamConverter::onComboBox_audio_codec_textChanged(const QString &arg1)
{
    lockSignals(true);
    ui->comboBox_audio_bitrate->setEnabled(true);
    ui->comboBox_audio_bitrate->clear();
    ui->comboBox_audio_container->clear();
    ui->comboBox_audio_sampling->setEnabled(true);
    ui->comboBox_audio_channels->setEnabled(true);
    ui->comboBox_audio_container->setEnabled(true);
    if (arg1 == tr("Source")) {
        ui->comboBox_audio_bitrate->addItems({tr("Source")});
        ui->comboBox_audio_bitrate->setEnabled(false);
        ui->comboBox_audio_sampling->setCurrentIndex(0);
        ui->comboBox_audio_sampling->setEnabled(false);
        ui->comboBox_audio_channels->setCurrentIndex(0);
        ui->comboBox_audio_channels->setEnabled(false);
        ui->comboBox_audio_container->addItems({"MKA"});
        ui->comboBox_audio_container->setEnabled(false);
    }
    else if (arg1 == "Advanced Audio Coding") {
        ui->comboBox_audio_bitrate->addItems({"384k", "320k", "256k", "192k", "128k", "96k"});
        ui->comboBox_audio_bitrate->setCurrentIndex(2);
        ui->comboBox_audio_container->addItems({"AAC", "MKA"});
    }
    else if (arg1 == "Dolby Digital") {
        ui->comboBox_audio_bitrate->addItems({"640k", "448k", "384k", "256k"});
        ui->comboBox_audio_bitrate->setCurrentIndex(1);
        ui->comboBox_audio_container->addItems({"AC3", "MKA"});
    }
    else if (arg1 == "Dolby TrueHD") {
        ui->comboBox_audio_bitrate->addItems({"3840k", "3072k", "2048k", "1920k", "1536k", "1472k", "1344k", "1280k", "1152k",
                                                     "1024k", "960k", "768k", "640k", "576k", "512k", "448k", "384k"});
        ui->comboBox_audio_bitrate->setCurrentIndex(4);
        ui->comboBox_audio_container->addItems({"DTS", "THD", "MKA"});
    }
    else if (arg1 == "Vorbis") {
        ui->comboBox_audio_bitrate->addItems({"448k", "384k", "256k", "128k", "96k", "64k"});
        ui->comboBox_audio_bitrate->setCurrentIndex(2);
        ui->comboBox_audio_container->addItems({"OGG", "MKA"});
    }
    else if (arg1 == "Opus") {
        ui->comboBox_audio_bitrate->addItems({"448k", "384k", "256k", "128k", "96k", "64k"});
        ui->comboBox_audio_bitrate->setCurrentIndex(2);
        ui->comboBox_audio_container->addItems({"Opus", "MKA"});
    }
    else if (arg1 == "Pulse Code Modulation 16 bit") {
        ui->comboBox_audio_bitrate->addItems({tr("Auto")});
        ui->comboBox_audio_bitrate->setEnabled(false);
        ui->comboBox_audio_container->addItems({"WAV", "MKA"});
    }
    else if (arg1 == "Pulse Code Modulation 24 bit") {
        ui->comboBox_audio_bitrate->addItems({tr("Auto")});
        ui->comboBox_audio_bitrate->setEnabled(false);
        ui->comboBox_audio_container->addItems({"WAV", "MKA"});
    }
    else if (arg1 == "Pulse Code Modulation 32 bit") {
        ui->comboBox_audio_bitrate->addItems({tr("Auto")});
        ui->comboBox_audio_bitrate->setEnabled(false);
        ui->comboBox_audio_container->addItems({"WAV", "MKA"});
    }
    lockSignals(false);
}

/************************************************
** Subtitles
************************************************/

void StreamConverter::onComboBox_subt_codec_textChanged(const QString &arg1)
{
    lockSignals(true);
    ui->comboBox_subt_container->clear();
    ui->comboBox_subt_container->setEnabled(true);
    if (arg1 == tr("Source")) {
        ui->comboBox_subt_container->addItems({"MKS"});
        ui->comboBox_subt_container->setEnabled(false);
    }
    else if (arg1 == "SubRip") {
        ui->comboBox_subt_container->addItems({"SRT"});
    }
    else if (arg1 == "WebVTT") {
        ui->comboBox_subt_container->addItems({"VTT"});
    }
    else if (arg1 == "SubStation Alpha") {
        ui->comboBox_subt_container->addItems({"SSA"});
    }
    else if (arg1 == "Advanced SSA") {
        ui->comboBox_subt_container->addItems({"ASS"});
    }
    else if (arg1 == "Timed Text") {
        ui->comboBox_subt_container->addItems({"TTML"});
    }
    else if (arg1 == "MOV text") {
        ui->comboBox_subt_container->addItems({"MOV"});
    }
    lockSignals(false);
}

/************************************************
** Encoder
************************************************/

void StreamConverter::onStart()
{
    initEncoding();
}

void StreamConverter::onStop()
{
    Print("Call Stop ...");
    if (m_pEncoder->getEncodingState() != QProcess::NotRunning) {
        bool confirm = showDialogMessage(tr("Stop encoding?"));
        if (confirm)
            m_pEncoder->stopEncoding();
    }
}

void StreamConverter::initEncoding()
{
    m_aParam.AUDIO_CODEC = ui->comboBox_audio_codec->currentIndex();
    m_aParam.AUDIO_BITRATE = ui->comboBox_audio_bitrate->currentIndex();
    m_aParam.AUDIO_SAMPLING = ui->comboBox_audio_sampling->currentIndex();
    m_aParam.AUDIO_CHANNELS = ui->comboBox_audio_channels->currentIndex();
    m_aParam.AUDIO_CONTAINER = ui->comboBox_audio_container->currentText();
    m_sParam.SUBT_CODEC = ui->comboBox_subt_codec->currentIndex();
    m_sParam.SUBT_CONTAINER = ui->comboBox_subt_container->currentText();
    m_pEncoder->initEncoding(m_pData, &m_aParam, &m_sParam);
}

void StreamConverter::onEncodingStarted()
{

}

void StreamConverter::onEncodingInitError(const QString &message)
{
    showInfoMessage(message);
}

void StreamConverter::onEncodingProgress(const int percent, const float rem_time)
{
    ui->progressBar->setValue(percent);
    ui->progressBar_export->setValue(percent);
    ui->label_RemTime->setText(Helper::timeConverter(rem_time));
    ui->label_RemTime_export->setText(Helper::timeConverter(rem_time));
}

void StreamConverter::onEncodingLog(const QString &log)
{
    //Print(log.toStdString());
}

void StreamConverter::onEncodingCompleted()
{
    QTimer::singleShot(800, this, &StreamConverter::acceptDialog);
}

void StreamConverter::onEncodingAborted()
{
    ui->buttonReturn->show();
}

void StreamConverter::onEncodingError(const QString &error_message, bool popup)
{
    Q_UNUSED(popup)
    const QString msg = (error_message != "") ? tr("An error occurred: ") + error_message :
                                                tr("Unexpected error occurred!");
    showInfoMessage(msg);
    ui->buttonReturn->show();
}

void StreamConverter::pause()
{
    if (m_pEncoder->getEncodingState() != QProcess::NotRunning)
        m_pEncoder->pauseEncoding();
}

void StreamConverter::resume()
{
    if (m_pEncoder->getEncodingState() != QProcess::NotRunning)
        m_pEncoder->resumeEncoding();
}

bool StreamConverter::showDialogMessage(const QString &message)
{
    Message msg(this, MessType::DIALOG, message);
    if (msg.exec() == Dialog::Accept)
        return true;
    return false;
}

void StreamConverter::showInfoMessage(const QString &message)
{
    Message msg(this, MessType::INFO, message, false);
    msg.exec();
}
