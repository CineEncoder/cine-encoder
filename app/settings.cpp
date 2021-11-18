/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: settings.cpp
 MODIFIED: November, 2021
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "settings.h"
#include "ui_settings.h"



Settings::Settings(QWidget *parent):
    QDialog(parent),
    ui(new Ui::Settings)
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

Settings::~Settings()
{
    delete ui;
}

void Settings::closeEvent(QCloseEvent *event)
{
    event->ignore();
    *_ptr_settingsWindowGeometry = this->saveGeometry();
    event->accept();
}

void Settings::setParameters(QByteArray *ptr_settingsWindowGeometry, QString *ptr_output_folder,
                             QString *ptr_temp_folder, bool *ptr_protection, bool *ptr_showHDR_mode,
                             int *ptr_timer_interval, int *ptr_theme, QString *ptr_prefixName,
                             QString *ptr_suffixName, int *ptr_prefxType, int *ptr_suffixType,
                             bool *ptr_hideInTrayFlag, QString *ptr_language, const QString &_desktopEnv,
                             int *ptr_fontSize, QString *ptr_font)
{
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    QFont title_font;
    title_font.setPointSize(10);
    ui->label_title->setFont(title_font);

    desktopEnv = _desktopEnv;
    _ptr_settingsWindowGeometry = ptr_settingsWindowGeometry;
    _ptr_showHDR_mode = ptr_showHDR_mode;
    _ptr_output_folder = ptr_output_folder;
    _ptr_temp_folder = ptr_temp_folder;
    _ptr_protection = ptr_protection;
    _ptr_timer_interval = ptr_timer_interval;
    _ptr_theme = ptr_theme;
    _ptr_prefixName = ptr_prefixName;
    _ptr_suffixName = ptr_suffixName;
    _ptr_prefxType = ptr_prefxType;
    _ptr_suffixType = ptr_suffixType;
    _ptr_hideInTrayFlag = ptr_hideInTrayFlag;
    _ptr_language = ptr_language;
    _ptr_font = ptr_font;
    _ptr_fontSize = ptr_fontSize;

    if (*_ptr_settingsWindowGeometry != "default") {
        this->restoreGeometry(*_ptr_settingsWindowGeometry);
        if (this->isMaximized()) _expandWindowsState = true;
    }
    ui->lineEdit_tempPath->setText(*_ptr_temp_folder);
    ui->lineEdit_outPath->setText(*_ptr_output_folder);
    ui->spinBox_protection_timer->setValue(*_ptr_timer_interval);

    if (*_ptr_showHDR_mode == true) {
        ui->checkBox_showHDR->setChecked(true);
    }
    if (*_ptr_hideInTrayFlag == true) {
        ui->checkBox_tray->setChecked(true);
    }
    if (*_ptr_protection == true) {
        ui->checkBox_protection->setChecked(true);
        ui->spinBox_protection_timer->setEnabled(true);
    }
    QMap<QString, int> langIndex;
    langIndex["en"] = 0;
    langIndex["zh"] = 1;
    langIndex["de"] = 2;
    langIndex["ru"] = 3;
    if (langIndex.contains(*_ptr_language)) {
        ui->comboBox_lang->setCurrentIndex(langIndex.value(*_ptr_language));
    }
    QMap<int, int> fontSizeIndex;
    fontSizeIndex[8] = 0;
    fontSizeIndex[9] = 1;
    fontSizeIndex[10] = 2;
    fontSizeIndex[11] = 3;
    fontSizeIndex[12] = 4;
    if (fontSizeIndex.contains(*_ptr_fontSize)) {
        ui->comboBox_fontsize->setCurrentIndex(fontSizeIndex.value(*_ptr_fontSize));
    }

    ui->comboBox_theme->setCurrentIndex(*_ptr_theme);
    ui->comboBoxPrefixType->setCurrentIndex(*_ptr_prefxType);
    ui->comboBoxSuffixType->setCurrentIndex(*_ptr_suffixType);
    if (*_ptr_suffixType == 0) {
        ui->lineEditSuffix->setText(*_ptr_suffixName);
    }

    QFontDatabase database;
    QFontDatabase::WritingSystem values = QFontDatabase::WritingSystem::Latin;
    const QStringList fontFamilies = database.families(values);
    QStringListModel *fontModel = new QStringListModel(this);
    fontModel->setStringList(fontFamilies);
    ui->comboBox_font->blockSignals(true);
    ui->comboBox_font->setModel(fontModel);
    QString appFontFamily = qApp->font().family();
    int fontInd = ui->comboBox_font->findText(appFontFamily);
    if (fontInd != -1) {
        ui->comboBox_font->setCurrentIndex(fontInd);
    }
    ui->comboBox_font->blockSignals(false);

    QListView *comboboxLangListView = new QListView(ui->comboBox_lang);
    QListView *comboboxThemeListView = new QListView(ui->comboBox_theme);
    QListView *comboboxFontListView = new QListView(ui->comboBox_font);
    QListView *comboboxFontSizeListView = new QListView(ui->comboBox_fontsize);
    QListView *comboboxPrefixTypeListView = new QListView(ui->comboBoxPrefixType);
    QListView *comboboxSuffixTypeListView = new QListView(ui->comboBoxSuffixType);
    ui->comboBox_lang->setView(comboboxLangListView);
    ui->comboBox_theme->setView(comboboxThemeListView);
    ui->comboBox_font->setView(comboboxFontListView);
    ui->comboBox_fontsize->setView(comboboxFontSizeListView);
    ui->comboBoxPrefixType->setView(comboboxPrefixTypeListView);
    ui->comboBoxSuffixType->setView(comboboxSuffixTypeListView);

    QRegExpValidator *prefixValidator = new QRegExpValidator(QRegExp("^[^\\\\/:*?\"<>|+%!@]*$"), ui->lineEditPrefix);
    QRegExpValidator *suffixValidator = new QRegExpValidator(QRegExp("^[^\\\\/:*?\"<>|+%!@]*$"), ui->lineEditSuffix);
    ui->lineEditPrefix->setValidator(prefixValidator);
    ui->lineEditSuffix->setValidator(suffixValidator);
}

void Settings::on_closeWindow_clicked()
{
    this->close();
}

void Settings::on_expandWindow_clicked()
{
    if (!this->isMaximized()) {
        _expandWindowsState = true;
        this->showMaximized();
    } else {
        _expandWindowsState = false;
        this->showNormal();
    }
}

void Settings::on_buttonCancel_clicked()
{
    this->close();
}

void Settings::on_buttonApply_clicked()
{
    /*===================== Font ==================*/
    *_ptr_font = ui->comboBox_font->currentText();

    const int font_size_index = ui->comboBox_fontsize->currentIndex();
    int arrFontSize[5] = {8, 9, 10, 11, 12};
    *_ptr_fontSize = arrFontSize[font_size_index];
    /*===================== Theme =================*/
    *_ptr_theme = ui->comboBox_theme->currentIndex();

    /*===================== Lang ==================*/
    const int lang_index = ui->comboBox_lang->currentIndex();
    QString arrLang[4] = {"en", "zh", "de", "ru"};
    *_ptr_language = arrLang[lang_index];

    /*==================== Paths ==================*/
    *_ptr_temp_folder = ui->lineEdit_tempPath->text();
    *_ptr_output_folder = ui->lineEdit_outPath->text();

    /*==================== Tray  ==================*/
    int stts_tray = ui->checkBox_tray->checkState();
    if (stts_tray == 2) {
        *_ptr_hideInTrayFlag = true;
    } else {
        *_ptr_hideInTrayFlag = false;
    }
    /*================= HDR Info  =================*/
    int stts_hdr_info = ui->checkBox_showHDR->checkState();
    if (stts_hdr_info == 2) {
        *_ptr_showHDR_mode = true;
    } else {
        *_ptr_showHDR_mode = false;
    }
    /*================ Protection ================*/
    *_ptr_timer_interval = ui->spinBox_protection_timer->value();

    int stts_protect = ui->checkBox_protection->checkState();
    if (stts_protect == 2) {
        *_ptr_protection = true;
    } else {
        *_ptr_protection = false;
    }
    /*============== Pref and Suff ===============*/
    *_ptr_prefxType = ui->comboBoxPrefixType->currentIndex();
    *_ptr_suffixType = ui->comboBoxSuffixType->currentIndex();
    if (*_ptr_prefxType != 0) {
        *_ptr_prefixName = ui->lineEditPrefix->text();
    }
    if (*_ptr_suffixType == 0) {
        *_ptr_suffixName = ui->lineEditSuffix->text();
    }

    this->accept();
}

void Settings::on_buttonReset_clicked()
{
    ui->lineEdit_tempPath->clear();
    ui->lineEdit_outPath->clear();
    ui->checkBox_showHDR->setChecked(false);
    ui->checkBox_tray->setChecked(false);
    ui->checkBox_protection->setChecked(false);
    ui->spinBox_protection_timer->setEnabled(false);
    ui->comboBox_theme->setCurrentIndex(3);
    ui->comboBox_lang->setCurrentIndex(0);
    ui->comboBoxPrefixType->setCurrentIndex(0);
    ui->comboBoxSuffixType->setCurrentIndex(0);
    ui->lineEditPrefix->setText("output");
    ui->lineEditSuffix->setText("_encoded_");
    ui->comboBox_fontsize->setCurrentIndex(0);
    QFont font;
    QString appFontFamily = font.defaultFamily();
    int fontInd = ui->comboBox_font->findText(appFontFamily);
    if (fontInd != -1) {
        ui->comboBox_font->setCurrentIndex(fontInd);
    }
}

void Settings::on_buttonOutputPath_clicked()
{
    const QString output_folder_name = callFileDialog("Select output folder");
    if (output_folder_name.isEmpty()) {
        return;
    }
    ui->lineEdit_outPath->setText(output_folder_name);
}

void Settings::on_buttonTempPath_clicked()
{
    const QString temp_folder_name = callFileDialog("Select temp folder");
    if (temp_folder_name.isEmpty()) {
        return;
    }
    ui->lineEdit_tempPath->setText(temp_folder_name);
}

void Settings::on_checkBox_protection_clicked()
{
    int stts_protect = ui->checkBox_protection->checkState();
    if (stts_protect == 2) {
        ui->spinBox_protection_timer->setEnabled(true);
    } else {
        ui->spinBox_protection_timer->setEnabled(false);
    }
}

QString Settings::callFileDialog(const QString title)
{
    QFileDialog selectFolderWindow(nullptr);
    selectFolderWindow.setWindowTitle(title);
    selectFolderWindow.setMinimumWidth(600);
    selectFolderWindow.setWindowFlags(Qt::Dialog | Qt::SubWindow);
    if (desktopEnv == "gnome") selectFolderWindow.setOption(QFileDialog::DontUseNativeDialog, true);
    selectFolderWindow.setFileMode(QFileDialog::DirectoryOnly);
    selectFolderWindow.setAcceptMode(QFileDialog::AcceptOpen);
    selectFolderWindow.setDirectory(QDir::homePath());
    if (selectFolderWindow.exec() == QFileDialog::Accepted) {
        return selectFolderWindow.selectedFiles().at(0);
    }
    return QString("");
}

bool Settings::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            ui->frame_middle->setFocus();
            return true;
        }
        return false;
    }

    if (event->type() == QEvent::MouseButtonRelease) // *************** Reset ************************* //
    {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->button() == Qt::LeftButton)
        {
            QGuiApplication::restoreOverrideCursor();
            clickPressedFlag = false;
            clickPressed_Left_ResizeFlag = false;
            clickPressed_Left_Top_ResizeFlag = false;
            clickPressed_Top_ResizeFlag = false;
            clickPressed_Right_Top_ResizeFlag = false;
            clickPressed_Right_ResizeFlag = false;
            clickPressed_Right_Bottom_ResizeFlag = false;
            clickPressed_Bottom_ResizeFlag = false;
            clickPressed_Left_Bottom_ResizeFlag = false;
            return true;
        }
        return false;
    }

    if (watched == ui->widget_main) // *************** Resize window realisation ************************* //
    {
        if (!this->isMaximized())
        {
            if (event->type() == QEvent::HoverLeave)
            {
                QGuiApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
                return true;
            }
            if (event->type() == QEvent::HoverMove && !clickPressed_Left_ResizeFlag
                     && !clickPressed_Left_Top_ResizeFlag && !clickPressed_Top_ResizeFlag
                     && !clickPressed_Right_Top_ResizeFlag && !clickPressed_Right_ResizeFlag
                     && !clickPressed_Right_Bottom_ResizeFlag && !clickPressed_Bottom_ResizeFlag
                     && !clickPressed_Left_Bottom_ResizeFlag)
            {
                curWidth = this->width();
                curHeight = this->height();
                mouseCoordinate = ui->widget_main->mapFromGlobal(QCursor::pos());
                if ((mouseCoordinate.x() < 6) && (mouseCoordinate.y() > 62) && (mouseCoordinate.y() < (curHeight - 6)))
                {
                    QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                    return true;
                }
                if ((mouseCoordinate.x() < 6) && (mouseCoordinate.y() < 6))
                {
                    QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                    return true;
                }
                if ((mouseCoordinate.x() > 6) && (mouseCoordinate.x() < (curWidth - 120)) && (mouseCoordinate.y() < 3))
                {
                    QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                    return true;
                }
                if ((mouseCoordinate.x() > (curWidth - 6)) && (mouseCoordinate.y() < 6))
                {
                    QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                    return true;
                }
                if ((mouseCoordinate.x() > (curWidth - 6)) && (mouseCoordinate.y() > 62) && (mouseCoordinate.y() < (curHeight - 6)))
                {
                    QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                    return true;
                }
                if ((mouseCoordinate.x() > (curWidth - 6)) && (mouseCoordinate.y() > (curHeight - 6)))
                {
                    QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                    return true;
                }
                if ((mouseCoordinate.x() > 6) && (mouseCoordinate.x() < (curWidth - 6)) && (mouseCoordinate.y() > (curHeight - 6)))
                {
                    QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                    return true;
                }
                if ((mouseCoordinate.x() < 6) && (mouseCoordinate.y() > (curHeight - 6)))
                {
                    QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                    return true;
                }
                QGuiApplication::restoreOverrideCursor();
                return false;
            }
            if (event->type() == QEvent::MouseButtonPress)
            {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->button() == Qt::LeftButton)
                {
                    oldWidth = this->width();
                    oldHeight = this->height();
                    mouseClickCoordinate = mouse_event->pos();
                    if ((mouseClickCoordinate.x() < 6) && (mouseClickCoordinate.y() > 62) && (mouseClickCoordinate.y() < (oldHeight - 6)))
                    {
                        clickPressed_Left_ResizeFlag = true;
                        return true;
                    }
                    if ((mouseClickCoordinate.x() < 6) && (mouseClickCoordinate.y() < 6))
                    {
                        clickPressed_Left_Top_ResizeFlag = true;
                        return true;
                    }
                    if ((mouseClickCoordinate.x() > 6) && (mouseClickCoordinate.x() < (oldWidth - 120)) && (mouseClickCoordinate.y() < 3))
                    {
                        clickPressed_Top_ResizeFlag = true;
                        return true;
                    }
                    if ((mouseClickCoordinate.x() > (oldWidth - 6)) && (mouseClickCoordinate.y() < 6))
                    {
                        clickPressed_Right_Top_ResizeFlag = true;
                        return true;
                    }
                    if ((mouseClickCoordinate.x() > (oldWidth - 6)) && (mouseClickCoordinate.y() > 62) && (mouseClickCoordinate.y() < (oldHeight - 6)))
                    {
                        clickPressed_Right_ResizeFlag = true;
                        return true;
                    }
                    if ((mouseClickCoordinate.x() > (oldWidth - 6)) && (mouseClickCoordinate.y() > (oldHeight - 6)))
                    {
                        clickPressed_Right_Bottom_ResizeFlag = true;
                        return true;
                    }
                    if ((mouseClickCoordinate.x() > 6) && (mouseClickCoordinate.x() < (oldWidth - 6)) && (mouseClickCoordinate.y() > (oldHeight - 6)))
                    {
                        clickPressed_Bottom_ResizeFlag = true;
                        return true;
                    }
                    if ((mouseClickCoordinate.x() < 6) && (mouseClickCoordinate.y() > (oldHeight - 6)))
                    {
                        clickPressed_Left_Bottom_ResizeFlag = true;
                        return true;
                    }
                    return false;
                }
                return false;
            }
            if (event->type() == QEvent::MouseMove)
            {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->buttons() & Qt::LeftButton)
                {
                    int deltaX = mouse_event->globalPos().x() - mouseClickCoordinate.x();
                    int deltaY = mouse_event->globalPos().y() - mouseClickCoordinate.y();
                    int deltaWidth = static_cast<int>(mouse_event->localPos().x()) - mouseClickCoordinate.x();
                    int deltaHeight = static_cast<int>(mouse_event->localPos().y()) - mouseClickCoordinate.y();
                    if (clickPressed_Left_ResizeFlag)
                    {
                        this->setGeometry(deltaX, this->pos().y(), this->width() - deltaWidth, oldHeight);
                        return true;
                    }
                    if (clickPressed_Left_Top_ResizeFlag)
                    {
                        this->setGeometry(deltaX, deltaY, this->width() - deltaWidth, this->height() - deltaHeight);
                        return true;
                    }
                    if (clickPressed_Top_ResizeFlag)
                    {
                        this->setGeometry(this->pos().x(), deltaY, oldWidth, this->height() - deltaHeight);
                        return true;
                    }
                    if (clickPressed_Right_Top_ResizeFlag)
                    {
                        this->setGeometry(this->pos().x(), deltaY, oldWidth + deltaWidth, this->height() - deltaHeight);
                        return true;
                    }
                    if (clickPressed_Right_ResizeFlag)
                    {
                        this->setGeometry(this->pos().x(), this->pos().y(), oldWidth + deltaWidth, oldHeight);
                        return true;
                    }
                    if (clickPressed_Right_Bottom_ResizeFlag)
                    {
                        this->setGeometry(this->pos().x(), this->pos().y(), oldWidth + deltaWidth, oldHeight + deltaHeight);
                        return true;
                    }
                    if (clickPressed_Bottom_ResizeFlag)
                    {
                        this->setGeometry(this->pos().x(), this->pos().y(), oldWidth, oldHeight + deltaHeight);
                        return true;
                    }
                    if (clickPressed_Left_Bottom_ResizeFlag)
                    {
                        this->setGeometry(deltaX, this->pos().y(), this->width() - deltaWidth, oldHeight + deltaHeight);
                        return true;
                    }
                    return false;
                }
                return false;
            }
            return false;
        }
        return false;
    }

    if (watched == ui->frame_main) // ******** Resize right frame realisation ********************** //
    {
        if (event->type() == QEvent::HoverMove)
        {
            QGuiApplication::restoreOverrideCursor();
            return true;
        }
        return false;
    }

    if (watched == ui->frame_top) // *************** Drag window realisation ************************* //
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton)
            {
                mouseClickCoordinate = mouse_event->pos();
                clickPressedFlag = true;
                return true;
            }
            return false;
        }
        if ((event->type() == QEvent::MouseMove) && clickPressedFlag == true)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                if (this->isMaximized())
                {
                    on_expandWindow_clicked();
                }
                this->move(mouse_event->globalPos() - mouseClickCoordinate);
                return true;
            }
            return false;
        }
        if (event->type() == QEvent::MouseButtonDblClick)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                on_expandWindow_clicked();
                return true;
            }
            return false;
        }
        return false;
    }
    return QDialog::eventFilter(watched, event);
}

void Settings::on_comboBoxPrefixType_currentIndexChanged(int index)
{
    if (index == 0) {
        ui->lineEditPrefix->setEnabled(false);
        ui->lineEditPrefix->setText(tr("None"));
    } else {
        ui->lineEditPrefix->setEnabled(true);
        ui->lineEditPrefix->setText(*_ptr_prefixName);
    }
}

void Settings::on_comboBoxSuffixType_currentIndexChanged(int index)
{
    if (index == 0) {
        ui->lineEditSuffix->setEnabled(true);
        ui->lineEditSuffix->setText(*_ptr_suffixName);
    } else {
        ui->lineEditSuffix->setEnabled(false);
        ui->lineEditSuffix->setText("_hhmmss_MMddyyyy");
    }
}

void Settings::on_comboBox_font_currentIndexChanged(const QString &arg1)
{
    QFont font;
    font.setFamily(arg1);
    ui->comboBox_font->setFont(font);
}

void Settings::on_buttonTab_1_clicked()
{
    ui->buttonTab_1->setEnabled(false);
    ui->buttonTab_2->setEnabled(true);
    ui->tabWidgetSettings->setCurrentIndex(0);
}

void Settings::on_buttonTab_2_clicked()
{
    ui->buttonTab_1->setEnabled(true);
    ui->buttonTab_2->setEnabled(false);
    ui->tabWidgetSettings->setCurrentIndex(1);
}
