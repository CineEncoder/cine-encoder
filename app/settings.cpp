/****************************************************************
SETTINGS.CPP
****************************************************************/

#include "settings.h"
#include "ui_settings.h"
#include "taskcomplete.h"


Settings::Settings(QWidget *parent): QDialog(parent), ui_settings(new Ui::Settings)
{
    ui_settings->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);

    ui_settings->widget_main->installEventFilter(this);
    ui_settings->widget_main->setAttribute(Qt::WA_Hover, true);

    ui_settings->frame_main->installEventFilter(this);
    ui_settings->frame_main->setAttribute(Qt::WA_Hover, true);
    ui_settings->frame_main->setAttribute(Qt::WA_NoMousePropagation, true);

    ui_settings->frame_top->installEventFilter(this);
}

Settings::~Settings()
{
    delete ui_settings;
}

void Settings::closeEvent(QCloseEvent *close_settings)  /*** Show prompt when close app ***/
{
    close_settings->ignore();
    *_ptr_settingsWindowGeometry = this->saveGeometry();
    if (_flag_save == false)
    {
        *_ptr_output_folder = _curr_output_folder;
        *_ptr_temp_folder = _curr_temp_folder;
        *_ptr_showHDR_mode = _curr_showHDR_mode;
        *_ptr_protection = _curr_protection;
        *_ptr_timer_interval = _curr_timer_interval;
        *_ptr_theme = _curr_theme;
        *_ptr_prefixName = _curr_prefixName;
        *_ptr_suffixName = _curr_suffixName;
        *_ptr_prefxType = _curr_prefxType;
        *_ptr_suffixType = _curr_suffixType;
        *_ptr_hideInTrayFlag = _curr_hideInTrayFlag;
        *_ptr_language = _curr_language;
        *_ptr_font = _curr_font;
        *_ptr_fontSize = _curr_fontSize;
    }
    close_settings->accept();
}

void Settings::setParameters(QByteArray *ptr_settingsWindowGeometry, QFile *ptr_stn_file,
                             QString *ptr_output_folder, QString *ptr_temp_folder,
                             bool *ptr_protection, bool *ptr_showHDR_mode, int *ptr_timer_interval,
                             int *ptr_theme, QString *ptr_prefixName, QString *ptr_suffixName,
                             int *ptr_prefxType, int *ptr_suffixType, bool *ptr_hideInTrayFlag,
                             QString *ptr_language, bool *ptr_aceptFlag, QString &_desktopEnv,
                             int *ptr_fontSize, QString *ptr_font)  /*** Set parameters ***/
{
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    QFont title_font;
    title_font.setPointSize(10);
    ui_settings->label_title->setFont(title_font);

    desktopEnv = _desktopEnv;
    _ptr_settingsWindowGeometry = ptr_settingsWindowGeometry;
    _ptr_showHDR_mode = ptr_showHDR_mode;
    _ptr_stn_file = ptr_stn_file;
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
    _ptr_acceptFlag = ptr_aceptFlag;

    if (*_ptr_settingsWindowGeometry != "default") {
        this->restoreGeometry(*_ptr_settingsWindowGeometry);
        if (this->isMaximized()) {
            _expandWindowsState = true;
        }
    }
    ui_settings->lineEdit_9->setText(*_ptr_temp_folder);
    ui_settings->lineEdit_10->setText(*_ptr_output_folder);
    ui_settings->spinBox_3->setValue(*_ptr_timer_interval);

    if (*_ptr_showHDR_mode == true)
    {
        ui_settings->checkBox_2->setChecked(true);
    }
    if (*_ptr_hideInTrayFlag == true)
    {
        ui_settings->checkBox_tray->setChecked(true);
    }
    if (*_ptr_protection == true)
    {
        ui_settings->checkBox_3->setChecked(true);
        ui_settings->spinBox_3->setEnabled(true);
    }
    QMap<QString, int> langIndex;
    langIndex["en"] = 0;
    langIndex["zh"] = 1;
    langIndex["de"] = 2;
    langIndex["ru"] = 3;
    if (langIndex.contains(*_ptr_language)) {
        ui_settings->comboBox_lang->setCurrentIndex(langIndex.value(*_ptr_language));
    }
    QMap<int, int> fontSizeIndex;
    fontSizeIndex[8] = 0;
    fontSizeIndex[9] = 1;
    fontSizeIndex[10] = 2;
    fontSizeIndex[11] = 3;
    fontSizeIndex[12] = 4;
    if (fontSizeIndex.contains(*_ptr_fontSize)) {
        ui_settings->comboBox_fontsize->setCurrentIndex(fontSizeIndex.value(*_ptr_fontSize));
    }

    ui_settings->comboBox_theme->setCurrentIndex(*_ptr_theme);
    ui_settings->comboBoxPrefixType->setCurrentIndex(*_ptr_prefxType);
    ui_settings->comboBoxSuffixType->setCurrentIndex(*_ptr_suffixType);
    if (*_ptr_suffixType == 0) {
        ui_settings->lineEditSuffix->setText(*_ptr_suffixName);
    }

    QFontDatabase database;
    QFontDatabase::WritingSystem values = QFontDatabase::WritingSystem::Latin;
    const QStringList fontFamilies = database.families(values);
    QStringListModel *fontModel = new QStringListModel(this);
    fontModel->setStringList(fontFamilies);
    ui_settings->comboBox_font->blockSignals(true);
    ui_settings->comboBox_font->setModel(fontModel);
    QString appFontFamily = qApp->font().family();
    int fontInd = ui_settings->comboBox_font->findText(appFontFamily);
    if (fontInd != -1) {
        ui_settings->comboBox_font->setCurrentIndex(fontInd);
    }
    ui_settings->comboBox_font->blockSignals(false);

    _curr_output_folder = *_ptr_output_folder;
    _curr_temp_folder = *_ptr_temp_folder;
    _curr_showHDR_mode = *_ptr_showHDR_mode;
    _curr_protection = *_ptr_protection;
    _curr_timer_interval = *_ptr_timer_interval;
    _curr_theme = *_ptr_theme;
    _curr_prefixName = *_ptr_prefixName;
    _curr_suffixName = *_ptr_suffixName;
    _curr_prefxType = *_ptr_prefxType;
    _curr_suffixType = *_ptr_suffixType;
    _curr_hideInTrayFlag = *_ptr_hideInTrayFlag;
    _curr_language = *_ptr_language;
    _curr_font = *_ptr_font;
    _curr_fontSize = *_ptr_fontSize;
    _flag_save = false;

    QListView *comboboxLangListView = new QListView(ui_settings->comboBox_lang);
    QListView *comboboxThemeListView = new QListView(ui_settings->comboBox_theme);
    QListView *comboboxFontListView = new QListView(ui_settings->comboBox_font);
    QListView *comboboxFontSizeListView = new QListView(ui_settings->comboBox_fontsize);
    QListView *comboboxPrefixTypeListView = new QListView(ui_settings->comboBoxPrefixType);
    QListView *comboboxSuffixTypeListView = new QListView(ui_settings->comboBoxSuffixType);
    ui_settings->comboBox_lang->setView(comboboxLangListView);
    ui_settings->comboBox_theme->setView(comboboxThemeListView);
    ui_settings->comboBox_font->setView(comboboxFontListView);
    ui_settings->comboBox_fontsize->setView(comboboxFontSizeListView);
    ui_settings->comboBoxPrefixType->setView(comboboxPrefixTypeListView);
    ui_settings->comboBoxSuffixType->setView(comboboxSuffixTypeListView);
}

void Settings::on_closeWindow_clicked() /*** Close settings window ***/
{
    this->close();
}

void Settings::on_expandWindow_clicked()
{
    if (_expandWindowsState == false)
    {
        this->showMaximized();
        _expandWindowsState = true;
    } else {
        this->showNormal();
        _expandWindowsState = false;
    }
}

void Settings::on_buttonCancel_clicked() /*** Close settings window ***/
{
    this->close();
}

void Settings::on_buttonApply_clicked() /*** Save settings ***/
{
    if ((*_ptr_stn_file).open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString line_0 = QString("temp_folder:") + *_ptr_temp_folder + QString("\n");
        (*_ptr_stn_file).write(line_0.toUtf8());

        QString line_1 = QString("output_folder:") + *_ptr_output_folder + QString("\n");
        (*_ptr_stn_file).write(line_1.toUtf8());

        if (*_ptr_showHDR_mode == true) {
            (*_ptr_stn_file).write("show_hdr:true\n");
        } else {
            (*_ptr_stn_file).write("show_hdr:false\n");
        }

        if (*_ptr_protection == true) {
            (*_ptr_stn_file).write("protection:true\n");
        } else {
            (*_ptr_stn_file).write("protection:false\n");
        }

        *_ptr_timer_interval = ui_settings->spinBox_3->value();
        QString line_2 = QString("timer_interval:") + QString::number(*_ptr_timer_interval) + QString("\n");
        (*_ptr_stn_file).write(line_2.toUtf8());

        *_ptr_theme = ui_settings->comboBox_theme->currentIndex();
        QString line_3 = QString("theme:") + QString::number(*_ptr_theme) + QString("\n");
        (*_ptr_stn_file).write(line_3.toUtf8());

        *_ptr_prefxType = ui_settings->comboBoxPrefixType->currentIndex();
        QString line_4 = QString("prefix_type:") + QString::number(*_ptr_prefxType) + QString("\n");
        (*_ptr_stn_file).write(line_4.toUtf8());

        *_ptr_suffixType = ui_settings->comboBoxSuffixType->currentIndex();
        QString line_5 = QString("suffix_type:") + QString::number(*_ptr_suffixType) + QString("\n");
        (*_ptr_stn_file).write(line_5.toUtf8());

        if (*_ptr_prefxType != 0) {
            *_ptr_prefixName = ui_settings->lineEditPrefix->text();
        }
        QString line_6 = QString("prefix_name:") + (*_ptr_prefixName).replace("\\", "").replace("/", "")
                        .replace(":", "").replace("*", "").replace("?", "").replace("\"", "").replace("<", "")
                        .replace(">", "").replace("|", "").replace("+", "").replace("%", "").replace("!", "")
                        .replace("@", "")+ QString("\n");
        (*_ptr_stn_file).write(line_6.toUtf8());

        if (*_ptr_suffixType == 0) {
            *_ptr_suffixName = ui_settings->lineEditSuffix->text();
        }
        QString line_7 = QString("suffix_name:") + (*_ptr_suffixName).replace("\\", "").replace("/", "")
                        .replace(":", "").replace("*", "").replace("?", "").replace("\"", "").replace("<", "")
                        .replace(">", "").replace("|", "").replace("+", "").replace("%", "").replace("!", "")
                        .replace("@", "")+ QString("\n");
        (*_ptr_stn_file).write(line_7.toUtf8());

        (*_ptr_stn_file).close();
        *_ptr_font = ui_settings->comboBox_font->currentText();
        _flag_save = true;
        *_ptr_acceptFlag = true;
        this->close();
    }
    else
    {
        _message = tr("Settings file not found!\n");
        call_task_complete(_message, false);
    };
}

void Settings::on_buttonReset_clicked() /*** Reset settings ***/
{
    ui_settings->lineEdit_9->clear();
    ui_settings->lineEdit_10->clear();
    ui_settings->checkBox_2->setChecked(false);
    ui_settings->checkBox_3->setChecked(false);
    ui_settings->spinBox_3->setEnabled(false);
    ui_settings->comboBox_theme->setCurrentIndex(3);
    ui_settings->comboBoxPrefixType->setCurrentIndex(0);
    ui_settings->comboBoxSuffixType->setCurrentIndex(0);
    ui_settings->comboBox_lang->setCurrentIndex(0);
    ui_settings->comboBox_fontsize->setCurrentIndex(0);
    ui_settings->lineEditSuffix->setText("_encoded_");
    *_ptr_prefixName = "output";
    *_ptr_suffixName = "_encoded_";
    *_ptr_temp_folder = "";
    *_ptr_output_folder = "";
    *_ptr_protection = false;
    *_ptr_showHDR_mode = false;
    *_ptr_theme = 3;
    //*_ptr_language = "en";
    //*_ptr_fontSize = 8;
    QFont font;
    QString appFontFamily = font.defaultFamily();
    int fontInd = ui_settings->comboBox_font->findText(appFontFamily);
    if (fontInd != -1) {
        ui_settings->comboBox_font->setCurrentIndex(fontInd);
    }
}

void Settings::on_buttonOutputPath_clicked()  /*** Select output folder ***/
{
    QString output_folder_name = callFileDialog("Select output folder");
    if (output_folder_name.isEmpty()) {
        return;
    }
    ui_settings->lineEdit_10->setText(output_folder_name);
    *_ptr_output_folder = output_folder_name;
}

void Settings::on_buttonTempPath_clicked() /*** Select temp folder ***/
{
    QString temp_folder_name = callFileDialog("Select temp folder");
    if (temp_folder_name.isEmpty()) {
        return;
    }
    ui_settings->lineEdit_9->setText(temp_folder_name);
    *_ptr_temp_folder = temp_folder_name;
}

void Settings::on_checkBox_2_clicked()  /*** Show HDR info select ***/
{
    int stts_2 = ui_settings->checkBox_2->checkState();
    if (stts_2 == 2) {
        *_ptr_showHDR_mode = true;
    } else {
        *_ptr_showHDR_mode = false;
    }
}

void Settings::on_checkBox_3_clicked()  /*** Protection mode select ***/
{
    int stts_3 = ui_settings->checkBox_3->checkState();
    if (stts_3 == 2) {
        *_ptr_protection = true;
        ui_settings->spinBox_3->setEnabled(true);
    } else {
        *_ptr_protection = false;
        ui_settings->spinBox_3->setEnabled(false);
    };
}

void Settings::on_checkBox_tray_clicked()
{
    int stts_tray = ui_settings->checkBox_tray->checkState();
    if (stts_tray == 2) {
        *_ptr_hideInTrayFlag = true;
    } else {
        *_ptr_hideInTrayFlag = false;
    }
}

QString Settings::callFileDialog(const QString title)  /*** Call file dialog ***/
{
    QFileDialog *selectFolderWindow = new QFileDialog(nullptr);
    selectFolderWindow->setFileMode(QFileDialog::DirectoryOnly);
#ifdef Q_OS_WIN
    selectFolderWindow->setOptions(QFileDialog::ShowDirsOnly |
                                   QFileDialog::ReadOnly);
#endif
    if (desktopEnv == "gnome") {
        selectFolderWindow->setOptions(QFileDialog::ShowDirsOnly |
                                       QFileDialog::DontUseNativeDialog |
                                       QFileDialog::ReadOnly);
    } else {
        selectFolderWindow->setOptions(QFileDialog::ShowDirsOnly |
                                       QFileDialog::ReadOnly);
    }
    selectFolderWindow->setDirectory(QDir::homePath());
    selectFolderWindow->setMinimumWidth(600);
    selectFolderWindow->setWindowTitle(title);
    selectFolderWindow->setWindowFlags(Qt::Dialog | Qt::SubWindow);
    selectFolderWindow->exec();
    if (selectFolderWindow->result() == 0) {
        return QString("");
    }
    const QStringList folder_name_dir = selectFolderWindow->selectedFiles();
    delete selectFolderWindow;
    QString folder_name = folder_name_dir[0];
    return folder_name;
}

void Settings::call_task_complete(const QString &_message, const bool &_timer_mode)   /*** Call task complete ***/
{
    Taskcomplete taskcomplete(this);
    taskcomplete.setMessage(_message, _timer_mode);
    taskcomplete.setModal(true);
    taskcomplete.exec();
}

bool Settings::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            ui_settings->frame_middle->setFocus();
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

    if (watched == ui_settings->widget_main) // *************** Resize window realisation ************************* //
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
                mouseCoordinate = ui_settings->widget_main->mapFromGlobal(QCursor::pos());
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

    if (watched == ui_settings->frame_main) // ******** Resize right frame realisation ********************** //
    {
        if (event->type() == QEvent::HoverMove)
        {
            QGuiApplication::restoreOverrideCursor();
            return true;
        }
        return false;
    }

    if (watched == ui_settings->frame_top) // *************** Drag window realisation ************************* //
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
        ui_settings->lineEditPrefix->setEnabled(false);
        ui_settings->lineEditPrefix->setText(tr("None"));
    } else {
        ui_settings->lineEditPrefix->setEnabled(true);
        ui_settings->lineEditPrefix->setText(*_ptr_prefixName);
    }
}

void Settings::on_comboBoxSuffixType_currentIndexChanged(int index)
{
    if (index == 0) {
        ui_settings->lineEditSuffix->setEnabled(true);
        ui_settings->lineEditSuffix->setText(*_ptr_suffixName);
    } else {
        ui_settings->lineEditSuffix->setEnabled(false);
        ui_settings->lineEditSuffix->setText("_hhmmss_MMddyyyy");
    }
}

void Settings::on_comboBox_lang_currentIndexChanged(int index)
{
    QString arrLang[4] = {
        "en", "zh", "de", "ru"
    };
    *_ptr_language = arrLang[index];
}

void Settings::on_comboBox_fontsize_currentIndexChanged(int index)
{
    int arrFontSize[5] = {
        8, 9, 10, 11, 12
    };
    *_ptr_fontSize = arrFontSize[index];
}

void Settings::on_comboBox_font_currentIndexChanged(const QString &arg1)
{
    QFont font;
    font.setFamily(arg1);
    ui_settings->comboBox_font->setFont(font);
}

void Settings::on_buttonTab_1_clicked()
{
    ui_settings->buttonTab_1->setEnabled(false);
    ui_settings->buttonTab_2->setEnabled(true);
    ui_settings->tabWidgetSettings->setCurrentIndex(0);
}

void Settings::on_buttonTab_2_clicked()
{
    ui_settings->buttonTab_1->setEnabled(true);
    ui_settings->buttonTab_2->setEnabled(false);
    ui_settings->tabWidgetSettings->setCurrentIndex(1);
}
