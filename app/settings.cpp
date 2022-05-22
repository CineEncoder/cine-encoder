/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: settings.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "settings.h"
#include "ui_settings.h"
#include <QFileDialog>
#include <QKeyEvent>
#include <QListView>
#include <QMap>
#include <QFontDatabase>
#include <QStringListModel>
#include "constants.h"


Settings::Settings(QWidget *parent):
    BaseWindow(parent, true),
    ui(new Ui::Settings)
{
    QWidget *ui_widget = new QWidget(this);
    setCentralWidget(ui_widget);
    ui->setupUi(ui_widget);
    setTitleBar(ui->frame_top);
#ifdef Q_OS_UNIX
    setMaskWidget(ui_widget);
#endif
    ui->frame_middle->setFocusPolicy(Qt::StrongFocus);

    connect(ui->closeWindow, &QPushButton::clicked, this, &Settings::onCloseWindow);
    connect(ui->buttonCancel, &QPushButton::clicked, this, &Settings::onCloseWindow);
    connect(ui->buttonApply, &QPushButton::clicked, this, &Settings::onButtonApply);
    connect(ui->buttonReset, &QPushButton::clicked, this, &Settings::onButtonReset);
    connect(ui->buttonTab_1, &QPushButton::clicked, this, &Settings::onButtonTab_1);
    connect(ui->buttonTab_2, &QPushButton::clicked, this, &Settings::onButtonTab_2);
    connect(ui->buttonOutputPath, &QPushButton::clicked, this, &Settings::onButtonOutputPath);
    connect(ui->buttonTempPath, &QPushButton::clicked, this, &Settings::onButtonTempPath);
    connect(ui->checkBox_protection, &QCheckBox::clicked, this, &Settings::onCheckBox_protection_clicked);
    connect(ui->comboBoxPrefixType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Settings::onComboBoxPrefixType_indexChanged);
    connect(ui->comboBoxSuffixType, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Settings::onComboBoxSuffixType_indexChanged);
    connect(ui->comboBox_font, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &Settings::onComboBox_font_indexChanged);

    connect(this, &Settings::destroyed, this, [this]() {
        SETTINGS(_settings);
        _settings.beginGroup("SettingsWidget");
        _settings.setValue("SettingsWidget/geometry", this->saveGeometry());
        _settings.endGroup();
    });
}

Settings::~Settings()
{
    delete ui;
}

void Settings::setParameters(QString    *ptr_output_folder,
                             QString    *ptr_temp_folder,
                             bool       *ptr_protection,
                             bool       *ptr_showHDR_mode,
                             int        *ptr_timer_interval,
                             int        *ptr_theme,
                             QString    *ptr_prefixName,
                             QString    *ptr_suffixName,
                             int        *ptr_prefxType,
                             int        *ptr_suffixType,
                             bool       *ptr_hideInTrayFlag,
                             QString    *ptr_language,
                             int        *ptr_fontSize,
                             QString    *ptr_font)
{
    QFont title_font;
    title_font.setPointSize(10);
    ui->label_title->setFont(title_font);
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

    SETTINGS(_settings);
    _settings.beginGroup("SettingsWidget");
    this->restoreGeometry(_settings.value("SettingsWidget/geometry", geometry()).toByteArray());
    _settings.endGroup();

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

void Settings::onCloseWindow()
{
    closeDialog();
}

void Settings::onButtonApply()
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
    *_ptr_hideInTrayFlag = (stts_tray == 2) ? true : false;

    /*================= HDR Info  =================*/
    int stts_hdr_info = ui->checkBox_showHDR->checkState();
    *_ptr_showHDR_mode = (stts_hdr_info == 2) ? true : false;

    /*================ Protection ================*/
    *_ptr_timer_interval = ui->spinBox_protection_timer->value();
    int stts_protect = ui->checkBox_protection->checkState();
    *_ptr_protection = (stts_protect == 2) ? true : false;

    /*============== Pref and Suff ===============*/
    *_ptr_prefxType = ui->comboBoxPrefixType->currentIndex();
    *_ptr_suffixType = ui->comboBoxSuffixType->currentIndex();
    if (*_ptr_prefxType != 0) {
        *_ptr_prefixName = ui->lineEditPrefix->text();
    }
    if (*_ptr_suffixType == 0) {
        *_ptr_suffixName = ui->lineEditSuffix->text();
    }

    acceptDialog();
}

void Settings::onButtonReset()
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

void Settings::onButtonTab_1()
{
    ui->buttonTab_1->setEnabled(false);
    ui->buttonTab_2->setEnabled(true);
    ui->tabWidgetSettings->setCurrentIndex(0);
}

void Settings::onButtonTab_2()
{
    ui->buttonTab_1->setEnabled(true);
    ui->buttonTab_2->setEnabled(false);
    ui->tabWidgetSettings->setCurrentIndex(1);
}

bool Settings::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            ui->frame_middle->setFocus();
            return true;
        }
    }
    return BaseWindow::eventFilter(watched, event);
}

void Settings::onButtonOutputPath()
{
    QStringList result;
    Helper::openFileDialog(FileDialogType::SELECTFOLDER,
                           tr("Select output folder"),
                           *_ptr_output_folder,
                           result);
    if (!result.isEmpty()) {
        ui->lineEdit_outPath->setText(result.at(0));
    }
}

void Settings::onButtonTempPath()
{
    QStringList result;
    Helper::openFileDialog(FileDialogType::SELECTFOLDER,
                           tr("Select temp folder"),
                           *_ptr_temp_folder,
                           result);
    if (!result.isEmpty()) {
        ui->lineEdit_tempPath->setText(result.at(0));
    }
}

void Settings::onCheckBox_protection_clicked()
{
    int stts_protect = ui->checkBox_protection->checkState();
    ui->spinBox_protection_timer->setEnabled(stts_protect == 2);
}

void Settings::onComboBoxPrefixType_indexChanged(int index)
{
    if (index == 0) {
        ui->lineEditPrefix->setEnabled(false);
        ui->lineEditPrefix->setText(tr("None"));
    } else {
        ui->lineEditPrefix->setEnabled(true);
        ui->lineEditPrefix->setText(*_ptr_prefixName);
    }
}

void Settings::onComboBoxSuffixType_indexChanged(int index)
{
    if (index == 0) {
        ui->lineEditSuffix->setEnabled(true);
        ui->lineEditSuffix->setText(*_ptr_suffixName);
    } else {
        ui->lineEditSuffix->setEnabled(false);
        ui->lineEditSuffix->setText("_hhmmss_MMddyyyy");
    }
}

void Settings::onComboBox_font_indexChanged(int index)
{
    QFont font;
    const QString family = ui->comboBox_font->itemText(index);
    font.setFamily(family);
    ui->comboBox_font->setFont(font);
}
