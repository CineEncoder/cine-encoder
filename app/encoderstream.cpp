/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: encoderstream.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "encoderstream.h"
#include "tables.h"
#include <QDir>
#include <QMap>
#include <iostream>
#include <math.h>
#include <ctime>
#include <algorithm>

#define rnd(num) static_cast<int>(round(num))



EncoderStream::EncoderStream(QObject *parent) :
    QObject(parent),
    m_pData(nullptr)
{
    m_pProcessEncoding = new QProcess(this);
    m_pProcessEncoding->setProcessChannelMode(QProcess::MergedChannels);
    m_pProcessEncoding->setWorkingDirectory(QDir::homePath());
}

EncoderStream::~EncoderStream()
{

}

/************************************************
** Encoder
************************************************/

void EncoderStream::initEncoding(StreamData *data,
                                 EncoderAudioParam *aParam,
                                 EncoderSubtParam *sParam)
{
    Print("Make preset...");
    Tables t;
    m_pData  = data;
    int ACODEC = aParam->AUDIO_CODEC;
    int ABITRATE = aParam->AUDIO_BITRATE;
    int ASAMPLING = aParam->AUDIO_SAMPLING;
    int ACHANNELS = aParam->AUDIO_CHANNELS;
    QString ACONTAINER = aParam->AUDIO_CONTAINER;
    int SCODEC = sParam->SUBT_CODEC;
    QString SCONTAINER = sParam->SUBT_CONTAINER;

    m_preset_0 = "";
    m_preset = "";
    m_error_message = "";
    m_output_file = QFileInfo(data->output_file).absolutePath() + "/" +
                    QFileInfo(data->output_file).completeBaseName() +
                    "_" + numToStr(m_pData->stream) + ".";

    QString mapParam;
    if (data->cont_type == ContentType::Audio) {
        m_output_file += ACONTAINER.toLower();
        mapParam = QString("-map 0:a:%1? ").arg(numToStr(m_pData->stream));
    } else {
        m_output_file += SCONTAINER.toLower();
        mapParam = QString("-map 0:s:%1? ").arg(numToStr(m_pData->stream));
    }

    /************************************* Audio module ***************************************/
    QString sampling("");
    {
        const QString selected_sampling = t.arr_sampling[ASAMPLING];
        if (selected_sampling != "Source")
            sampling = QString("-af aresample=%1:resampler=soxr ").arg(selected_sampling);
    }
    QString channels("");
    {
        const QString selected_channels = t.arr_channels[ACHANNELS];
        if (selected_channels != "Source")
            channels = QString(" -ac %1").arg(selected_channels);
    }
    QString acodec("");
    {
        QString selected_bitrate("");
        const QString selected_acodec = t.arr_acodec_sep[ACODEC];
        if (selected_acodec == "Advanced Audio Coding") {
            selected_bitrate = t.arr_bitrate[0][ABITRATE];
            acodec = QString("-strict experimental -c:a aac -b:a %1").arg(selected_bitrate);
        }
        else
        if (selected_acodec == "Dolby Digital") {
            selected_bitrate = t.arr_bitrate[1][ABITRATE];
            acodec = QString("-c:a ac3 -b:a %1").arg(selected_bitrate);
        }
        else
        if (selected_acodec == "Dolby TrueHD") {
            selected_bitrate = t.arr_bitrate[2][ABITRATE];
            acodec = QString("-strict -2 -c:a dca -b:a %1").arg(selected_bitrate);
        }
        else
        if (selected_acodec == "Vorbis") {
            selected_bitrate = t.arr_bitrate[3][ABITRATE];
            acodec = QString("-c:a libvorbis -b:a %1").arg(selected_bitrate);
        }
        else
        if (selected_acodec == "Opus") {
            selected_bitrate = t.arr_bitrate[4][ABITRATE];
            acodec = QString("-c:a libopus -b:a %1").arg(selected_bitrate);
        }
        else
        if (selected_acodec == "Pulse Code Modulation 16 bit") {
            acodec = "-c:a pcm_s16le";
        }
        else
        if (selected_acodec == "Pulse Code Modulation 24 bit") {
            acodec = "-c:a pcm_s24le";
        }
        else
        if (selected_acodec == "Pulse Code Modulation 32 bit") {
            acodec = "-c:a pcm_s32le";
        }
        else
        if (selected_acodec == tr("Source")) {
            acodec = "-c:a copy";
        }
    }
    const QString aparam = "-sn " + sampling + acodec + channels;

    /************************************ Subtitle module *************************************/
    QString scodec("");
    {
        const QString selected_scodec = t.arr_scodec_sep[SCODEC];
        if (selected_scodec == "SubRip") {
            scodec = QString("-c:s srt");
        }
        else
        if (selected_scodec == "WebVTT") {
            scodec = QString("-c:s webvtt");
        }
        else
        if (selected_scodec == "SubStation Alpha") {
            scodec = QString("-c:s ssa");
        }
        else
        if (selected_scodec == "Advanced SSA") {
            scodec = QString("-c:s ass");
        }
        else
        if (selected_scodec == "Timed Text") {
            scodec = QString("-c:s ttml");
        }
        else
        if (selected_scodec == "MOV text") {
            scodec = QString("-c:s mov_text");
        }
        else
        if (selected_scodec == tr("Source")) {
            scodec = "-c:s copy";
        }
    }
    const QString sparam = "-an " + scodec;

    /************************************* Result module ***************************************/
    const QString param = (data->cont_type == ContentType::Audio) ? aparam : sparam;
    m_preset_0 = QString("-hide_banner -probesize 100M -analyzeduration 50M");
    m_preset = QString("-vn -map_metadata -1 -map_chapters -1 ") + mapParam + param;
    Print("preset_0: " << m_preset_0.toStdString());
    Print("preset: " << m_preset.toStdString());
    QString log = QString("Preset: %1 -i <input file> %2 -y <output file>\n")
                    .arg(m_preset_0, m_preset);
    emit onEncodingLog(log);
    encode();
}

void EncoderStream::encode()   // Encode
{
    Print("Encode ...");
    QStringList arguments;
    m_pProcessEncoding->disconnect();
    connect(m_pProcessEncoding, &QProcess::readyReadStandardOutput, this, &EncoderStream::progress);
    connect(m_pProcessEncoding, SIGNAL(finished(int)), this, SLOT(completed(int)));
    emit onEncodingProgress(0, 0.0f);

    if (m_pData->duration <= 0.f) {
        m_message = tr("The file does not contain duration information!\nSelect the correct input file!");
        emit onEncodingInitError(m_message);
        return;
    }
    emit onEncodingStarted();
    m_loop_start = time(nullptr);
    arguments << m_preset_0.split(" ") << "-i" << m_pData->input_file
              << m_preset.split(" ") << "-y" << m_output_file;
    //qDebug() << arguments;
    m_pProcessEncoding->start("ffmpeg", arguments);
    if (!m_pProcessEncoding->waitForStarted()) {
        Print("cmd command not found!!!");
        m_pProcessEncoding->disconnect();
        m_message = tr("An unknown error occurred!\n Possible FFMPEG not installed.\n");
        emit onEncodingInitError(m_message);
    }
}

void EncoderStream::progress()   // Progress
{
    QString line = QString(m_pProcessEncoding->readAllStandardOutput());
    const QString line_mod = line.replace("   ", " ").replace("  ", " ").replace("  ", " ").replace("= ", "=");
    emit onEncodingLog(line_mod);
    m_error_message = line_mod;
    const int pos_st = line_mod.indexOf("time=");
    if (pos_st != -1) {
        const QString data = line_mod.split("time=").at(1);
        const QString data_mod = data.split(' ').at(0);
        const QStringList data_mod_2 = data_mod.split(':');
        const float h_cur = data_mod_2.at(0).toFloat();
        const float m_cur = data_mod_2.at(1).toFloat();
        const float s_cur = data_mod_2.at(2).toFloat();
        float dur = 3600.f*h_cur + 60.f*m_cur + s_cur;
        if (dur <= 0.0f)
            dur = 0.001;
        const time_t iter_start = time(nullptr);
        const int timer = static_cast<int>(iter_start - m_loop_start);
        const float full_time = static_cast<float>(timer * m_pData->duration) / dur;
        float rem_time = full_time - static_cast<float>(timer);
        if (rem_time < 0.0f)
            rem_time = 0.0f;
        if (rem_time > MAXIMUM_ALLOWED_TIME)
            rem_time = MAXIMUM_ALLOWED_TIME;

        float percent = static_cast<float>(dur * 100.f) / m_pData->duration;
        int percent_int = rnd(percent);
        if (percent_int > 100)
            percent_int = 100;
        emit onEncodingProgress(percent_int, rem_time);
    }
}

QProcess::ProcessState EncoderStream::getEncodingState()
{
    return m_pProcessEncoding->state();
}

void EncoderStream::pauseEncoding()
{
#ifdef Q_OS_WIN
    _PROCESS_INFORMATION *pi = m_pProcessEncoding->pid();
    SuspendThread(pi->hThread);  // pause for Windows
#else
    kill(pid_t(m_pProcessEncoding->processId()), SIGSTOP);  // pause for Unix
#endif
}

void EncoderStream::resumeEncoding()
{
#ifdef Q_OS_WIN
    _PROCESS_INFORMATION *pi = m_pProcessEncoding->pid();
    ResumeThread(pi->hThread);  // resume for Windows
#else
    kill(pid_t(m_pProcessEncoding->processId()), SIGCONT); // resume for Unix
#endif
}

void EncoderStream::stopEncoding()
{
    m_pProcessEncoding->disconnect();
    connect(m_pProcessEncoding, SIGNAL(finished(int)), this, SLOT(abort()));
    m_pProcessEncoding->kill();
}

void EncoderStream::killEncoding()
{
    if (m_pProcessEncoding->state() == QProcess::Running)
        m_pProcessEncoding->kill();
}

void EncoderStream::completed(int exit_code)
{
    m_pProcessEncoding->disconnect();
    if (exit_code == 0) {
        emit onEncodingProgress(100, 0.0f);
        emit onEncodingCompleted();
    } else
        emit onEncodingError(m_error_message);
}

void EncoderStream::abort()
{
    m_pProcessEncoding->disconnect();
    emit onEncodingAborted();
}
