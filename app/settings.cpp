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

typedef void(Settings::*FnVoidVoid)(void);
typedef void(Settings::*FnVoidInt)(int);


Settings::Settings(QWidget *parent):
    BaseWindow(parent, true),
    ui(new Ui::Settings)
{
    ui->setupUi(centralWidget());
    setTitleBar(ui->frame_top);
    ui->frameMiddle->setFocusPolicy(Qt::StrongFocus);

    // Buttons
    QPushButton *btns[] = {
        ui->closeWindow, ui->buttonCancel, ui->buttonApply, ui->buttonReset,
        ui->buttonOutputPath, ui->buttonTempPath
    };
    FnVoidVoid btn_methods[] = {
        &Settings::onCloseWindow, &Settings::onCloseWindow, &Settings::onButtonApply,
        &Settings::onButtonReset, &Settings::onButtonOutputPath, &Settings::onButtonTempPath
    };
    for (int i = 0; i < 6; i++)
        connect(btns[i], &QPushButton::clicked, this, btn_methods[i]);

    // Tab buttons
    QList<QPushButton*> tabButtons = {ui->buttonTab_1, ui->buttonTab_2};
    for (int i = 0; i < tabButtons.size(); i++) {
        connect(tabButtons[i], &QPushButton::clicked, this, [this, i, tabButtons]() {
            for (int j = 0; j < tabButtons.size(); j++)
                tabButtons[j]->setEnabled(i != j);
            ui->tabWidgetSettings->setCurrentIndex(i);
        });
    }

    // Check boxes
    connect(ui->checkBox_protection, &QCheckBox::clicked, this,
            &Settings::onCheckBoxProtectFlag_clicked);

    // Combo boxes
    QComboBox *boxes[] = {
        ui->comboBoxPrefixType, ui->comboBoxSuffixType, ui->comboBox_font
    };
    FnVoidInt boxes_methods[] = {
        &Settings::onComboBoxPrefixType_indexChanged, &Settings::onComboBoxSuffixType_indexChanged,
        &Settings::onComboBoxFont_indexChanged
    };
    for (int i = 0; i < 3; i++)
        connect(boxes[i], static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
                this, boxes_methods[i]);

    // On close
    connect(this, &Settings::destroyed, this, [this]() {
        SETTINGS(stn);
        stn.beginGroup("SettingsWidget");
        stn.setValue("SettingsWidget/geometry", this->saveGeometry());
        stn.endGroup();
    });
}

Settings::~Settings()
{
    delete ui;
}

void Settings::setParameters(QString    *pOutputFolder,
                             QString    *pTempFolder,
                             bool       *pProtectFlag,
                             bool       *pShowHdrFlag,
                             int        *pTimerInterval,
                             int        *pTheme,
                             QString    *pPrefixName,
                             QString    *pSuffixName,
                             int        *pPrefxType,
                             int        *pSuffixType,
                             bool       *pHideInTrayFlag,
                             QString    *pLanguage,
                             int        *pFontSize,
                             QString    *pFont)
{
    QFont title_font;
    title_font.setPointSize(10);
    ui->label_title->setFont(title_font);
    m_pShowHdrFlag = pShowHdrFlag;
    m_pOutputFolder = pOutputFolder;
    m_pTempFolder = pTempFolder;
    m_pProtectFlag = pProtectFlag;
    m_pTimerInterval = pTimerInterval;
    m_pTheme = pTheme;
    m_pPrefixName = pPrefixName;
    m_pSuffixName = pSuffixName;
    m_pPrefxType = pPrefxType;
    m_pSuffixType = pSuffixType;
    m_pHideInTrayFlag = pHideInTrayFlag;
    m_pLanguage = pLanguage;
    m_pFont = pFont;
    m_pFontSize = pFontSize;

    SETTINGS(stn);
    stn.beginGroup("SettingsWidget");
    restoreGeometry(stn.value("SettingsWidget/geometry", geometry()).toByteArray());
    stn.endGroup();

    ui->lineEdit_tempPath->setText(*m_pTempFolder);
    ui->lineEdit_outPath->setText(*m_pOutputFolder);
    ui->spinBox_protectionTimer->setValue(*m_pTimerInterval);

    if (*m_pShowHdrFlag == true) {
        ui->checkBox_showHDR->setChecked(true);
    }
    if (*m_pHideInTrayFlag == true) {
        ui->checkBox_tray->setChecked(true);
    }
    if (*m_pProtectFlag == true) {
        ui->checkBox_protection->setChecked(true);
        ui->spinBox_protectionTimer->setEnabled(true);
    }
    QMap<QString, int> langIndex;
    langIndex["en"] = 0;
    langIndex["zh"] = 1;
    langIndex["de"] = 2;
    langIndex["ru"] = 3;
    if (langIndex.contains(*m_pLanguage)) {
        ui->comboBox_lang->setCurrentIndex(langIndex.value(*m_pLanguage));
    }
    QMap<int, int> fontSizeIndex;
    fontSizeIndex[8] = 0;
    fontSizeIndex[9] = 1;
    fontSizeIndex[10] = 2;
    fontSizeIndex[11] = 3;
    fontSizeIndex[12] = 4;
    if (fontSizeIndex.contains(*m_pFontSize)) {
        ui->comboBox_fontsize->setCurrentIndex(fontSizeIndex.value(*m_pFontSize));
    }

    ui->comboBox_theme->setCurrentIndex(*m_pTheme);
    ui->comboBoxPrefixType->setCurrentIndex(*m_pPrefxType);
    ui->comboBoxSuffixType->setCurrentIndex(*m_pSuffixType);
    if (*m_pSuffixType == 0) {
        ui->lineEditSuffix->setText(*m_pSuffixName);
    }

    QFontDatabase database;
    QFontDatabase::WritingSystem values = QFontDatabase::WritingSystem::Latin;
    const QStringList fontFamilies = database.families(values);
    QStringListModel *fontModel = new QStringListModel(ui->comboBox_font);
    fontModel->setStringList(fontFamilies);
    ui->comboBox_font->blockSignals(true);
    ui->comboBox_font->setModel(fontModel);
    const QString appFontFamily = qApp->font().family();
    const int fontInd = ui->comboBox_font->findText(appFontFamily);
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
    *m_pFont = ui->comboBox_font->currentText();

    const int font_size_index = ui->comboBox_fontsize->currentIndex();
    int arrFontSize[5] = {8, 9, 10, 11, 12};
    *m_pFontSize = arrFontSize[font_size_index];
    /*===================== Theme =================*/
    *m_pTheme = ui->comboBox_theme->currentIndex();

    /*===================== Lang ==================*/
    const int lang_index = ui->comboBox_lang->currentIndex();
    QString arrLang[4] = {"en", "zh", "de", "ru"};
    *m_pLanguage = arrLang[lang_index];

    /*==================== Paths ==================*/
    *m_pTempFolder = ui->lineEdit_tempPath->text();
    *m_pOutputFolder = ui->lineEdit_outPath->text();

    /*==================== Tray  ==================*/
    int stts_tray = ui->checkBox_tray->checkState();
    *m_pHideInTrayFlag = (stts_tray == 2) ? true : false;

    /*================= HDR Info  =================*/
    int stts_hdr_info = ui->checkBox_showHDR->checkState();
    *m_pShowHdrFlag = (stts_hdr_info == 2) ? true : false;

    /*================ Protection ================*/
    *m_pTimerInterval = ui->spinBox_protectionTimer->value();
    int stts_protect = ui->checkBox_protection->checkState();
    *m_pProtectFlag = (stts_protect == 2) ? true : false;

    /*============== Pref and Suff ===============*/
    *m_pPrefxType = ui->comboBoxPrefixType->currentIndex();
    *m_pSuffixType = ui->comboBoxSuffixType->currentIndex();
    if (*m_pPrefxType != 0) {
        *m_pPrefixName = ui->lineEditPrefix->text();
    }
    if (*m_pSuffixType == 0) {
        *m_pSuffixName = ui->lineEditSuffix->text();
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
    ui->spinBox_protectionTimer->setEnabled(false);
    ui->comboBox_theme->setCurrentIndex(3);
    ui->comboBox_lang->setCurrentIndex(0);
    ui->comboBoxPrefixType->setCurrentIndex(0);
    ui->comboBoxSuffixType->setCurrentIndex(0);
    ui->lineEditPrefix->setText(DEFAULTPREFIX);
    ui->lineEditSuffix->setText(DEFAULTSUFFIX);
    ui->comboBox_fontsize->setCurrentIndex(0);
    QFont font;
    QString appFontFamily = font.defaultFamily();
    int fontInd = ui->comboBox_font->findText(appFontFamily);
    if (fontInd != -1) {
        ui->comboBox_font->setCurrentIndex(fontInd);
    }
}

bool Settings::eventFilter(QObject *watched, QEvent *event)
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

void Settings::onButtonOutputPath()
{
    QStringList result;
    Helper::openFileDialog(Helper::FileDialogType::SELECTFOLDER,
                           tr("Select output folder"),
                           *m_pOutputFolder,
                           result);
    if (!result.isEmpty()) {
        ui->lineEdit_outPath->setText(result.at(0));
    }
}

void Settings::onButtonTempPath()
{
    QStringList result;
    Helper::openFileDialog(Helper::FileDialogType::SELECTFOLDER,
                           tr("Select temp folder"),
                           *m_pTempFolder,
                           result);
    if (!result.isEmpty()) {
        ui->lineEdit_tempPath->setText(result.at(0));
    }
}

void Settings::onCheckBoxProtectFlag_clicked()
{
    int stts_protect = ui->checkBox_protection->checkState();
    ui->spinBox_protectionTimer->setEnabled(stts_protect == 2);
}

void Settings::onComboBoxPrefixType_indexChanged(int index)
{
    if (index == 0) {
        ui->lineEditPrefix->setEnabled(false);
        ui->lineEditPrefix->setText(tr("None"));
    } else {
        ui->lineEditPrefix->setEnabled(true);
        ui->lineEditPrefix->setText(*m_pPrefixName);
    }
}

void Settings::onComboBoxSuffixType_indexChanged(int index)
{
    if (index == 0) {
        ui->lineEditSuffix->setEnabled(true);
        ui->lineEditSuffix->setText(*m_pSuffixName);
    } else {
        ui->lineEditSuffix->setEnabled(false);
        ui->lineEditSuffix->setText("_hhmmss_MMddyyyy");
    }
}

void Settings::onComboBoxFont_indexChanged(int index)
{
    QFont font;
    const QString family = ui->comboBox_font->itemText(index);
    font.setFamily(family);
    ui->comboBox_font->setFont(font);
}
