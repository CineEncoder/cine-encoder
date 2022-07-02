/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: preset.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "preset.h"
#include "ui_preset.h"
#include "tables.h"
#include "message.h"
#include <QListView>
#include <QMouseEvent>
#include <QCloseEvent>
#include <QResizeEvent>
#include <QTimer>
#include <iostream>
#include <math.h>

#define SLT(method) &Preset::method

typedef void(Preset::*FnVoidVoid)(void);
typedef void(Preset::*FnVoidInt)(int);
typedef void(Preset::*FnVoidStr)(const QString&);

using namespace Constants;

Preset::Preset(QWidget *parent, QVector<QString> *pOld_param):
    BaseWindow(parent, true),
    ui(new Ui::Preset),
    m_pNew_param(pOld_param),
    m_aspectRatio(0.0f),
    m_repeat(0),
    m_windowActivated(false)
{
    ui->setupUi(centralWidget());
    setTitleBar(ui->frame_top);
    QFont font;
    font.setPointSize(10);
    ui->label_title->setFont(font);
    ui->frameMiddle->setFocusPolicy(Qt::StrongFocus);

    connect(ui->closeWindow, &QPushButton::clicked, this, &Preset::onCloseWindow);
    connect(ui->buttonCancel, &QPushButton::clicked, this, &Preset::onCloseWindow);
    connect(ui->buttonApply, &QPushButton::clicked, this, &Preset::onButtonApply);
    // Tab buttons
    QList<QPushButton*> tabButtons = {ui->buttonTab_1, ui->buttonTab_2,
                                      ui->buttonTab_3, ui->buttonTab_4};
    for (int i = 0; i < tabButtons.size(); i++) {
        connect(tabButtons[i], &QPushButton::clicked, this, [this, i, tabButtons]() {
            for (int j = 0; j < tabButtons.size(); j++)
                tabButtons[j]->setEnabled(i != j);
            ui->tabWidgetSettings->setCurrentIndex(i);
        });
    }

    QComboBox *iboxes[] = {
        ui->comboBoxAspectRatio, ui->comboBoxFrameRate,
        ui->comboBox_preset, ui->comboBox_pass
    };
    FnVoidInt iboxes_methods[] = {
        SLT(onComboBoxAspectRatio_indexChanged), SLT(onComboBoxFrameRate_indexChanged),
        SLT(onComboBox_preset_indexChanged), SLT(onComboBox_pass_indexChanged)
    };
    for (int i = 0; i < 4; i++)
        connect(iboxes[i], static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, iboxes_methods[i]);

    QComboBox *boxes[] = {
        ui->comboBox_width, ui->comboBox_height, ui->comboBox_codec,
        ui->comboBox_mode, ui->comboBox_audio_codec, ui->comboBox_master_disp
    };
    FnVoidStr boxes_methods[] = {
        SLT(onComboBox_width_textChanged), SLT(onComboBox_height_textChanged), SLT(onComboBox_codec_textChanged),
        SLT(onComboBox_mode_textChanged), SLT(onComboBox_audio_codec_textChanged), SLT(onComboBox_master_disp_textChanged)
    };
    for (int i = 0; i < 6; i++)
        connect(boxes[i], &QComboBox::currentTextChanged, this, boxes_methods[i]);

    connect(ui->comboBox_container, &QComboBox::currentTextChanged,
            this, &Preset::onComboBox_container_textChanged);
    connect(ui->comboBox_audio_bitrate, &QComboBox::currentTextChanged,
            this, &Preset::onComboBox_audio_bitrate_textChanged);
    connect(ui->lineEdit_bitrate, &QLineEdit::editingFinished,
            this, &Preset::onLineEdit_bitrate_editingFinished);

    connect(this, &Preset::destroyed, this, [this]() {
        SETTINGS(stn);
        stn.beginGroup("PresetWidget");
        stn.setValue("PresetWidget/geometry", this->saveGeometry());
        stn.endGroup();
    });
}

Preset::~Preset()
{
    delete ui;
}

void Preset::onCloseWindow()
{
    closeDialog();
}

void Preset::onButtonApply()  // Apply preset
{
    (*m_pNew_param)[CurParamIndex::OUTPUT_PARAM] = ui->textBrowser_presetname->toPlainText();
    (*m_pNew_param)[CurParamIndex::CODEC] = QString::number(ui->comboBox_codec->currentIndex());
    (*m_pNew_param)[CurParamIndex::MODE] = QString::number(ui->comboBox_mode->currentIndex());
    (*m_pNew_param)[CurParamIndex::CONTAINER] = QString::number(ui->comboBox_container->currentIndex());
    (*m_pNew_param)[CurParamIndex::BQR] = ui->lineEdit_bitrate->text();
    (*m_pNew_param)[CurParamIndex::MINRATE] = ui->lineEdit_minrate->text();
    (*m_pNew_param)[CurParamIndex::MAXRATE] = ui->lineEdit_maxrate->text();
    (*m_pNew_param)[CurParamIndex::BUFSIZE] = ui->lineEdit_bufsize->text();
    (*m_pNew_param)[CurParamIndex::LEVEL] = QString::number(ui->comboBox_level->currentIndex());
    (*m_pNew_param)[CurParamIndex::FRAME_RATE] = QString::number(ui->comboBoxFrameRate->currentIndex());
    (*m_pNew_param)[CurParamIndex::BLENDING] = QString::number(ui->comboBoxBlending->currentIndex());
    (*m_pNew_param)[CurParamIndex::WIDTH] = QString::number(ui->comboBox_width->currentIndex());
    (*m_pNew_param)[CurParamIndex::HEIGHT] = QString::number(ui->comboBox_height->currentIndex());
    (*m_pNew_param)[CurParamIndex::PASS]= QString::number(ui->comboBox_pass->currentIndex());
    (*m_pNew_param)[CurParamIndex::PRESET] = QString::number(ui->comboBox_preset->currentIndex());
    (*m_pNew_param)[CurParamIndex::COLOR_RANGE] = QString::number(ui->comboBox_color_range->currentIndex());
    (*m_pNew_param)[CurParamIndex::PRIMARY] = QString::number(ui->comboBox_color_prim->currentIndex());
    (*m_pNew_param)[CurParamIndex::MATRIX] = QString::number(ui->comboBox_color_matrix->currentIndex());
    (*m_pNew_param)[CurParamIndex::TRC] = QString::number(ui->comboBox_transfer->currentIndex());
    (*m_pNew_param)[CurParamIndex::MIN_LUM] = ui->lineEdit_min_lum->text();
    (*m_pNew_param)[CurParamIndex::MAX_LUM] = ui->lineEdit_max_lum->text();
    (*m_pNew_param)[CurParamIndex::MAX_CLL] = ui->lineEdit_max_cll->text();
    (*m_pNew_param)[CurParamIndex::MAX_FALL] = ui->lineEdit_max_fall->text();
    (*m_pNew_param)[CurParamIndex::MASTER_DISPLAY] = QString::number(ui->comboBox_master_disp->currentIndex());
    (*m_pNew_param)[CurParamIndex::CHROMA_COORD] = ui->lineEdit_chroma_coord->text();
    (*m_pNew_param)[CurParamIndex::WHITE_COORD] = ui->lineEdit_white_coord->text();
    (*m_pNew_param)[CurParamIndex::AUDIO_CODEC] = QString::number(ui->comboBox_audio_codec->currentIndex());
    (*m_pNew_param)[CurParamIndex::AUDIO_BITRATE] = QString::number(ui->comboBox_audio_bitrate->currentIndex());
    (*m_pNew_param)[CurParamIndex::ASAMPLE_RATE] = QString::number(ui->comboBox_audio_sampling->currentIndex());
    (*m_pNew_param)[CurParamIndex::ACHANNELS] = QString::number(ui->comboBox_audio_channels->currentIndex());
    (*m_pNew_param)[CurParamIndex::REP_PRIM] = QString::number(ui->checkBox_primaries->checkState());
    (*m_pNew_param)[CurParamIndex::REP_MATRIX] = QString::number(ui->checkBox_matrix->checkState());
    (*m_pNew_param)[CurParamIndex::REP_TRC] = QString::number(ui->checkBox_transfer->checkState());

    this->acceptDialog();
}

void Preset::lockSignals(bool status)
{
    ui->comboBox_codec->blockSignals(status);
    ui->comboBox_audio_codec->blockSignals(status);
    ui->comboBox_master_disp->blockSignals(status);
    ui->comboBoxAspectRatio->blockSignals(status);
    ui->comboBox_width->blockSignals(status);
    ui->comboBox_height->blockSignals(status);
}

void Preset::showEvent(QShowEvent *event)
{
    BaseWindow::showEvent(event);
    if (!m_windowActivated) {
        m_windowActivated = true;

        SETTINGS(stn);
        if (stn.childGroups().contains("PresetWidget")) {
            stn.beginGroup("PresetWidget");
            restoreGeometry(stn.value("PresetWidget/geometry", geometry()).toByteArray());
            stn.endGroup();
        } else {
            QSizeF size(this->size());
            QPoint center = QPointF(size.width()/2, size.height()/2).toPoint();
            move(parentWidget()->geometry().center() - center);
        }

        QTimer *timer = new QTimer(this);
        timer->setInterval(450);
        connect(timer, &QTimer::timeout, this, &Preset::repeat_handler);
        timer->start();

        auto comboBoxes = findChildren<QComboBox*>();
        foreach (auto combo, comboBoxes) {
            //Print(combo->objectName().toStdString());
            QListView *_view = new QListView(combo);
            _view->setTextElideMode(Qt::ElideMiddle);
            combo->setView(_view);
        }

        QDoubleValidator *doubleValidator = new QDoubleValidator(0.0, 10000.0, 3, this);
        doubleValidator->setNotation(QDoubleValidator::StandardNotation);
        ui->lineEdit_bitrate->setValidator(doubleValidator);
        ui->lineEdit_minrate->setValidator(doubleValidator);
        ui->lineEdit_maxrate->setValidator(doubleValidator);
        ui->lineEdit_bufsize->setValidator(doubleValidator);
        ui->lineEdit_max_lum->setValidator(doubleValidator);
        ui->lineEdit_min_lum->setValidator(doubleValidator);
        ui->lineEdit_max_cll->setValidator(doubleValidator);
        ui->lineEdit_max_fall->setValidator(doubleValidator);

    //    QIntValidator *intValidator = new QIntValidator(0, 10000, this);
    //    ui->lineEdit_max_lum->setValidator(intValidator);
    //    ui->lineEdit_min_lum->setValidator(intValidator);
    //    ui->lineEdit_max_cll->setValidator(intValidator);
    //    ui->lineEdit_max_fall->setValidator(intValidator);
        if ((*m_pNew_param).size() != PARAMETERS_COUNT) {
            QTimer::singleShot(1000, this, [this](){
                Message msg(this, MessType::INFO, tr("An unexpected error has occurred: \
                            the number of parameters does not match. The window will be closed."));
                msg.exec();
                closeDialog();
            });
        }
        ui->textBrowser_presetname->setText((*m_pNew_param)[CurParamIndex::OUTPUT_PARAM]);
        ui->comboBox_codec->setCurrentIndex((*m_pNew_param)[CurParamIndex::CODEC].toInt());
        ui->comboBox_mode->setCurrentIndex((*m_pNew_param)[CurParamIndex::MODE].toInt());
        ui->comboBox_container->setCurrentIndex((*m_pNew_param)[CurParamIndex::CONTAINER].toInt());
        ui->lineEdit_bitrate->setText((*m_pNew_param)[CurParamIndex::BQR]);
        ui->lineEdit_minrate->setText((*m_pNew_param)[CurParamIndex::MINRATE]);
        ui->lineEdit_maxrate->setText((*m_pNew_param)[CurParamIndex::MAXRATE]);
        ui->lineEdit_bufsize->setText((*m_pNew_param)[CurParamIndex::BUFSIZE]);
        ui->comboBox_level->setCurrentIndex((*m_pNew_param)[CurParamIndex::LEVEL].toInt());
        ui->comboBoxFrameRate->setCurrentIndex((*m_pNew_param)[CurParamIndex::FRAME_RATE].toInt());
        ui->comboBoxBlending->setCurrentIndex((*m_pNew_param)[CurParamIndex::BLENDING].toInt());
        ui->comboBox_width->setCurrentIndex((*m_pNew_param)[CurParamIndex::WIDTH].toInt());
        ui->comboBox_height->setCurrentIndex((*m_pNew_param)[CurParamIndex::HEIGHT].toInt());
        ui->comboBox_pass->setCurrentIndex((*m_pNew_param)[CurParamIndex::PASS].toInt());
        ui->comboBox_preset->setCurrentIndex((*m_pNew_param)[CurParamIndex::PRESET].toInt());
        ui->comboBox_color_range->setCurrentIndex((*m_pNew_param)[CurParamIndex::COLOR_RANGE].toInt());
        ui->comboBox_color_prim->setCurrentIndex((*m_pNew_param)[CurParamIndex::PRIMARY].toInt());
        ui->comboBox_color_matrix->setCurrentIndex((*m_pNew_param)[CurParamIndex::MATRIX].toInt());
        ui->comboBox_transfer->setCurrentIndex((*m_pNew_param)[CurParamIndex::TRC].toInt());
        ui->lineEdit_min_lum->setText((*m_pNew_param)[CurParamIndex::MIN_LUM]);
        ui->lineEdit_max_lum->setText((*m_pNew_param)[CurParamIndex::MAX_LUM]);
        ui->lineEdit_max_cll->setText((*m_pNew_param)[CurParamIndex::MAX_CLL]);
        ui->lineEdit_max_fall->setText((*m_pNew_param)[CurParamIndex::MAX_FALL]);
        ui->comboBox_master_disp->setCurrentIndex((*m_pNew_param)[CurParamIndex::MASTER_DISPLAY].toInt());
        ui->lineEdit_chroma_coord->setText((*m_pNew_param)[CurParamIndex::CHROMA_COORD]);
        ui->lineEdit_white_coord->setText((*m_pNew_param)[CurParamIndex::WHITE_COORD]);
        ui->comboBox_audio_codec->setCurrentIndex((*m_pNew_param)[CurParamIndex::AUDIO_CODEC].toInt());
        ui->comboBox_audio_bitrate->setCurrentIndex((*m_pNew_param)[CurParamIndex::AUDIO_BITRATE].toInt());
        ui->comboBox_audio_sampling->setCurrentIndex((*m_pNew_param)[CurParamIndex::ASAMPLE_RATE].toInt());
        ui->comboBox_audio_channels->setCurrentIndex((*m_pNew_param)[CurParamIndex::ACHANNELS].toInt());
        ui->checkBox_primaries->setCheckState((Qt::CheckState)(*m_pNew_param)[CurParamIndex::REP_PRIM].toInt());
        ui->checkBox_matrix->setCheckState((Qt::CheckState)(*m_pNew_param)[CurParamIndex::REP_MATRIX].toInt());
        ui->checkBox_transfer->setCheckState((Qt::CheckState)(*m_pNew_param)[CurParamIndex::REP_TRC].toInt());
    }
}

bool Preset::eventFilter(QObject *watched, QEvent *event)
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

/************************************************
** Transform
************************************************/

void Preset::repeat_handler()
{
    //Print("Call by timer... Repeat count: " << m_repeat);
    if (m_repeat > 0) {
        m_repeat = 0;
        change_preset_name();
    }
}

void Preset::change_preset_name()
{
    //Print("Call change preset name...");
    const int _ind_codec = ui->comboBox_codec->currentIndex();
    const int _ind_mode = ui->comboBox_mode->currentIndex();
    const int _ind_pass = ui->comboBox_pass->currentIndex();
    const int _ind_preset = ui->comboBox_preset->currentIndex();
    const int _ind_acodec = ui->comboBox_audio_codec->currentIndex();
    const int _ind_abitrate = ui->comboBox_audio_bitrate->currentIndex();
    QVector<int> _ind = {_ind_codec, _ind_mode, _ind_pass,
                        _ind_preset, _ind_acodec, _ind_abitrate};
    if (_ind.indexOf(-1) != -1)
        return;

    Tables t;
    /************************************* Codec module ***************************************/   
    const QString codec = t.getCodecName(_ind_codec) + ", ";
    const QString selected_clrspace = t.arr_codec[_ind_codec][1];
    const QString clrspace = (selected_clrspace != "") ? selected_clrspace + QString(", ") : QString("");

    /************************************* Resize module ***************************************/

    QString res("");
    const QString w = ui->comboBox_width->currentText();
    const QString h = ui->comboBox_height->currentText();
    if (w != tr("Source") && h != tr("Source")) {
        res = w + "x" + h + ", ";
    } else if (w == tr("Source") && h == tr("Source")) {
        res = QString("Res: %1, ").arg(tr("Source"));
    } else {
        res = QString("Res: %1, ").arg(tr("Custom"));
    }

    /************************************* FPS module ***************************************/

    const QString f = ui->comboBoxFrameRate->currentText();
    const QString  fps = (f == tr("Source")) ? QString("Fps: %1, ").arg(tr("Source")) : f + " fps, ";

    /************************************* Mode module ***************************************/    
    QString mode("");
    const QString selected_mode = t.getCurrentMode(_ind_codec, _ind_mode);
    if ((selected_mode != "" && selected_mode != tr("Auto")) && (selected_mode == "CRF" || selected_mode == "CQP")) {
        mode = selected_mode + " " + ui->lineEdit_bitrate->text() + ", ";
    }
    else if ((selected_mode != "" && selected_mode != tr("Auto")) && (selected_mode != "CRF" && selected_mode != "CQP")) {
        mode = selected_mode + " " + ui->lineEdit_bitrate->text() + tr(" MBps, ");
    }

    /************************************* Preset module ***************************************/   
    QString preset("");
    QString p = t.arr_preset[_ind_codec][_ind_preset];
    if (p != "" && p != tr("None")) {
        preset = tr("Preset: ") + p + ", ";
    }

    /************************************* Pass module ***************************************/  
    QString pass("");
    const QString selected_pass = t.getCurrentPass(_ind_codec, _ind_pass);
    if (selected_pass != "" && selected_pass != tr("Auto")) {
        pass = selected_pass + ", ";
    }
    QString hdr("");
    /*if (t.arr_codec[_ind_codec][2] != "") {
        hdr = t.arr_codec[_ind_codec][2] + tr("Enabled, ");
    }*/

    /************************************* Audio module ***************************************/
    QString acodec("");
    if (t.arr_acodec[_ind_codec][_ind_acodec] != "") {
        acodec = tr("Audio: ") + t.arr_acodec[_ind_codec][_ind_acodec] + ", ";
    }

    QString abitrate("");
    if (t.arr_acodec[_ind_codec][_ind_acodec] == "AAC") {
        abitrate = t.arr_bitrate[0][_ind_abitrate] + ", ";
    } else
    if (t.arr_acodec[_ind_codec][_ind_acodec] == "AC3") {
        abitrate = t.arr_bitrate[1][_ind_abitrate] + ", ";
    } else
    if (t.arr_acodec[_ind_codec][_ind_acodec] == "DTS") {
        abitrate = t.arr_bitrate[2][_ind_abitrate] + ", ";
    } else
    if (t.arr_acodec[_ind_codec][_ind_acodec] == "Vorbis") {
        abitrate = t.arr_bitrate[3][_ind_abitrate] + ", ";
    } else
    if (t.arr_acodec[_ind_codec][_ind_acodec] == "Opus") {
        abitrate = t.arr_bitrate[4][_ind_abitrate] + ", ";
    }

    /************************************* Result module ***************************************/

    const QString container = ui->comboBox_container->currentText();
    QString cur_preset_name = codec + res + fps + clrspace + mode + preset + pass + hdr
            + acodec + abitrate + container;
    ui->textBrowser_presetname->setText(cur_preset_name);
}

void Preset::onComboBoxAspectRatio_indexChanged(int index)
{
    Tables t;
    const QString width = t.aspect_ratio[index][0];
    const QString height = t.aspect_ratio[index][1];
    const int widthIndex = ui->comboBox_width->findText(width, Qt::MatchCaseSensitive);
    const int heightIndex = ui->comboBox_height->findText(height, Qt::MatchCaseSensitive);

    if (widthIndex >= 0) {
        ui->comboBox_width->setCurrentIndex(widthIndex);
    }
    if (heightIndex >= 0) {
        ui->comboBox_height->setCurrentIndex(heightIndex);
    }
    const float width_= width.toFloat();
    const float height_ = height.toFloat();
    m_aspectRatio = (height_ != 0.0f ) ? round(10000 * width_ / height_)/10000 : 0.0f;
    //Print("AR: " << m_aspectRatio);
    m_repeat++;
}

void Preset::onComboBox_width_textChanged(const QString &arg1)
{
    lockSignals(true);

    Tables t;
    QString height = ui->comboBox_height->currentText();
    for (int i = 0; i < 34; i++) {
        if (t.aspect_ratio[i][0] == arg1 && t.aspect_ratio[i][1] == height) {
            ui->comboBoxAspectRatio->setCurrentIndex(i);
            break;
        } else {
            ui->comboBoxAspectRatio->setCurrentIndex(33);
        }
    }
    lockSignals(false);
    calculateDAR(arg1, height);
    m_repeat++;
}

void Preset::onComboBox_height_textChanged(const QString &arg1)
{
    lockSignals(true);
    Tables t;
    QString width = ui->comboBox_width->currentText();
    for (int i = 0; i < 34; i++) {
        if (t.aspect_ratio[i][0] == width && t.aspect_ratio[i][1] == arg1) {
            ui->comboBoxAspectRatio->setCurrentIndex(i);
            break;
        } else {
            ui->comboBoxAspectRatio->setCurrentIndex(33);
        }
    }
    lockSignals(false);
    calculateDAR(width, arg1);
    m_repeat++;
}

void Preset::onComboBoxFrameRate_indexChanged(int index)
{
    int codec = ui->comboBox_codec->currentIndex();
    bool blendingFlag = (codec >= CODEC_QSV_FIRST && codec <= CODEC_QSV_LAST) ||
            (codec >= CODEC_VAAPI_FIRST && codec <= CODEC_VAAPI_LAST) ? true : false;
    if (index == 0) {
        ui->comboBoxBlending->setCurrentIndex(0);
        ui->comboBoxBlending->setEnabled(false);
    } else {
        if (blendingFlag) {
            ui->comboBoxBlending->setCurrentIndex(0);
            ui->comboBoxBlending->setEnabled(false);
        } else {
            ui->comboBoxBlending->setEnabled(true);
        }
    }
    m_repeat++;
}

void Preset::calculateDAR(QString width, QString height)
{
    if (width == tr("Source") && height == tr("Source")) {
        ui->lineEdit_DAR->setText(tr("Source"));
    }
    else
    if (width == tr("Source") && height != tr("Source")) {
        ui->lineEdit_DAR->setText(tr("Undef"));
    }
    else
    if (width != tr("Source") && height == tr("Source")) {
        ui->lineEdit_DAR->setText(tr("Undef"));
    }
    else
    if (width != tr("Source") && height != tr("Source")) {
        int width_int = width.toInt();
        int height_int = height.toInt();

        int nod = 1;
        int mod = 1;
        int a = width_int;
        int b = height_int;
        if (a == b) {
            nod = a;
        }
        else if (a < b) {
            mod = a;
            a = b;
            b = mod;
            while (mod) {
                mod = a % b;
                if (mod) {
                    a = b;
                    b = mod;
                }
            }
            nod = b;
        }
        else if (a > b) {
            mod = b;
            b = a;
            a = mod;
            while (mod) {
                mod = b % a;
                if (mod) {
                    b = a;
                    a = mod;
                }
            }
            nod = a;
        }

        if (nod < 80) {
            double width_d = width.toDouble();
            double height_d = height.toDouble();
            double ar = width_d/height_d;
            ui->lineEdit_DAR->setText(QString::number(ar, 'f', 2) + QString(":1"));
        }
        else {
            int x = width_int/nod;
            int y = height_int/nod;
            ui->lineEdit_DAR->setText(QString::number(x) + ":" + QString::number(y));
        }
    }
}

/************************************************
** Video
************************************************/

void Preset::disableHDR()
{
    ui->comboBox_color_range->setCurrentIndex(0);
    ui->lineEdit_min_lum->clear();
    ui->lineEdit_max_lum->clear();
    ui->lineEdit_max_cll->clear();
    ui->lineEdit_max_fall->clear();
    ui->comboBox_master_disp->setCurrentIndex(0);
    ui->comboBox_color_range->setEnabled(false);
    ui->lineEdit_min_lum->setEnabled(false);
    ui->lineEdit_max_lum->setEnabled(false);
    ui->lineEdit_max_cll->setEnabled(false);
    ui->lineEdit_max_fall->setEnabled(false);
    ui->comboBox_master_disp->setEnabled(false);
}

void Preset::onComboBox_codec_textChanged(const QString &arg1)  // Change current codec
{
    const int row = ui->comboBox_codec->currentIndex();
    if (row != -1) {
        Tables t;
        lockSignals(true);
        //Print("Curr codec... " << row);
        ui->comboBoxAspectRatio->setEnabled(true);
        ui->comboBox_width->setEnabled(true);
        ui->comboBox_height->setEnabled(true);
        ui->comboBoxFrameRate->setEnabled(true);
        ui->comboBoxFrameRate->setCurrentIndex(0);
        ui->comboBox_container->setEnabled(true);
        ui->comboBox_mode->setEnabled(true);
        ui->comboBox_pass->setEnabled(true);
        ui->comboBox_preset->setEnabled(true);
        ui->comboBox_level->setEnabled(true);
        ui->comboBox_color_range->setEnabled(true);
        ui->comboBox_color_prim->setEnabled(true);
        ui->comboBox_color_matrix->setEnabled(true);
        ui->comboBox_transfer->setEnabled(true);
        ui->lineEdit_min_lum->setEnabled(true);
        ui->lineEdit_max_lum->setEnabled(true);
        ui->lineEdit_max_cll->setEnabled(true);
        ui->lineEdit_max_fall->setEnabled(true);
        ui->comboBox_master_disp->setEnabled(true);
        ui->comboBox_container->clear();
        ui->comboBox_mode->clear();
        ui->comboBox_pass->clear();
        ui->comboBox_preset->clear();
        ui->comboBox_level->clear();
        ui->comboBox_audio_codec->clear();

        ui->comboBox_container->addItems(t.getContainersListByRow(row));
        ui->comboBox_pass->addItems(t.getPassesListByRow(row));
        ui->comboBox_mode->addItems(t.getModesListByRow(row));
        ui->comboBox_preset->addItems(t.getPresetsListByRow(row));
        ui->comboBox_level->addItems(t.getLevelsListByRow(row));
        ui->comboBox_audio_codec->addItems(t.getAudioCodecsListByRow(row));

        if (arg1 == tr("H.265/HEVC 4:2:0 12 bit")) {
            ui->comboBox_container->setCurrentIndex(2);
            ui->comboBox_profile->setCurrentIndex(Profile::MAIN12);
            ui->comboBox_preset->setCurrentIndex(6);
            ui->comboBox_level->setCurrentIndex(0);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P12LE);
        }

        else if (arg1 == tr("H.265/HEVC 4:2:0 10 bit")) {
            ui->comboBox_container->setCurrentIndex(2);
            ui->comboBox_profile->setCurrentIndex(Profile::MAIN10);
            ui->comboBox_preset->setCurrentIndex(6);
            ui->comboBox_level->setCurrentIndex(0);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P10LE);
        }

        else if (arg1 == tr("H.265/HEVC 4:2:0 8 bit")) {
            ui->comboBox_container->setCurrentIndex(2);
            ui->comboBox_profile->setCurrentIndex(Profile::MAIN);
            ui->comboBox_preset->setCurrentIndex(6);
            ui->comboBox_level->setCurrentIndex(0);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P);
            disableHDR();
        }

        else if (arg1 == tr("H.264/AVC 4:2:0 8 bit")) {
            ui->comboBox_container->setCurrentIndex(2);
            ui->comboBox_profile->setCurrentIndex(Profile::HIGH);
            ui->comboBox_preset->setCurrentIndex(6);
            ui->comboBox_level->setCurrentIndex(0);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P);
            disableHDR();
        }

        else if (arg1 == tr("VP9 4:2:0 10 bit")) {
            ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_2);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P10LE);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_level->setEnabled(false);
        }

        else if (arg1 == tr("VP9 4:2:0 8 bit")) {
            ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_1);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_level->setEnabled(false);
            disableHDR();
        }

        else if (arg1 == tr("Intel QSV H.265/HEVC 4:2:0 10 bit")) {
            ui->comboBox_container->setCurrentIndex(2);
            ui->comboBoxAspectRatio->setCurrentIndex(0);
            ui->comboBoxAspectRatio->setEnabled(false);
            ui->comboBox_width->setEnabled(false);
            ui->comboBox_height->setEnabled(false);
            ui->comboBoxFrameRate->setEnabled(false);
            ui->comboBox_profile->setCurrentIndex(Profile::MAIN10);
            ui->comboBox_preset->setCurrentIndex(4);
            ui->comboBox_level->setCurrentIndex(0);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
        }

        else if (arg1 == tr("Intel QSV H.265/HEVC 4:2:0 8 bit")) {
            ui->comboBox_container->setCurrentIndex(2);
            ui->comboBoxAspectRatio->setCurrentIndex(0);
            ui->comboBoxAspectRatio->setEnabled(false);
            ui->comboBox_width->setEnabled(false);
            ui->comboBox_height->setEnabled(false);
            ui->comboBoxFrameRate->setEnabled(false);
            ui->comboBox_profile->setCurrentIndex(Profile::MAIN);
            ui->comboBox_preset->setCurrentIndex(4);
            ui->comboBox_level->setCurrentIndex(0);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
            disableHDR();
        }

        else if (arg1 == tr("Intel QSV H.264/AVC 4:2:0 8 bit")) {
            ui->comboBox_container->setCurrentIndex(2);
            ui->comboBoxAspectRatio->setCurrentIndex(0);
            ui->comboBoxAspectRatio->setEnabled(true);
            ui->comboBox_width->setEnabled(true);
            ui->comboBox_height->setEnabled(true);
            ui->comboBoxFrameRate->setEnabled(true);
            ui->comboBox_profile->setCurrentIndex(Profile::HIGH);
            ui->comboBox_preset->setCurrentIndex(4);
            ui->comboBox_level->setCurrentIndex(0);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
            ui->comboBox_mode->setEnabled(true);
            ui->comboBox_pass->setEnabled(false);
            disableHDR();
        }

        else if (arg1 == tr("Intel QSV VP9 4:2:0 10 bit")) {
            ui->comboBoxAspectRatio->setCurrentIndex(0);
            ui->comboBoxAspectRatio->setEnabled(false);
            ui->comboBox_width->setEnabled(false);
            ui->comboBox_height->setEnabled(false);
            ui->comboBoxFrameRate->setEnabled(false);
            ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_2);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
            ui->comboBox_level->setEnabled(false);
        }

        else if (arg1 == tr("Intel QSV VP9 4:2:0 8 bit")) {
            ui->comboBoxAspectRatio->setCurrentIndex(0);
            ui->comboBoxAspectRatio->setEnabled(false);
            ui->comboBox_width->setEnabled(false);
            ui->comboBox_height->setEnabled(false);
            ui->comboBoxFrameRate->setEnabled(false);
            ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_1);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
            ui->comboBox_level->setEnabled(false);
            disableHDR();
        }

        else if (arg1 == tr("Intel QSV MPEG-2 4:2:0 8 bit")) {
            ui->comboBox_container->setCurrentIndex(3);
            ui->comboBoxAspectRatio->setCurrentIndex(0);
            ui->comboBoxAspectRatio->setEnabled(false);
            ui->comboBox_width->setEnabled(false);
            ui->comboBox_height->setEnabled(false);
            ui->comboBoxFrameRate->setEnabled(false);
            ui->comboBox_profile->setCurrentIndex(Profile::HIGH);
            ui->comboBox_preset->setCurrentIndex(4);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
            ui->comboBox_level->setEnabled(false);
            disableHDR();
        }

        else if (arg1 == tr("Intel VAAPI H.264/AVC 4:2:0 8 bit")) {
            ui->comboBox_container->setCurrentIndex(2);
            ui->comboBoxAspectRatio->setCurrentIndex(0);
            ui->comboBoxAspectRatio->setEnabled(true);
            ui->comboBox_width->setEnabled(true);
            ui->comboBox_height->setEnabled(true);
            ui->comboBoxFrameRate->setEnabled(true);
            ui->comboBox_profile->setCurrentIndex(Profile::HIGH);
            ui->comboBox_preset->setCurrentIndex(4);
            ui->comboBox_level->setCurrentIndex(0);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
            ui->comboBox_mode->setEnabled(true);
            ui->comboBox_pass->setEnabled(false);
            disableHDR();
        }

        else if (arg1 == tr("NVENC H.265/HEVC 4:2:0 10 bit")) {
            ui->comboBox_container->setCurrentIndex(2);
            ui->comboBox_profile->setCurrentIndex(Profile::MAIN10);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::P010LE);
            ui->comboBox_preset->setCurrentIndex(1);
            ui->comboBox_level->setCurrentIndex(0);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
        }

        else if (arg1 == tr("NVENC H.265/HEVC 4:2:0 8 bit")) {
            ui->comboBox_container->setCurrentIndex(2);
            ui->comboBox_profile->setCurrentIndex(Profile::MAIN);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P);
            ui->comboBox_preset->setCurrentIndex(1);
            ui->comboBox_level->setCurrentIndex(0);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
            disableHDR();
        }

        else if (arg1 == tr("NVENC H.264/AVC 4:2:0 8 bit")) {
            ui->comboBox_container->setCurrentIndex(2);
            ui->comboBox_profile->setCurrentIndex(Profile::HIGH);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P);
            ui->comboBox_preset->setCurrentIndex(1);
            ui->comboBox_level->setCurrentIndex(0);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
            disableHDR();
        }

        else if (arg1 == tr("ProRes Proxy")) {
            ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_0);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422P10LE);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_container->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
        }

        else if (arg1 == "ProRes LT") {
            ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_1);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422P10LE);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_container->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
        }

        else if (arg1 == tr("ProRes Standard")) {
            ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_2);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422P10LE);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_container->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
        }

        else if (arg1 == "ProRes HQ") {
            ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_3);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422P10LE);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_container->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
        }

        else if (arg1 == "ProRes 4444") {
            ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_4);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV444P10LE);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_container->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
        }

        else if (arg1 == "ProRes 4444 XQ") {
            ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_5);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV444P10LE);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_container->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
        }

        else if (arg1 == "DNxHR LB") {
            ui->comboBox_profile->setCurrentIndex(Profile::DNXHR_LB);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422p);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_container->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
            disableHDR();
        }

        else if (arg1 == "DNxHR SQ") {
            ui->comboBox_profile->setCurrentIndex(Profile::DNXHR_SQ);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422p);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_container->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
            disableHDR();
        }

        else if (arg1 == "DNxHR HQ") {
            ui->comboBox_profile->setCurrentIndex(Profile::DNXHR_HQ);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422p);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_container->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
            disableHDR();
        }

        else if (arg1 == "DNxHR HQX") {
            ui->comboBox_profile->setCurrentIndex(Profile::DNXHR_HQX);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422P10LE);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_container->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
        }

        else if (arg1 == "DNxHR 444") {
            ui->comboBox_profile->setCurrentIndex(Profile::DNXHR_444);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV444P10LE);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_container->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
        }

        else if (arg1 == "XDCAM HD422") {
            ui->comboBoxFrameRate->setCurrentIndex(8);
            ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_0);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422p);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_container->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
            disableHDR();
        }

        else if (arg1 == "XAVC 4:2:2") {
            ui->comboBoxFrameRate->setCurrentIndex(8);
            ui->lineEdit_bitrate->setText("480");
            ui->lineEdit_bufsize->setText("480");
            ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_AUTO);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422p);
            ui->comboBox_container->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
            disableHDR();
        }

        else if (arg1 == tr("Source")) {
            ui->comboBox_container->setCurrentIndex(2);
            ui->comboBoxAspectRatio->setCurrentIndex(0);
            ui->comboBoxAspectRatio->setEnabled(false);
            ui->comboBox_width->setEnabled(false);
            ui->comboBox_height->setEnabled(false);
            ui->comboBoxFrameRate->setCurrentIndex(0);
            ui->comboBoxFrameRate->setEnabled(false);
            ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_AUTO);
            ui->comboBox_level->setEnabled(false);
            ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
            ui->comboBox_preset->setEnabled(false);
            ui->comboBox_mode->setEnabled(false);
            ui->comboBox_pass->setEnabled(false);
            ui->comboBox_color_prim->setEnabled(false);
            ui->comboBox_color_matrix->setEnabled(false);
            ui->comboBox_transfer->setEnabled(false);
            ui->comboBox_color_prim->setCurrentIndex(0);
            ui->comboBox_color_matrix->setCurrentIndex(0);
            ui->comboBox_transfer->setCurrentIndex(0);
        }
        lockSignals(false);
    }

}

void Preset::onComboBox_mode_textChanged(const QString &arg1)  // Change curret mode
{
    lockSignals(true);
    //Print("Change current mode...");
    ui->lineEdit_bitrate->clear();
    ui->lineEdit_minrate->clear();
    ui->lineEdit_maxrate->clear();
    ui->lineEdit_bufsize->clear();
    ui->lineEdit_bitrate->setEnabled(true);
    ui->lineEdit_minrate->setEnabled(true);
    ui->lineEdit_maxrate->setEnabled(true);
    ui->lineEdit_bufsize->setEnabled(true);
    ui->comboBox_pass->setEnabled(true);
    if (arg1 == tr("Auto")) {
        ui->label_bitrate->setText(tr("Bitrate"));
        ui->label_minrate->show();
        ui->label_maxrate->show();
        ui->label_bufsize->show();
        ui->label_bitrate_prefix->setText(tr("MBps"));
        ui->label_minrate_prefix->show();
        ui->label_maxrate_prefix->show();
        ui->label_bufsize_prefix->show();
        ui->lineEdit_minrate->show();
        ui->lineEdit_maxrate->show();
        ui->lineEdit_bufsize->show();
        ui->lineEdit_bitrate->setText(tr("Auto"));
        ui->lineEdit_minrate->setText(tr("Auto"));
        ui->lineEdit_maxrate->setText(tr("Auto"));
        ui->lineEdit_bufsize->setText(tr("Auto"));
        ui->lineEdit_bitrate->setEnabled(false);
        ui->lineEdit_minrate->setEnabled(false);
        ui->lineEdit_maxrate->setEnabled(false);
        ui->lineEdit_bufsize->setEnabled(false);
    }
    else if (arg1 == tr("Constant Bitrate")) {
        ui->label_bitrate->setText(tr("Bitrate"));
        ui->label_bitrate_prefix->setText(tr("MBps"));
        ui->label_minrate->hide();
        ui->label_maxrate->hide();
        ui->label_bufsize->show();
        ui->label_minrate_prefix->hide();
        ui->label_maxrate_prefix->hide();
        ui->label_bufsize_prefix->show();
        ui->lineEdit_minrate->hide();
        ui->lineEdit_maxrate->hide();
        ui->lineEdit_bufsize->show();
        ui->lineEdit_bitrate->setText("50");
        ui->lineEdit_bufsize->setText("50");
    }
    else if (arg1 == tr("Average Bitrate")) {
        ui->label_bitrate->setText(tr("Bitrate"));
        ui->label_minrate->hide();
        ui->label_maxrate->hide();
        ui->label_bufsize->hide();
        ui->label_bitrate_prefix->setText(tr("MBps"));
        ui->label_minrate_prefix->hide();
        ui->label_maxrate_prefix->hide();
        ui->label_bufsize_prefix->hide();
        ui->lineEdit_minrate->hide();
        ui->lineEdit_maxrate->hide();
        ui->lineEdit_bufsize->hide();
        ui->lineEdit_bitrate->setText("50");
    }
    else if (arg1 == tr("Variable Bitrate")) {
        ui->label_bitrate->setText(tr("Bitrate"));
        ui->label_bitrate_prefix->setText(tr("MBps"));
        ui->label_minrate->show();
        ui->label_maxrate->show();
        ui->label_bufsize->show();
        ui->label_minrate_prefix->show();
        ui->label_maxrate_prefix->show();
        ui->label_bufsize_prefix->show();
        ui->lineEdit_minrate->show();
        ui->lineEdit_maxrate->show();
        ui->lineEdit_bufsize->show();
        ui->lineEdit_bitrate->setText("50");
        ui->lineEdit_minrate->setText("50");
        ui->lineEdit_maxrate->setText("50");
        ui->lineEdit_bufsize->setText("50");
    }
    else if (arg1 == tr("Constant Rate Factor")) {
        ui->label_bitrate->setText(tr("Rate factor"));
        ui->label_minrate->hide();
        ui->label_maxrate->hide();
        ui->label_bufsize->hide();
        ui->label_bitrate_prefix->setText("");
        ui->label_minrate_prefix->hide();
        ui->label_maxrate_prefix->hide();
        ui->label_bufsize_prefix->hide();
        ui->lineEdit_minrate->hide();
        ui->lineEdit_maxrate->hide();
        ui->lineEdit_bufsize->hide();
        ui->lineEdit_bitrate->setText("19");
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_pass->setCurrentIndex(0);
    }
    else if (arg1 == tr("Constant QP")) {
        ui->label_bitrate->setText(tr("Quantizer"));
        ui->label_minrate->hide();
        ui->label_maxrate->hide();
        ui->label_bufsize->hide();
        ui->label_bitrate_prefix->setText("");
        ui->label_minrate_prefix->hide();
        ui->label_maxrate_prefix->hide();
        ui->label_bufsize_prefix->hide();
        ui->lineEdit_minrate->hide();
        ui->lineEdit_maxrate->hide();
        ui->lineEdit_bufsize->hide();
        ui->lineEdit_bitrate->setText("19");
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_pass->setCurrentIndex(0);
    }
    lockSignals(false);
    m_repeat++;
}

void Preset::onComboBox_preset_indexChanged(int index) // Preset index changed
{
    //Print("Index of Preset: " << index);
    Q_UNUSED(index)
    m_repeat++;
}

void Preset::onComboBox_pass_indexChanged(int index)  // Pass index changed
{
    //Print("Index of Pass: " << index);
    Q_UNUSED(index)
    m_repeat++;
}

void Preset::onComboBox_container_textChanged()
{
    m_repeat++;
}

void Preset::onLineEdit_bitrate_editingFinished()
{
    m_repeat++;
}

/************************************************
** Audio
************************************************/

void Preset::onComboBox_audio_codec_textChanged(const QString &arg1) // Change current audio codec
{
    lockSignals(true);
    //Print("Change current audio codec...");
    ui->comboBox_audio_bitrate->setEnabled(true);
    ui->comboBox_audio_bitrate->clear();
    ui->comboBox_audio_sampling->setEnabled(true);
    ui->comboBox_audio_channels->setEnabled(true);
    if (arg1 == tr("Source")) {
        ui->comboBox_audio_bitrate->addItems({tr("Source")});
        ui->comboBox_audio_bitrate->setEnabled(false);
        ui->comboBox_audio_sampling->setCurrentIndex(0);
        ui->comboBox_audio_sampling->setEnabled(false);
        ui->comboBox_audio_channels->setCurrentIndex(0);
        ui->comboBox_audio_channels->setEnabled(false);
    }
    else if (arg1 == "AAC") {
        ui->comboBox_audio_bitrate->addItems({"384k", "320k", "256k", "192k", "128k", "96k"});
        ui->comboBox_audio_bitrate->setCurrentIndex(2);
    }
    else if (arg1 == "AC3") {
        ui->comboBox_audio_bitrate->addItems({"640k", "448k", "384k", "256k"});
        ui->comboBox_audio_bitrate->setCurrentIndex(1);
    }
    else if (arg1 == "DTS") {
        ui->comboBox_audio_bitrate->addItems({"3840k", "3072k", "2048k", "1920k", "1536k", "1472k", "1344k", "1280k", "1152k",
                                                     "1024k", "960k", "768k", "640k", "576k", "512k", "448k", "384k"});
        ui->comboBox_audio_bitrate->setCurrentIndex(4);
    }
    else if (arg1 == "Vorbis") {
        ui->comboBox_audio_bitrate->addItems({"448k", "384k", "256k", "128k", "96k", "64k"});
        ui->comboBox_audio_bitrate->setCurrentIndex(2);
    }
    else if (arg1 == "Opus") {
        ui->comboBox_audio_bitrate->addItems({"448k", "384k", "256k", "128k", "96k", "64k"});
        ui->comboBox_audio_bitrate->setCurrentIndex(2);
    }
    else if (arg1 == "PCM 16 bit") {
        ui->comboBox_audio_bitrate->addItems({tr("Auto")});
        ui->comboBox_audio_bitrate->setEnabled(false);
    }
    else if (arg1 == "PCM 24 bit") {
        ui->comboBox_audio_bitrate->addItems({tr("Auto")});
        ui->comboBox_audio_bitrate->setEnabled(false);
    }
    else if (arg1 == "PCM 32 bit") {
        ui->comboBox_audio_bitrate->addItems({tr("Auto")});
        ui->comboBox_audio_bitrate->setEnabled(false);
    }
    m_repeat++;
    lockSignals(false);
}

void Preset::onComboBox_audio_bitrate_textChanged()  // Abitrate currentText changed
{
    m_repeat++;
}

/************************************************
** HDR Metadata
************************************************/

void Preset::onComboBox_master_disp_textChanged(const QString &arg1)  // Change current master display
{
    lockSignals(true);
    //Print("Change current master display...");
    ui->lineEdit_chroma_coord->clear();
    ui->lineEdit_white_coord->clear();
    ui->lineEdit_chroma_coord->setEnabled(false);
    ui->lineEdit_white_coord->setEnabled(false);
    if (arg1 == tr("Unsprt")) {
        ui->lineEdit_chroma_coord->setText(tr("Unsprt"));
        ui->lineEdit_white_coord->setText(tr("Unsprt"));
    }
    else if (arg1 == tr("Source")) {
        ui->lineEdit_chroma_coord->setText(tr("Source"));
        ui->lineEdit_white_coord->setText(tr("Source"));
    }
    else if (arg1 == tr("Custom")) {
        ui->lineEdit_chroma_coord->setText("0.680,0.320,0.265,0.690,0.150,0.060");
        ui->lineEdit_white_coord->setText("0.3127,0.3290");
        ui->lineEdit_chroma_coord->setEnabled(true);
        ui->lineEdit_white_coord->setEnabled(true);
    }
    else if (arg1 == tr("Display P3")) {
        ui->lineEdit_chroma_coord->setText("0.680,0.320,0.265,0.690,0.150,0.060");
        ui->lineEdit_white_coord->setText("0.3127,0.3290");
    }
    else if (arg1 == tr("DCI P3")) {
        ui->lineEdit_chroma_coord->setText("0.680,0.320,0.265,0.690,0.150,0.060");
        ui->lineEdit_white_coord->setText("0.314,0.3510");
    }
    else if (arg1 == "BT.2020") {
        ui->lineEdit_chroma_coord->setText("0.708,0.292,0.170,0.797,0.131,0.046");
        ui->lineEdit_white_coord->setText("0.3127,0.3290");
    }
    else if (arg1 == "BT.709") {
        ui->lineEdit_chroma_coord->setText("0.640,0.330,0.30,0.60,0.150,0.060");
        ui->lineEdit_white_coord->setText("0.3127,0.3290");
    }
    lockSignals(false);
}
