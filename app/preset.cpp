/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: preset.cpp
 MODIFIED: November, 2021
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "preset.h"
#include "ui_preset.h"



Preset::Preset(QWidget *parent):
    QDialog(parent),
    ui(new Ui::Preset),
    _expandWindowsState(false),
    clickPressedFlag(false),
    clickPressedToResizeFlag(8, false),
    mouseClickCoordinate(QPoint())
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);

    ui->widget_main->installEventFilter(this);
    ui->widget_main->setAttribute(Qt::WA_Hover, true);

    ui->frame_main->installEventFilter(this);
    ui->frame_main->setAttribute(Qt::WA_Hover, true);
    ui->frame_main->setAttribute(Qt::WA_NoMousePropagation, true);

    ui->frame_top->installEventFilter(this);
}

Preset::~Preset()
{
    delete ui;
}

void Preset::closeEvent(QCloseEvent *close_preset)  /*** Show prompt when close app ***/
{
    close_preset->ignore();
    *_ptr_presetWindowGeometry = this->saveGeometry();
    close_preset->accept();
}

bool Preset::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            ui->frame_middle->setFocus();
            return true;
        }
        return QDialog::eventFilter(watched, event);
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->button() == Qt::LeftButton) {
            QGuiApplication::restoreOverrideCursor();
            clickPressedFlag = false;
            clickPressedToResizeFlag.fill(false);
            return true;
        }
        return QDialog::eventFilter(watched, event);
    }

    if (watched == ui->widget_main) {   // ********** Resize window realisation *********** //
        if (!this->isMaximized()) {
            if (event->type() == QEvent::HoverLeave) {
                QGuiApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
                return true;

            }
            if (event->type() == QEvent::HoverMove && clickPressedToResizeFlag.indexOf(true) == -1) {
                const QPoint &&mouseCoordinate = ui->widget_main->mapFromGlobal(QCursor::pos());
                if (mouseCoordinate.x() < 6) {
                    if (mouseCoordinate.y() < 6) {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                        return true;
                    }
                    if (mouseCoordinate.y() > 6 && mouseCoordinate.y() < (height() - 6)) {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                        return true;
                    }
                    if (mouseCoordinate.y() > (height() - 6)) {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                        return true;
                    }
                    QGuiApplication::restoreOverrideCursor();
                    return QDialog::eventFilter(watched, event);
                }
                if (mouseCoordinate.x() > 6 && mouseCoordinate.x() < (width() - 6)) {
                    if (mouseCoordinate.y() < 6 || mouseCoordinate.y() > (height() - 6)) {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                        return true;
                    }
                    QGuiApplication::restoreOverrideCursor();
                    return QDialog::eventFilter(watched, event);
                }
                if (mouseCoordinate.x() > (width() - 6)) {
                    if (mouseCoordinate.y() < 6) {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                        return true;
                    }
                    if (mouseCoordinate.y() > 6 && mouseCoordinate.y() < (height() - 6)) {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                        return true;
                    }
                    if (mouseCoordinate.y() > (height() - 6)) {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                        return true;
                    }
                    QGuiApplication::restoreOverrideCursor();
                    return QDialog::eventFilter(watched, event);
                }
                QGuiApplication::restoreOverrideCursor();
                return QDialog::eventFilter(watched, event);
            }
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->button() == Qt::LeftButton) {
                    oldWidth = this->width();
                    oldHeight = this->height();
                    mouseClickCoordinate = mouse_event->pos();
                    if (mouseClickCoordinate.x() < 6) {
                        if (mouseClickCoordinate.y() < 6) {
                            clickPressedToResizeFlag[Resize::LEFT_TOP] = true;
                            return true;
                        }
                        if (mouseClickCoordinate.y() > 6 && mouseClickCoordinate.y() < (oldHeight - 6)) {
                            clickPressedToResizeFlag[Resize::LEFT] = true;
                            return true;
                        }
                        if (mouseClickCoordinate.y() > (oldHeight - 6)) {
                            clickPressedToResizeFlag[Resize::LEFT_BOTTOM] = true;
                            return true;
                        }
                        return QDialog::eventFilter(watched, event);
                    }
                    if (mouseClickCoordinate.x() > 6 && mouseClickCoordinate.x() < (oldWidth - 6)) {
                        if (mouseClickCoordinate.y() < 6) {
                            clickPressedToResizeFlag[Resize::TOP] = true;
                            return true;
                        }
                        if (mouseClickCoordinate.y() > (oldHeight - 6)) {
                            clickPressedToResizeFlag[Resize::BOTTOM] = true;
                            return true;
                        }
                        return QDialog::eventFilter(watched, event);
                    }
                    if (mouseClickCoordinate.x() > (oldWidth - 6)) {
                        if (mouseClickCoordinate.y() < 6) {
                            clickPressedToResizeFlag[Resize::RIGHT_TOP] = true;
                            return true;
                        }
                        if (mouseClickCoordinate.y() > 6 && mouseClickCoordinate.y() < (oldHeight - 6)) {
                            clickPressedToResizeFlag[Resize::RIGHT] = true;
                            return true;
                        }
                        if (mouseClickCoordinate.y() > (oldHeight - 6)) {
                            clickPressedToResizeFlag[Resize::RIGHT_BOTTOM] = true;
                            return true;
                        }
                        return QDialog::eventFilter(watched, event);
                    }
                    return QDialog::eventFilter(watched, event);
                }
                return QDialog::eventFilter(watched, event);
            }
            if (event->type() == QEvent::MouseMove) {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->buttons() & Qt::LeftButton) {
                    int index = clickPressedToResizeFlag.indexOf(true);
                    if (index != -1) {
                        const int &&deltaX = mouse_event->globalPos().x() - mouseClickCoordinate.x();
                        const int &&deltaY = mouse_event->globalPos().y() - mouseClickCoordinate.y();
                        const int &&deltaWidth = static_cast<int>(mouse_event->localPos().x()) - mouseClickCoordinate.x();
                        const int &&deltaHeight = static_cast<int>(mouse_event->localPos().y()) - mouseClickCoordinate.y();
                        switch (index) {
                        case Resize::LEFT:
                            setGeometry(deltaX, pos().y(), width() - deltaWidth, oldHeight);
                            break;
                        case Resize::LEFT_TOP:
                            setGeometry(deltaX, deltaY, width() - deltaWidth, height() - deltaHeight);
                            break;
                        case Resize::LEFT_BOTTOM:
                            setGeometry(deltaX, pos().y(), width() - deltaWidth, oldHeight + deltaHeight);
                            break;
                        case Resize::TOP:
                            setGeometry(pos().x(), deltaY, oldWidth, height() - deltaHeight);
                            break;
                        case Resize::RIGHT:
                            setGeometry(pos().x(), pos().y(), oldWidth + deltaWidth, oldHeight);
                            break;
                        case Resize::RIGHT_TOP:
                            setGeometry(pos().x(), deltaY, oldWidth + deltaWidth, height() - deltaHeight);
                            break;
                        case Resize::RIGHT_BOTTOM:
                            setGeometry(pos().x(), pos().y(), oldWidth + deltaWidth, oldHeight + deltaHeight);
                            break;
                        case Resize::BOTTOM:
                            setGeometry(pos().x(), pos().y(), oldWidth, oldHeight + deltaHeight);
                            break;
                        }
                        return true;
                    }
                    return QDialog::eventFilter(watched, event);
                }
                return QDialog::eventFilter(watched, event);
            }
            return QDialog::eventFilter(watched, event);
        }
        return QDialog::eventFilter(watched, event);
    }

    if (watched == ui->frame_main) {   // ******** Restore cursor realisation ********** //
        if (event->type() == QEvent::HoverMove) {
            QGuiApplication::restoreOverrideCursor();
            return true;
        }
        return QDialog::eventFilter(watched, event);
    }

    if (watched == ui->frame_top) {    // ******* Drag and expand window realisation ******* //
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton) {
                mouseClickCoordinate = mouse_event->pos();
                clickPressedFlag = true;
                return true;
            }
            return QDialog::eventFilter(watched, event);
        }
        if ((event->type() == QEvent::MouseMove) && clickPressedFlag) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton) {
                if (this->isMaximized()) on_expandWindow_clicked();
                this->move(mouse_event->globalPos() - mouseClickCoordinate);
                return true;
            }
            return QDialog::eventFilter(watched, event);
        }
        if (event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton) {
                on_expandWindow_clicked();
                return true;
            }
            return QDialog::eventFilter(watched, event);
        }
        return QDialog::eventFilter(watched, event);
    }
    return QDialog::eventFilter(watched, event);
}

void Preset::setParameters(QByteArray *ptr_presetWindowGeometry, QVector<QString> *_old_param)  /*** Set parameters ***/
{
    _new_param = _old_param;
    _ptr_presetWindowGeometry = ptr_presetWindowGeometry;
    if (*_ptr_presetWindowGeometry != "default") {
        this->restoreGeometry(*_ptr_presetWindowGeometry);
        if (this->isMaximized()) {
            _expandWindowsState = true;
        }
    }
    _repeat = 0;
    _aspectRatio = 0.0f;
    timer = new QTimer(this);
    timer->setInterval(450);
    connect(timer, SIGNAL(timeout()), this, SLOT(repeat_handler()));
    timer->start();

    QFont font;
    font.setPointSize(10);
    ui->label_title->setFont(font);
    QListView *comboboxAspectRatioListView = new QListView(ui->comboBoxAspectRatio);
    QListView *comboboxWidthListView = new QListView(ui->comboBox_width);
    QListView *comboboxHeightListView = new QListView(ui->comboBox_height);
    QListView *comboboxFrameRateListView = new QListView(ui->comboBoxFrameRate);
    QListView *comboboxBlendingListView = new QListView(ui->comboBoxBlending);
    QListView *comboboxCodecListView = new QListView(ui->comboBox_codec);
    QListView *comboboxModeListView = new QListView(ui->comboBox_mode);
    QListView *comboboxContainerListView = new QListView(ui->comboBox_container);
    QListView *comboboxPassListView = new QListView(ui->comboBox_pass);
    QListView *comboboxPresetListView = new QListView(ui->comboBox_preset);
    QListView *comboboxLevelListView = new QListView(ui->comboBox_level);
    QListView *comboboxACodecListView = new QListView(ui->comboBox_audio_codec);
    QListView *comboboxABitrateListView = new QListView(ui->comboBox_audio_bitrate);
    QListView *comboboxASamplingListView = new QListView(ui->comboBox_audio_sampling);
    QListView *comboboxAChannelsListView = new QListView(ui->comboBox_audio_channels);
    QListView *comboboxColorRangeListView = new QListView(ui->comboBox_color_range);
    QListView *comboboxColorPrimListView = new QListView(ui->comboBox_color_prim);
    QListView *comboboxColorMatrixListView = new QListView(ui->comboBox_color_matrix);
    QListView *comboboxTransferListView = new QListView(ui->comboBox_transfer);
    QListView *comboboxMasterDisplayListView = new QListView(ui->comboBox_master_disp);
    comboboxAspectRatioListView->setTextElideMode(Qt::ElideMiddle);
    ui->comboBoxAspectRatio->setView(comboboxAspectRatioListView);
    ui->comboBox_width->setView(comboboxWidthListView);
    ui->comboBox_height->setView(comboboxHeightListView);
    ui->comboBoxFrameRate->setView(comboboxFrameRateListView);
    ui->comboBoxBlending->setView(comboboxBlendingListView);
    comboboxCodecListView->setTextElideMode(Qt::ElideMiddle);
    ui->comboBox_codec->setView(comboboxCodecListView);
    ui->comboBox_mode->setView(comboboxModeListView);
    ui->comboBox_container->setView(comboboxContainerListView);
    ui->comboBox_pass->setView(comboboxPassListView);
    ui->comboBox_preset->setView(comboboxPresetListView);
    ui->comboBox_level->setView(comboboxLevelListView);
    ui->comboBox_audio_codec->setView(comboboxACodecListView);
    ui->comboBox_audio_bitrate->setView(comboboxABitrateListView);
    ui->comboBox_audio_sampling->setView(comboboxASamplingListView);
    ui->comboBox_audio_channels->setView(comboboxAChannelsListView);
    ui->comboBox_color_range->setView(comboboxColorRangeListView);
    ui->comboBox_color_prim->setView(comboboxColorPrimListView);
    ui->comboBox_color_matrix->setView(comboboxColorMatrixListView);
    ui->comboBox_transfer->setView(comboboxTransferListView);
    ui->comboBox_master_disp->setView(comboboxMasterDisplayListView);

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

    ui->textBrowser_presetname->setText((*_new_param)[Parameters::_OUTPUT_PARAM]);
    ui->comboBox_codec->setCurrentIndex((*_new_param)[Parameters::_CODEC].toInt());
    ui->comboBox_mode->setCurrentIndex((*_new_param)[Parameters::_MODE].toInt());
    ui->comboBox_container->setCurrentIndex((*_new_param)[Parameters::_CONTAINER].toInt());
    ui->lineEdit_bitrate->setText((*_new_param)[Parameters::_BQR]);
    ui->lineEdit_minrate->setText((*_new_param)[Parameters::_MINRATE]);
    ui->lineEdit_maxrate->setText((*_new_param)[Parameters::_MAXRATE]);
    ui->lineEdit_bufsize->setText((*_new_param)[Parameters::_BUFSIZE]);
    ui->comboBox_level->setCurrentIndex((*_new_param)[Parameters::_LEVEL].toInt());
    ui->comboBoxFrameRate->setCurrentIndex((*_new_param)[Parameters::_FRAME_RATE].toInt());
    ui->comboBoxBlending->setCurrentIndex((*_new_param)[Parameters::_BLENDING].toInt());
    ui->comboBox_width->setCurrentIndex((*_new_param)[Parameters::_WIDTH].toInt());
    ui->comboBox_height->setCurrentIndex((*_new_param)[Parameters::_HEIGHT].toInt());
    ui->comboBox_pass->setCurrentIndex((*_new_param)[Parameters::_PASS].toInt());
    ui->comboBox_preset->setCurrentIndex((*_new_param)[Parameters::_PRESET].toInt());
    ui->comboBox_color_range->setCurrentIndex((*_new_param)[Parameters::_COLOR_RANGE].toInt());
    ui->comboBox_color_prim->setCurrentIndex((*_new_param)[Parameters::_PRIMARY].toInt());
    ui->comboBox_color_matrix->setCurrentIndex((*_new_param)[Parameters::_MATRIX].toInt());
    ui->comboBox_transfer->setCurrentIndex((*_new_param)[Parameters::_TRC].toInt());
    ui->lineEdit_min_lum->setText((*_new_param)[Parameters::_MIN_LUM]);
    ui->lineEdit_max_lum->setText((*_new_param)[Parameters::_MAX_LUM]);
    ui->lineEdit_max_cll->setText((*_new_param)[Parameters::_MAX_CLL]);
    ui->lineEdit_max_fall->setText((*_new_param)[Parameters::_MAX_FALL]);
    ui->comboBox_master_disp->setCurrentIndex((*_new_param)[Parameters::_MASTER_DISPLAY].toInt());
    ui->lineEdit_chroma_coord->setText((*_new_param)[Parameters::_CHROMA_COORD]);
    ui->lineEdit_white_coord->setText((*_new_param)[Parameters::_WHITE_COORD]);
    ui->comboBox_audio_codec->setCurrentIndex((*_new_param)[Parameters::_AUDIO_CODEC].toInt());
    ui->comboBox_audio_bitrate->setCurrentIndex((*_new_param)[Parameters::_AUDIO_BITRATE].toInt());
    ui->comboBox_audio_sampling->setCurrentIndex((*_new_param)[Parameters::_ASAMPLE_RATE].toInt());
    ui->comboBox_audio_channels->setCurrentIndex((*_new_param)[Parameters::_ACHANNELS].toInt());
    ui->checkBox_primaries->setCheckState((Qt::CheckState)(*_new_param)[Parameters::_REP_PRIM].toInt());
    ui->checkBox_matrix->setCheckState((Qt::CheckState)(*_new_param)[Parameters::_REP_MATRIX].toInt());
    ui->checkBox_transfer->setCheckState((Qt::CheckState)(*_new_param)[Parameters::_REP_TRC].toInt());
}

void Preset::on_closeWindow_clicked()
{
    this->close();
}

void Preset::on_expandWindow_clicked()
{
    if (!this->isMaximized()) {
        this->showMaximized();
        _expandWindowsState = true;
    }
    else {
        this->showNormal();
        _expandWindowsState = false;
    }
}

void Preset::on_buttonCancel_clicked()  /*** Close preset window ***/
{
    this->close();
}

void Preset::on_buttonApply_clicked()  /*** Apply preset ***/
{
    (*_new_param)[Parameters::_OUTPUT_PARAM] = ui->textBrowser_presetname->toPlainText();
    (*_new_param)[Parameters::_CODEC] = QString::number(ui->comboBox_codec->currentIndex());
    (*_new_param)[Parameters::_MODE] = QString::number(ui->comboBox_mode->currentIndex());
    (*_new_param)[Parameters::_CONTAINER] = QString::number(ui->comboBox_container->currentIndex());
    (*_new_param)[Parameters::_BQR] = ui->lineEdit_bitrate->text();
    (*_new_param)[Parameters::_MINRATE] = ui->lineEdit_minrate->text();
    (*_new_param)[Parameters::_MAXRATE] = ui->lineEdit_maxrate->text();
    (*_new_param)[Parameters::_BUFSIZE] = ui->lineEdit_bufsize->text();
    (*_new_param)[Parameters::_LEVEL] = QString::number(ui->comboBox_level->currentIndex());
    (*_new_param)[Parameters::_FRAME_RATE] = QString::number(ui->comboBoxFrameRate->currentIndex());
    (*_new_param)[Parameters::_BLENDING] = QString::number(ui->comboBoxBlending->currentIndex());
    (*_new_param)[Parameters::_WIDTH] = QString::number(ui->comboBox_width->currentIndex());
    (*_new_param)[Parameters::_HEIGHT] = QString::number(ui->comboBox_height->currentIndex());
    (*_new_param)[Parameters::_PASS]= QString::number(ui->comboBox_pass->currentIndex());
    (*_new_param)[Parameters::_PRESET] = QString::number(ui->comboBox_preset->currentIndex());
    (*_new_param)[Parameters::_COLOR_RANGE] = QString::number(ui->comboBox_color_range->currentIndex());
    (*_new_param)[Parameters::_PRIMARY] = QString::number(ui->comboBox_color_prim->currentIndex());
    (*_new_param)[Parameters::_MATRIX] = QString::number(ui->comboBox_color_matrix->currentIndex());
    (*_new_param)[Parameters::_TRC] = QString::number(ui->comboBox_transfer->currentIndex());
    (*_new_param)[Parameters::_MIN_LUM] = ui->lineEdit_min_lum->text();
    (*_new_param)[Parameters::_MAX_LUM] = ui->lineEdit_max_lum->text();
    (*_new_param)[Parameters::_MAX_CLL] = ui->lineEdit_max_cll->text();
    (*_new_param)[Parameters::_MAX_FALL] = ui->lineEdit_max_fall->text();
    (*_new_param)[Parameters::_MASTER_DISPLAY] = QString::number(ui->comboBox_master_disp->currentIndex());
    (*_new_param)[Parameters::_CHROMA_COORD] = ui->lineEdit_chroma_coord->text();
    (*_new_param)[Parameters::_WHITE_COORD] = ui->lineEdit_white_coord->text();
    (*_new_param)[Parameters::_AUDIO_CODEC] = QString::number(ui->comboBox_audio_codec->currentIndex());
    (*_new_param)[Parameters::_AUDIO_BITRATE] = QString::number(ui->comboBox_audio_bitrate->currentIndex());
    (*_new_param)[Parameters::_ASAMPLE_RATE] = QString::number(ui->comboBox_audio_sampling->currentIndex());
    (*_new_param)[Parameters::_ACHANNELS] = QString::number(ui->comboBox_audio_channels->currentIndex());
    (*_new_param)[Parameters::_REP_PRIM] = QString::number(ui->checkBox_primaries->checkState());
    (*_new_param)[Parameters::_REP_MATRIX] = QString::number(ui->checkBox_matrix->checkState());
    (*_new_param)[Parameters::_REP_TRC] = QString::number(ui->checkBox_transfer->checkState());

    this->accept();
}

void Preset::on_buttonTab_1_clicked()
{
    ui->buttonTab_1->setEnabled(false);
    ui->buttonTab_2->setEnabled(true);
    ui->buttonTab_3->setEnabled(true);
    ui->buttonTab_4->setEnabled(true);
    ui->tabWidgetSettings->setCurrentIndex(0);
}

void Preset::on_buttonTab_2_clicked()
{
    ui->buttonTab_1->setEnabled(true);
    ui->buttonTab_2->setEnabled(false);
    ui->buttonTab_3->setEnabled(true);
    ui->buttonTab_4->setEnabled(true);
    ui->tabWidgetSettings->setCurrentIndex(1);
}

void Preset::on_buttonTab_3_clicked()
{
    ui->buttonTab_1->setEnabled(true);
    ui->buttonTab_2->setEnabled(true);
    ui->buttonTab_3->setEnabled(false);
    ui->buttonTab_4->setEnabled(true);
    ui->tabWidgetSettings->setCurrentIndex(2);
}

void Preset::on_buttonTab_4_clicked()
{
    ui->buttonTab_1->setEnabled(true);
    ui->buttonTab_2->setEnabled(true);
    ui->buttonTab_3->setEnabled(true);
    ui->buttonTab_4->setEnabled(false);
    ui->tabWidgetSettings->setCurrentIndex(3);
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

/************************************************
** Transform
************************************************/

void Preset::change_preset_name()  /*** Call Change preset name ***/
{
    std::cout << "Call change preset name..." << std::endl;
    int c1 = ui->comboBox_codec->currentIndex();
    int c2 = ui->comboBox_mode->currentIndex();
    int c11 = ui->comboBox_pass->currentIndex();
    int c12 = ui->comboBox_preset->currentIndex();
    int c21 = ui->comboBox_audio_codec->currentIndex();
    int c22 = ui->comboBox_audio_bitrate->currentIndex();
    QString container = ui->comboBox_container->currentText();
    QString codec, fps, res, mode, clrspace, preset, pass, hdr, acodec, abitrate;

    if ((c1 == -1) || (c2 == -1)) {
        return;
    }
    if ((c11 == -1) || (c12 == -1)) {
        return;
    }
    if ((c21 == -1) || (c22 == -1)) {
        return;
    }

    /************************************* Codec module ***************************************/

    QString arr_codec[NUMBER_PRESETS][3] = {
        {"HEVC, ",                tr("YUV, 4:2:0, 12 bit, "), "HDR: "},
        {"HEVC, ",                tr("YUV, 4:2:0, 10 bit, "), "HDR: "},
        {"HEVC, ",                tr("YUV, 4:2:0, 8 bit, "),  ""},
        {"AVC, ",                 tr("YUV, 4:2:0, 8 bit, "),  ""},
        {"VP9, ",                 tr("YUV, 4:2:0, 10 bit, "), "HDR: "},
        {"VP9, ",                 tr("YUV, 4:2:0, 8  bit, "), ""},
        {"Intel QSV, HEVC, ",     tr("YUV, 4:2:0, 10 bit, "),  "HDR: "},
        {"Intel QSV, HEVC, ",     tr("YUV, 4:2:0, 8 bit, "),  ""},
        {"Intel QSV, AVC, ",      tr("YUV, 4:2:0, 8 bit, "),  ""},
        {"Intel QSV, VP9, ",      tr("YUV, 4:2:0, 10 bit, "), "HDR: "},
        {"Intel QSV, VP9, ",      tr("YUV, 4:2:0, 8  bit, "), ""},
        {"Intel QSV, MPEG-2, ",   tr("YUV, 4:2:0, 8 bit, "),  ""},
        {"NVENC, HEVC, ",         tr("YUV, 4:2:0, 10 bit, "), "HDR: "},
        {"NVENC, HEVC, ",         tr("YUV, 4:2:0, 8 bit, "),  ""},
        {"NVENC, AVC, ",          tr("YUV, 4:2:0, 8 bit, "),  ""},
        {tr("ProRes Proxy, "),    tr("YUV, 4:2:2, 10 bit, "), "HDR: "},
        {"ProRes LT, ",           tr("YUV, 4:2:2, 10 bit, "), "HDR: "},
        {tr("ProRes Standard, "), tr("YUV, 4:2:2, 10 bit, "), "HDR: "},
        {"ProRes HQ, ",           tr("YUV, 4:2:2, 10 bit, "), "HDR: "},
        {"ProRes 4444, ",         tr("YUV, 4:4:4, 10 bit, "), "HDR: "},
        {"ProRes 4444XQ, ",       tr("YUV, 4:4:4, 10 bit, "), "HDR: "},
        {"DNxHR LB, ",            tr("YUV, 4:2:2, 8 bit, "),  ""},
        {"DNxHR SQ, ",            tr("YUV, 4:2:2, 8 bit, "),  ""},
        {"DNxHR HQ, ",            tr("YUV, 4:2:2, 8 bit, "),  ""},
        {"DNxHR HQX, ",           tr("YUV, 4:2:2, 10 bit, "), "HDR: "},
        {"DNxHR 444, ",           tr("YUV, 4:4:4, 10 bit, "), "HDR: "},
        {"XDCAM HD422, ",         tr("YUV, 4:2:2, 8 bit, "),  ""},
        {"XAVC 4:2:2, ",          tr("YUV, 4:2:2, 8 bit, "),  ""},
        {tr("From source, "),     "",                         "HDR: "}
    };
    codec = arr_codec[c1][0];
    clrspace = arr_codec[c1][1];

    /************************************* Resize module ***************************************/

    QString w = ui->comboBox_width->currentText();
    QString h = ui->comboBox_height->currentText();
    if (w != tr("Source") && h != tr("Source")) {
        res = w + "x" + h + ", ";
    } else if (w == tr("Source") && h == tr("Source")) {
        res = QString("Res: %1, ").arg(tr("Source"));
    } else {
        res = QString("Res: %1, ").arg(tr("Custom"));
    }

    /************************************* FPS module ***************************************/

    QString f = ui->comboBoxFrameRate->currentText();
    if (f == tr("Source")) {
        fps = QString("Fps: %1, ").arg(tr("Source"));
    } else {
        fps = f + " fps, ";
    }

    /************************************* Mode module ***************************************/

    QString arr_mode[NUMBER_PRESETS][5] = {
        {"CBR", "ABR", "VBR", "CRF", "CQP"},
        {"CBR", "ABR", "VBR", "CRF", "CQP"},
        {"CBR", "ABR", "VBR", "CRF", "CQP"},
        {"CBR", "ABR", "VBR", "CRF", "CQP"},
        {"ABR", "CRF", "",    "",    ""},
        {"ABR", "CRF", "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"ABR", "CRF", "",    "",    ""},
        {"ABR", "CRF", "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"",    "",    "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"CBR", "",    "",    "",    ""},
        {"",    "",    "",    "",    ""}
    };
    QString selected_mode = arr_mode[c1][c2];
    if (selected_mode != "" && (selected_mode == "CRF" || selected_mode == "CQP")) {
        mode = selected_mode + " " + ui->lineEdit_bitrate->text() + ", ";
    }
    else if (selected_mode != "" && (selected_mode != "CRF" && selected_mode != "CQP")) {
        mode = selected_mode + " " + ui->lineEdit_bitrate->text() + tr(" MBps, ");
    }

    /************************************* Preset module ***************************************/

    QString arr_preset[NUMBER_PRESETS][10] = {
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Veryfast"),  tr("Faster"),    tr("Fast"),     tr("Medium"), tr("Slow"), tr("Slower"), tr("Veryslow"), "",           ""},
        {tr("None"), tr("Veryfast"),  tr("Faster"),    tr("Fast"),     tr("Medium"), tr("Slow"), tr("Slower"), tr("Veryslow"), "",           ""},
        {tr("None"), tr("Veryfast"),  tr("Faster"),    tr("Fast"),     tr("Medium"), tr("Slow"), tr("Slower"), tr("Veryslow"), "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Veryfast"),  tr("Faster"),    tr("Fast"),     tr("Medium"), tr("Slow"), tr("Slower"), tr("Veryslow"), "",           ""},
        {tr("None"), tr("Slow"),      "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Slow"),      "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Slow"),      "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {"",         "",              "",              "",             "",           "",         "",           "",             "",           ""}
    };
    QString p = arr_preset[c1][c12];
    if (p != "" && p != tr("None")) {
        preset = tr("Preset: ") + p + ", ";
    }

    /************************************* Pass module ***************************************/

    QString arr_pass[NUMBER_PRESETS][2] = {
        {tr("1 Pass"), tr("2 Pass")},
        {tr("1 Pass"), tr("2 Pass")},
        {tr("1 Pass"), tr("2 Pass")},
        {tr("1 Pass"), tr("2 Pass")},
        {tr("1 Pass"), tr("2 Pass")},
        {tr("1 Pass"), tr("2 Pass")},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {tr("2 Pass"), ""},
        {tr("2 Pass"), ""},
        {tr("2 Pass"), ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""},
        {"",       ""}
    };
    if (arr_pass[c1][c11] != "") {
        pass = arr_pass[c1][c11] + ", ";
    }
    if (arr_codec[c1][2] != "") {
        hdr = arr_codec[c1][2] + tr("Enabled, ");
    }

    /************************************* Audio module ***************************************/

    QString arr_acodec[NUMBER_PRESETS][6] = {
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"Opus",       "Vorbis",     tr("Source"), "",           "",     ""},
        {"Opus",       "Vorbis",     tr("Source"), "",           "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"Opus",       "Vorbis",     tr("Source"), "",           "",     ""},
        {"Opus",       "Vorbis",     tr("Source"), "",           "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"AAC",        "AC3",        "DTS",        tr("Source"), "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"PCM 16 bit", "",           "",           "",           "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",           "",     ""},
        {"AAC",        "AC3",        "DTS",        "Vorbis",     "Opus", tr("Source")}
    };
    if (arr_acodec[c1][c21] != "") {
        acodec = tr("Audio: ") + arr_acodec[c1][c21] + ", ";
    }
    QString arr_bitrate[5][17] = {
        {"384k",  "320k",  "256k",  "192k",  "128k",  "96k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AAC
        {"640k",  "448k",  "384k",  "256k",  "",      "",      "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AC3
        {"3840k", "3072k", "2048k", "1920k", "1536k", "1472k", "1344k", "1280k", "1152k", "1024k", "960k", "768k", "640k", "576k", "512k", "448k", "384k"}, // DTS
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // Vorbis
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""} // Opus
    };
    if (arr_acodec[c1][c21] == "AAC") {
        abitrate = arr_bitrate[0][c22] + ", ";
    }
    if (arr_acodec[c1][c21] == "AC3") {
        abitrate = arr_bitrate[1][c22] + ", ";
    }
    if (arr_acodec[c1][c21] == "DTS") {
        abitrate = arr_bitrate[2][c22] + ", ";
    }
    if (arr_acodec[c1][c21] == "Vorbis") {
        abitrate = arr_bitrate[3][c22] + ", ";
    }
    if (arr_acodec[c1][c21] == "Opus") {
        abitrate = arr_bitrate[4][c22] + ", ";
    }

    /************************************* Result module ***************************************/

    QString cur_preset_name = codec + res + fps + clrspace + mode + preset + pass + hdr
            + acodec + abitrate + container;
    ui->textBrowser_presetname->setText(cur_preset_name);
}

void Preset::on_comboBoxAspectRatio_currentIndexChanged(int index)
{
    QString aspect_ratio[34][2] = {
        {tr("Source"), tr("Source")}, // Source
        {"640",    "480"},    // 640x480
        {"720",    "480"},    // 720x480
        {"720",    "576"},    // 720x576
        {"720",    "486"},    // 720x486
        {"720",    "540"},    // 720x540
        {"768",    "576"},    // 768x576
        {"1280",   "720"},    // 1280x720
        {"1920",   "1080"},   // 1920x1080
        {"1440",   "1080"},   // 1440x1080
        {"1828",   "988"},    // 1828x988
        {"1828",   "1028"},   // 1828x1028
        {"1828",   "1102"},   // 1828x1102
        {"1828",   "1332"},   // 1828x1332
        {"1828",   "1556"},   // 1828x1556
        {"2048",   "872"},    // 2048x872
        {"2048",   "1107"},   // 2048x1107
        {"2048",   "1152"},   // 2048x1152
        {"2048",   "1234"},   // 2048x1234
        {"2048",   "1536"},   // 2048x1536
        {"3656",   "1976"},   // 3656x1976
        {"3656",   "2056"},   // 3656x2056
        {"3656",   "2204"},   // 3656x2204
        {"3656",   "2664"},   // 3656x2664
        {"3656",   "3112"},   // 3656x3112
        {"4096",   "1744"},   // 4096x1744
        {"4096",   "2214"},   // 4096x2214
        {"4096",   "2304"},   // 4096x2304
        {"4096",   "2468"},   // 4096x2468
        {"4096",   "3072"},   // 4096x3072
        {"3840",   "2160"},   // 3840x2160
        {"4520",   "2540"},   // 4520x2540
        {"7680",   "4320"},   // 7680x4320
        {tr("Custom"), tr("Custom")}  // Custom
    };

    QString width = aspect_ratio[index][0];
    QString height = aspect_ratio[index][1];

    int widthIndex = ui->comboBox_width->findText(width, Qt::MatchCaseSensitive);
    int heightIndex = ui->comboBox_height->findText(height, Qt::MatchCaseSensitive);

    if (widthIndex >= 0) {
        ui->comboBox_width->setCurrentIndex(widthIndex);
    }
    if (heightIndex >= 0) {
        ui->comboBox_height->setCurrentIndex(heightIndex);
    }

    float width_= width.toFloat();
    float height_ = height.toFloat();

    if (height_ != 0.0f ) {
        _aspectRatio = round(10000 * width_ / height_)/10000;
    } else {
        _aspectRatio = 0.0f;
    }
    //std::cout << ">>>>>>>>>>>>>> ASPECT RATIO : " << _aspectRatio << std::endl;
    _repeat++;
}

void Preset::on_comboBox_width_currentTextChanged(const QString &arg1)
{
    lockSignals(true);

    QString aspect_ratio[34][2] = {
        {tr("Source"), tr("Source")}, // Source
        {"640",    "480"},    // 640x480
        {"720",    "480"},    // 720x480
        {"720",    "576"},    // 720x576
        {"720",    "486"},    // 720x486
        {"720",    "540"},    // 720x540
        {"768",    "576"},    // 768x576
        {"1280",   "720"},    // 1280x720
        {"1920",   "1080"},   // 1920x1080
        {"1440",   "1080"},   // 1440x1080
        {"1828",   "988"},    // 1828x988
        {"1828",   "1028"},   // 1828x1028
        {"1828",   "1102"},   // 1828x1102
        {"1828",   "1332"},   // 1828x1332
        {"1828",   "1556"},   // 1828x1556
        {"2048",   "872"},    // 2048x872
        {"2048",   "1107"},   // 2048x1107
        {"2048",   "1152"},   // 2048x1152
        {"2048",   "1234"},   // 2048x1234
        {"2048",   "1536"},   // 2048x1536
        {"3656",   "1976"},   // 3656x1976
        {"3656",   "2056"},   // 3656x2056
        {"3656",   "2204"},   // 3656x2204
        {"3656",   "2664"},   // 3656x2664
        {"3656",   "3112"},   // 3656x3112
        {"4096",   "1744"},   // 4096x1744
        {"4096",   "2214"},   // 4096x2214
        {"4096",   "2304"},   // 4096x2304
        {"4096",   "2468"},   // 4096x2468
        {"4096",   "3072"},   // 4096x3072
        {"3840",   "2160"},   // 3840x2160
        {"4520",   "2540"},   // 4520x2540
        {"7680",   "4320"},   // 7680x4320
        {tr("Custom"), tr("Custom")}  // Custom
    };

    QString height = ui->comboBox_height->currentText();

    for (int i = 0; i < 34; i++) {
        if (aspect_ratio[i][0] == arg1 && aspect_ratio[i][1] == height) {
            ui->comboBoxAspectRatio->setCurrentIndex(i);
            break;
        } else {
            ui->comboBoxAspectRatio->setCurrentIndex(33);
        }
    }
    lockSignals(false);
    calculateDAR(arg1, height);
    _repeat++;
}

void Preset::on_comboBox_height_currentTextChanged(const QString &arg1)
{
    lockSignals(true);

    QString aspect_ratio[34][2] = {
        {tr("Source"), tr("Source")}, // Source
        {"640",    "480"},    // 640x480
        {"720",    "480"},    // 720x480
        {"720",    "576"},    // 720x576
        {"720",    "486"},    // 720x486
        {"720",    "540"},    // 720x540
        {"768",    "576"},    // 768x576
        {"1280",   "720"},    // 1280x720
        {"1920",   "1080"},   // 1920x1080
        {"1440",   "1080"},   // 1440x1080
        {"1828",   "988"},    // 1828x988
        {"1828",   "1028"},   // 1828x1028
        {"1828",   "1102"},   // 1828x1102
        {"1828",   "1332"},   // 1828x1332
        {"1828",   "1556"},   // 1828x1556
        {"2048",   "872"},    // 2048x872
        {"2048",   "1107"},   // 2048x1107
        {"2048",   "1152"},   // 2048x1152
        {"2048",   "1234"},   // 2048x1234
        {"2048",   "1536"},   // 2048x1536
        {"3656",   "1976"},   // 3656x1976
        {"3656",   "2056"},   // 3656x2056
        {"3656",   "2204"},   // 3656x2204
        {"3656",   "2664"},   // 3656x2664
        {"3656",   "3112"},   // 3656x3112
        {"4096",   "1744"},   // 4096x1744
        {"4096",   "2214"},   // 4096x2214
        {"4096",   "2304"},   // 4096x2304
        {"4096",   "2468"},   // 4096x2468
        {"4096",   "3072"},   // 4096x3072
        {"3840",   "2160"},   // 3840x2160
        {"4520",   "2540"},   // 4520x2540
        {"7680",   "4320"},   // 7680x4320
        {tr("Custom"), tr("Custom")}  // Custom
    };

    QString width = ui->comboBox_width->currentText();

    for (int i = 0; i < 34; i++) {
        if (aspect_ratio[i][0] == width && aspect_ratio[i][1] == arg1) {
            ui->comboBoxAspectRatio->setCurrentIndex(i);
            break;
        } else {
            ui->comboBoxAspectRatio->setCurrentIndex(33);
        }
    }
    lockSignals(false);
    calculateDAR(width, arg1);
    _repeat++;
}

void Preset::on_comboBoxFrameRate_currentIndexChanged(int index)
{
    if (index == 0) {
        ui->comboBoxBlending->setCurrentIndex(0);
        ui->comboBoxBlending->setEnabled(false);
    } else {
        ui->comboBoxBlending->setEnabled(true);
    }
    _repeat++;
}

void Preset::calculateDAR(QString width, QString height)
{
    if (width == tr("Source") && height == tr("Source")) {
        ui->lineEdit_DAR->setText(tr("Source"));
    }
    else if (width == tr("Source") && height != tr("Source")) {
        ui->lineEdit_DAR->setText(tr("Undef"));
    }
    else if (width != tr("Source") && height == tr("Source")) {
        ui->lineEdit_DAR->setText(tr("Undef"));
    }
    else if (width != tr("Source") && height != tr("Source")) {
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

void Preset::repeat_handler()  /*** Repeat handler ***/
{
    std::cout << "Call by timer... Repeat count: " << _repeat << std::endl;
    if (_repeat > 0) {
        _repeat = 0;
        change_preset_name();
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

void Preset::on_comboBox_codec_currentTextChanged(const QString &arg1)  /*** Change current codec ***/
{
    lockSignals(true);
    std::cout << "Change current codec..." << std::endl;
    const QStringList levelsH264 = {tr("Auto"), "1", "1b",  "1.1", "1.2", "1.3", "2",   "2.1", "2.2", "3", "3.1",
                                    "3.2",      "4", "4.1", "4.2", "5",   "5.1", "5.2", "6",   "6.1", "6.2"};
    const QStringList levelsH265 = {tr("Auto"), "1",   "2",   "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1",
                                    "5.2", "6", "6.1", "6.2"};
    const QStringList presetsH264 = {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"),
                                     tr("Fast"), tr("Medium"),    tr("Slow"),      tr("Slower"),   tr("Veryslow")};
    const QStringList presetsH265 = {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"),
                                     tr("Fast"), tr("Medium"),    tr("Slow"),      tr("Slower"),   tr("Veryslow")};
    const QStringList presetsH264QSV = {tr("None"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"),
                                        tr("Slow"), tr("Slower"),   tr("Veryslow")};
    const QStringList presetsH265QSV = {tr("None"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"),
                                        tr("Slow"), tr("Slower"),   tr("Veryslow")};
    const QStringList presetsMPEG2QSV = {tr("None"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"),
                                         tr("Slow"), tr("Slower"),   tr("Veryslow")};
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

    if (arg1 == tr("H.265/HEVC 4:2:0 12 bit")) {
        ui->comboBox_container->addItems({"MKV", "MOV", "MP4"});
        ui->comboBox_container->setCurrentIndex(2);
        ui->comboBox_mode->addItems({tr("Constant Bitrate"), tr("Average Bitrate"), tr("Variable Bitrate"),
                                            tr("Constant Rate Factor"), tr("Constant QP")});
        ui->comboBox_pass->addItems({tr("1 Pass"), tr("2 Pass")});
        ui->comboBox_profile->setCurrentIndex(Profile::MAIN12);
        ui->comboBox_preset->addItems(presetsH265);
        ui->comboBox_preset->setCurrentIndex(6);
        ui->comboBox_level->addItems(levelsH265);
        ui->comboBox_level->setCurrentIndex(0);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P12LE);
        ui->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", tr("Source")});
    }

    else if (arg1 == tr("H.265/HEVC 4:2:0 10 bit")) {
        ui->comboBox_container->addItems({"MKV", "MOV", "MP4"});
        ui->comboBox_container->setCurrentIndex(2);
        ui->comboBox_mode->addItems({tr("Constant Bitrate"), tr("Average Bitrate"), tr("Variable Bitrate"),
                                            tr("Constant Rate Factor"), tr("Constant QP")});
        ui->comboBox_pass->addItems({tr("1 Pass"), tr("2 Pass")});
        ui->comboBox_profile->setCurrentIndex(Profile::MAIN10);
        ui->comboBox_preset->addItems(presetsH265);
        ui->comboBox_preset->setCurrentIndex(6);
        ui->comboBox_level->addItems(levelsH265);
        ui->comboBox_level->setCurrentIndex(0);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P10LE);
        ui->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", tr("Source")});
    }

    else if (arg1 == tr("H.265/HEVC 4:2:0 8 bit")) {
        ui->comboBox_container->addItems({"MKV", "MOV", "MP4", "M2TS", "TS"});
        ui->comboBox_container->setCurrentIndex(2);
        ui->comboBox_mode->addItems({tr("Constant Bitrate"), tr("Average Bitrate"), tr("Variable Bitrate"),
                                            tr("Constant Rate Factor"), tr("Constant QP")});
        ui->comboBox_pass->addItems({tr("1 Pass"), tr("2 Pass")});
        ui->comboBox_profile->setCurrentIndex(Profile::MAIN);
        ui->comboBox_preset->addItems(presetsH265);
        ui->comboBox_preset->setCurrentIndex(6);
        ui->comboBox_level->addItems(levelsH265);
        ui->comboBox_level->setCurrentIndex(0);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P);
        ui->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", tr("Source")});
        disableHDR();
    }

    else if (arg1 == tr("H.264/AVC 4:2:0 8 bit")) {
        ui->comboBox_container->addItems({"MKV", "MOV", "MP4", "M2TS", "TS"});
        ui->comboBox_container->setCurrentIndex(2);
        ui->comboBox_mode->addItems({tr("Constant Bitrate"), tr("Average Bitrate"), tr("Variable Bitrate"),
                                            tr("Constant Rate Factor"), tr("Constant QP")});
        ui->comboBox_pass->addItems({tr("1 Pass"), tr("2 Pass")});
        ui->comboBox_profile->setCurrentIndex(Profile::HIGH);
        ui->comboBox_preset->addItems(presetsH264);
        ui->comboBox_preset->setCurrentIndex(6);
        ui->comboBox_level->addItems(levelsH264);
        ui->comboBox_level->setCurrentIndex(0);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P);
        ui->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", tr("Source")});
        disableHDR();
    }

    else if (arg1 == tr("VP9 4:2:0 10 bit")) {
        ui->comboBox_container->addItems({"WebM", "MKV"});
        ui->comboBox_mode->addItems({tr("Average Bitrate"), tr("Constant Rate Factor")});
        ui->comboBox_pass->addItems({tr("1 Pass"), tr("2 Pass")});
        ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_2);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P10LE);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"Opus", "Vorbis", tr("Source")});
    }

    else if (arg1 == tr("VP9 4:2:0 8 bit")) {
        ui->comboBox_container->addItems({"WebM", "MKV"});
        ui->comboBox_mode->addItems({tr("Average Bitrate"), tr("Constant Rate Factor")});
        ui->comboBox_pass->addItems({tr("1 Pass"), tr("2 Pass")});
        ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_1);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"Opus", "Vorbis", tr("Source")});
        disableHDR();
    }

    else if (arg1 == tr("Intel QSV H.265/HEVC 4:2:0 10 bit")) {
        ui->comboBoxAspectRatio->setCurrentIndex(0);
        ui->comboBoxAspectRatio->setEnabled(false);
        ui->comboBox_width->setEnabled(false);
        ui->comboBox_height->setEnabled(false);
        ui->comboBoxFrameRate->setEnabled(false);
        ui->comboBox_container->addItems({"MKV", "MOV", "MP4"});
        ui->comboBox_container->setCurrentIndex(2);
        ui->comboBox_mode->addItems({tr("Variable Bitrate")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::MAIN10);
        ui->comboBox_preset->addItems(presetsH265QSV);
        ui->comboBox_preset->setCurrentIndex(4);
        ui->comboBox_level->addItems(levelsH265);
        ui->comboBox_level->setCurrentIndex(0);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", tr("Source")});
    }

    else if (arg1 == tr("Intel QSV H.265/HEVC 4:2:0 8 bit")) {
        ui->comboBoxAspectRatio->setCurrentIndex(0);
        ui->comboBoxAspectRatio->setEnabled(false);
        ui->comboBox_width->setEnabled(false);
        ui->comboBox_height->setEnabled(false);
        ui->comboBoxFrameRate->setEnabled(false);
        ui->comboBox_container->addItems({"MKV", "MOV", "MP4"});
        ui->comboBox_container->setCurrentIndex(2);
        ui->comboBox_mode->addItems({tr("Variable Bitrate")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::MAIN);
        ui->comboBox_preset->addItems(presetsH265QSV);
        ui->comboBox_preset->setCurrentIndex(4);
        ui->comboBox_level->addItems(levelsH265);
        ui->comboBox_level->setCurrentIndex(0);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", tr("Source")});
        disableHDR();
    }

    else if (arg1 == tr("Intel QSV H.264/AVC 4:2:0 8 bit")) {
        ui->comboBoxAspectRatio->setCurrentIndex(0);
        ui->comboBoxAspectRatio->setEnabled(false);
        ui->comboBox_width->setEnabled(false);
        ui->comboBox_height->setEnabled(false);
        ui->comboBoxFrameRate->setEnabled(false);
        ui->comboBox_container->addItems({"MKV", "MOV", "MP4"});
        ui->comboBox_container->setCurrentIndex(2);
        ui->comboBox_mode->addItems({tr("Variable Bitrate")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::HIGH);
        ui->comboBox_preset->addItems(presetsH264QSV);
        ui->comboBox_preset->setCurrentIndex(4);
        ui->comboBox_level->addItems(levelsH264);
        ui->comboBox_level->setCurrentIndex(0);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", tr("Source")});
        disableHDR();
    }

    else if (arg1 == tr("Intel QSV VP9 4:2:0 10 bit")) {
        ui->comboBoxAspectRatio->setCurrentIndex(0);
        ui->comboBoxAspectRatio->setEnabled(false);
        ui->comboBox_width->setEnabled(false);
        ui->comboBox_height->setEnabled(false);
        ui->comboBoxFrameRate->setEnabled(false);
        ui->comboBox_container->addItems({"WebM", "MKV"});
        ui->comboBox_container->setCurrentIndex(0);
        ui->comboBox_mode->addItems({tr("Average Bitrate"), tr("Constant Rate Factor")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_2);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"Opus", "Vorbis", tr("Source")});
    }

    else if (arg1 == tr("Intel QSV VP9 4:2:0 8 bit")) {
        ui->comboBoxAspectRatio->setCurrentIndex(0);
        ui->comboBoxAspectRatio->setEnabled(false);
        ui->comboBox_width->setEnabled(false);
        ui->comboBox_height->setEnabled(false);
        ui->comboBoxFrameRate->setEnabled(false);
        ui->comboBox_container->addItems({"WebM", "MKV"});
        ui->comboBox_container->setCurrentIndex(0);
        ui->comboBox_mode->addItems({tr("Average Bitrate"), tr("Constant Rate Factor")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_1);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"Opus", "Vorbis", tr("Source")});
        disableHDR();
    }

    else if (arg1 == tr("Intel QSV MPEG-2 4:2:0 8 bit")) {
        ui->comboBoxAspectRatio->setCurrentIndex(0);
        ui->comboBoxAspectRatio->setEnabled(false);
        ui->comboBox_width->setEnabled(false);
        ui->comboBox_height->setEnabled(false);
        ui->comboBoxFrameRate->setEnabled(false);
        ui->comboBox_container->addItems({"MKV", "MPG", "AVI", "M2TS", "TS"});
        ui->comboBox_container->setCurrentIndex(3);
        ui->comboBox_mode->addItems({tr("Variable Bitrate")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::HIGH);
        ui->comboBox_preset->addItems(presetsMPEG2QSV);
        ui->comboBox_preset->setCurrentIndex(4);
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::PIXFORMAT_AUTO);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", tr("Source")});
        disableHDR();
    }

    else if (arg1 == tr("NVENC H.265/HEVC 4:2:0 10 bit")) {
        ui->comboBox_container->addItems({"MKV", "MOV", "MP4"});
        ui->comboBox_container->setCurrentIndex(2);
        ui->comboBox_mode->addItems({tr("Variable Bitrate")});
        ui->comboBox_pass->addItems({tr("2 Pass Optimisation")});
        ui->comboBox_profile->setCurrentIndex(Profile::MAIN10);
        ui->comboBox_preset->addItems({tr("None"), tr("Slow")});
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::P010LE);
        ui->comboBox_preset->setCurrentIndex(1);
        ui->comboBox_level->addItems(levelsH265);
        ui->comboBox_level->setCurrentIndex(0);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", tr("Source")});
    }

    else if (arg1 == tr("NVENC H.265/HEVC 4:2:0 8 bit")) {
        ui->comboBox_container->addItems({"MKV", "MOV", "MP4", "M2TS", "TS"});
        ui->comboBox_container->setCurrentIndex(2);
        ui->comboBox_mode->addItems({tr("Variable Bitrate")});
        ui->comboBox_pass->addItems({tr("2 Pass Optimisation")});
        ui->comboBox_profile->setCurrentIndex(Profile::MAIN);
        ui->comboBox_preset->addItems({tr("None"), tr("Slow")});
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P);
        ui->comboBox_preset->setCurrentIndex(1);
        ui->comboBox_level->addItems(levelsH265);
        ui->comboBox_level->setCurrentIndex(0);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", tr("Source")});
        disableHDR();
    }

    else if (arg1 == tr("NVENC H.264/AVC 4:2:0 8 bit")) {
        ui->comboBox_container->addItems({"MKV", "MOV", "MP4", "M2TS", "TS"});
        ui->comboBox_container->setCurrentIndex(2);
        ui->comboBox_mode->addItems({tr("Variable Bitrate")});
        ui->comboBox_pass->addItems({tr("2 Pass Optimisation")});
        ui->comboBox_profile->setCurrentIndex(Profile::HIGH);
        ui->comboBox_preset->addItems({tr("None"), tr("Slow")});
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV420P);
        ui->comboBox_preset->setCurrentIndex(1);
        ui->comboBox_level->addItems(levelsH264);
        ui->comboBox_level->setCurrentIndex(0);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", tr("Source")});
        disableHDR();
    }

    else if (arg1 == tr("ProRes Proxy")) {
        ui->comboBox_container->addItems({"MOV"});
        ui->comboBox_mode->addItems({tr("Auto")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_0);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422P10LE);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_container->setEnabled(false);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
    }

    else if (arg1 == "ProRes LT") {
        ui->comboBox_container->addItems({"MOV"});
        ui->comboBox_mode->addItems({tr("Auto")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_1);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422P10LE);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_container->setEnabled(false);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
    }

    else if (arg1 == tr("ProRes Standard")) {
        ui->comboBox_container->addItems({"MOV"});
        ui->comboBox_mode->addItems({tr("Auto")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_2);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422P10LE);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_container->setEnabled(false);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
    }

    else if (arg1 == "ProRes HQ") {
        ui->comboBox_container->addItems({"MOV"});
        ui->comboBox_mode->addItems({tr("Auto")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_3);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422P10LE);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_container->setEnabled(false);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
    }

    else if (arg1 == "ProRes 4444") {
        ui->comboBox_container->addItems({"MOV"});
        ui->comboBox_mode->addItems({tr("Auto")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_4);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV444P10LE);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_container->setEnabled(false);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
    }

    else if (arg1 == "ProRes 4444 XQ") {
        ui->comboBox_container->addItems({"MOV"});
        ui->comboBox_mode->addItems({tr("Auto")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_5);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV444P10LE);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_container->setEnabled(false);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
    }

    else if (arg1 == "DNxHR LB") {
        ui->comboBox_container->addItems({"MOV"});
        ui->comboBox_mode->addItems({tr("Auto")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::DNXHR_LB);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422p);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_container->setEnabled(false);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
        disableHDR();
    }

    else if (arg1 == "DNxHR SQ") {
        ui->comboBox_container->addItems({"MOV"});
        ui->comboBox_mode->addItems({tr("Auto")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::DNXHR_SQ);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422p);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_container->setEnabled(false);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
        disableHDR();
    }

    else if (arg1 == "DNxHR HQ") {
        ui->comboBox_container->addItems({"MOV"});
        ui->comboBox_mode->addItems({tr("Auto")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::DNXHR_HQ);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422p);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_container->setEnabled(false);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
        disableHDR();
    }

    else if (arg1 == "DNxHR HQX") {
        ui->comboBox_container->addItems({"MOV"});
        ui->comboBox_mode->addItems({tr("Auto")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::DNXHR_HQX);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422P10LE);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_container->setEnabled(false);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
    }

    else if (arg1 == "DNxHR 444") {
        ui->comboBox_container->addItems({"MOV"});
        ui->comboBox_mode->addItems({tr("Auto")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::DNXHR_444);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV444P10LE);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_container->setEnabled(false);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
    }

    else if (arg1 == "XDCAM HD422") {
        ui->comboBoxFrameRate->setCurrentIndex(8);
        ui->comboBox_container->addItems({"MXF"});
        ui->comboBox_mode->addItems({tr("Variable Bitrate")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_0);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({"2"});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422p);
        ui->comboBox_preset->setEnabled(false);
        ui->comboBox_container->setEnabled(false);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"PCM 16 bit"});
        disableHDR();
    }

    else if (arg1 == "XAVC 4:2:2") {
        ui->comboBoxFrameRate->setCurrentIndex(8);
        ui->comboBox_container->addItems({"MXF"});
        ui->comboBox_mode->addItems({tr("Constant Bitrate")});
        ui->lineEdit_bitrate->setText("480");
        ui->lineEdit_bufsize->setText("480");
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_AUTO);
        ui->comboBox_preset->addItems(presetsH264);
        ui->comboBox_level->addItems({"5.2"});
        ui->comboBox_level->setEnabled(false);
        ui->comboBox_pixfmt->setCurrentIndex(Pixformat::YUV422p);
        ui->comboBox_container->setEnabled(false);
        ui->comboBox_mode->setEnabled(false);
        ui->comboBox_pass->setEnabled(false);
        ui->comboBox_audio_codec->addItems({"PCM 16 bit", "PCM 24 bit", "PCM 32 bit"});
        disableHDR();
    }

    else if (arg1 == tr("Source")) {
        ui->comboBoxAspectRatio->setCurrentIndex(0);
        ui->comboBoxAspectRatio->setEnabled(false);
        ui->comboBox_width->setEnabled(false);
        ui->comboBox_height->setEnabled(false);
        ui->comboBoxFrameRate->setCurrentIndex(0);
        ui->comboBoxFrameRate->setEnabled(false);
        ui->comboBox_container->addItems({"MKV", "MOV", "MP4", "M2TS", "TS"});
        ui->comboBox_container->setCurrentIndex(2);
        ui->comboBox_mode->addItems({tr("Auto")});
        ui->comboBox_pass->addItems({tr("Auto")});
        ui->comboBox_profile->setCurrentIndex(Profile::PROFILE_AUTO);
        ui->comboBox_preset->addItems({tr("None")});
        ui->comboBox_level->addItems({tr("Auto")});
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
        ui->comboBox_audio_codec->addItems({"AAC", "AC3", "DTS", "Vorbis", "Opus", tr("Source")});
    }
    lockSignals(false);
}

void Preset::on_comboBox_mode_currentTextChanged(const QString &arg1)  /*** Change curret mode ***/
{
    lockSignals(true);
    std::cout << "Change current mode..." << std::endl;
    ui->lineEdit_bitrate->clear();
    ui->lineEdit_minrate->clear();
    ui->lineEdit_maxrate->clear();
    ui->lineEdit_bufsize->clear();
    ui->lineEdit_bitrate->setEnabled(true);
    ui->lineEdit_minrate->setEnabled(true);
    ui->lineEdit_maxrate->setEnabled(true);
    ui->lineEdit_bufsize->setEnabled(true);
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
    }
    lockSignals(false);
    _repeat++;
}

void Preset::on_comboBox_preset_currentIndexChanged(int index) /*** Signal Preset index changed ***/
{
    std::cout << "Index of Preset: " << index << std::endl;
    _repeat++;
}

void Preset::on_comboBox_pass_currentIndexChanged(int index)  /*** Signal Pass index changed ***/
{
    std::cout << "Index of Pass: " << index << std::endl;
    _repeat++;
}

void Preset::on_comboBox_container_currentTextChanged()  /*** Signal Container currentText changed ***/
{
    _repeat++;
}

void Preset::on_lineEdit_bitrate_editingFinished()
{
    _repeat++;
}

/************************************************
** Audio
************************************************/

void Preset::on_comboBox_audio_codec_currentTextChanged(const QString &arg1) /*** Change current audio codec ***/
{
    lockSignals(true);
    std::cout << "Change current audio codec..." << std::endl;
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
    _repeat++;
    lockSignals(false);
}

void Preset::on_comboBox_audio_bitrate_currentTextChanged()  /*** Signal Abitrate currentText changed ***/
{
    _repeat++;
}

/************************************************
** HDR Metadata
************************************************/

void Preset::on_comboBox_master_disp_currentTextChanged(const QString &arg1)  /*** Change current master display ***/
{
    lockSignals(true);
    std::cout << "Change current master display..." << std::endl;
    ui->lineEdit_chroma_coord->clear();
    ui->lineEdit_white_coord->clear();
    ui->lineEdit_chroma_coord->setEnabled(true);
    ui->lineEdit_white_coord->setEnabled(true);
    if (arg1 == tr("Unsprt")) {
        ui->lineEdit_chroma_coord->setText(tr("Unsprt"));
        ui->lineEdit_white_coord->setText(tr("Unsprt"));
        ui->lineEdit_chroma_coord->setEnabled(false);
        ui->lineEdit_white_coord->setEnabled(false);
    }
    else if (arg1 == tr("Source")) {
        ui->lineEdit_chroma_coord->setText(tr("Source"));
        ui->lineEdit_white_coord->setText(tr("Source"));
        ui->lineEdit_chroma_coord->setEnabled(false);
        ui->lineEdit_white_coord->setEnabled(false);
    }
    else if (arg1 == tr("Custom")) {
        ui->lineEdit_chroma_coord->setText("0.680,0.320,0.265,0.690,0.150,0.060");
        ui->lineEdit_white_coord->setText("0.3127,0.3290");
    }
    else if (arg1 == tr("Display P3")) {
        ui->lineEdit_chroma_coord->setText("0.680,0.320,0.265,0.690,0.150,0.060");
        ui->lineEdit_white_coord->setText("0.3127,0.3290");
        ui->lineEdit_chroma_coord->setEnabled(false);
        ui->lineEdit_white_coord->setEnabled(false);
    }
    else if (arg1 == tr("DCI P3")) {
        ui->lineEdit_chroma_coord->setText("0.680,0.320,0.265,0.690,0.150,0.060");
        ui->lineEdit_white_coord->setText("0.314,0.3510");
        ui->lineEdit_chroma_coord->setEnabled(false);
        ui->lineEdit_white_coord->setEnabled(false);
    }
    else if (arg1 == "BT.2020") {
        ui->lineEdit_chroma_coord->setText("0.708,0.292,0.170,0.797,0.131,0.046");
        ui->lineEdit_white_coord->setText("0.3127,0.3290");
        ui->lineEdit_chroma_coord->setEnabled(false);
        ui->lineEdit_white_coord->setEnabled(false);
    }
    else if (arg1 == "BT.709") {
        ui->lineEdit_chroma_coord->setText("0.640,0.330,0.30,0.60,0.150,0.060");
        ui->lineEdit_white_coord->setText("0.3127,0.3290");
        ui->lineEdit_chroma_coord->setEnabled(false);
        ui->lineEdit_white_coord->setEnabled(false);
    }
    lockSignals(false);
}
