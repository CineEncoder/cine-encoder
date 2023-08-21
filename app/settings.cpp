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
#include <QColorDialog>
#include "constants.h"

typedef void(Settings::*FnVoidVoid)(void);
typedef void(Settings::*FnVoidInt)(int);

using namespace Constants;

Settings::Settings(QWidget *parent):
    BaseWindow(parent, true),
    ui(new Ui::Settings),
    m_windowActivated(false)
{
    ui->setupUi(centralWidget());
    setTitleBar(ui->frame_top);
    ui->frameMiddle->setFocusPolicy(Qt::StrongFocus);
    ui->frame_main->setProperty("scale", int(Helper::scaling() * 100));

    // Buttons
    QPushButton *btns[] = {
        ui->closeWindow, ui->buttonCancel, ui->buttonApply, ui->buttonReset,
        ui->buttonOutputPath, ui->buttonTempPath,
        ui->subtitles_background_color, ui->subtitles_color
    };
    FnVoidVoid btn_methods[] = {
        &Settings::onCloseWindow, &Settings::onCloseWindow, &Settings::onButtonApply,
        &Settings::onButtonReset, &Settings::onButtonOutputPath, &Settings::onButtonTempPath,
        &Settings::subtitles_background_color_change, &Settings::subtitles_color_change
    };
    for (int i = 0; i < 8; i++)
        connect(btns[i], &QPushButton::clicked, this, btn_methods[i]);

    // Tab buttons
    QList<QPushButton*> tabButtons = {ui->buttonTab_1, ui->buttonTab_2, ui->buttonTab_3};
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
        ui->comboBoxPrefixType, ui->comboBoxSuffixType, ui->comboBox_font, ui->comboBox_subtitles_font
    };
    FnVoidInt boxes_methods[] = {
        &Settings::onComboBoxPrefixType_indexChanged, &Settings::onComboBoxSuffixType_indexChanged,
        &Settings::onComboBoxFont_indexChanged, &Settings::onComboBoxSubtitlesFont_indexChanged
    };
    for (int i = 0; i < 5; i++)
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
                             bool       *pMultiInstances,
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
                             QString    *pFont,
                             int        *pSubtitlesFontSize,
                             QString    *pSubtitlesFont,
                             bool       *pSubtitlesBackground,
                             QColor     *pSubtitlesColor,
                             QColor     *pSubtitlesBackgroundColor)
{
    QFont title_font;
    title_font.setPointSize(10);
    ui->label_title->setFont(title_font);
    m_pShowHdrFlag = pShowHdrFlag;
    m_pOutputFolder = pOutputFolder;
    m_pTempFolder = pTempFolder;
    m_pProtectFlag = pProtectFlag;
    m_pMultiInstances = pMultiInstances;
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
    m_pSubtitlesFont = pSubtitlesFont;
    m_pSubtitlesFontSize = pSubtitlesFontSize;
    m_pSubtitlesBackground = pSubtitlesBackground;
    m_pSubtitlesColor = pSubtitlesColor;
    m_pSubtitlesBackgroundColor = pSubtitlesBackgroundColor;

    ui->lineEdit_tempPath->setText(*m_pTempFolder);
    ui->lineEdit_outPath->setText(*m_pOutputFolder);
    ui->spinBox_protectionTimer->setValue(*m_pTimerInterval);

    if (*m_pShowHdrFlag) {
        ui->checkBox_showHDR->setChecked(true);
    }
    if (*m_pHideInTrayFlag) {
        ui->checkBox_tray->setChecked(true);
    }
    if (*m_pProtectFlag) {
        ui->checkBox_protection->setChecked(true);
        ui->spinBox_protectionTimer->setEnabled(true);
    }
    if (*m_pMultiInstances) {
        ui->checkBox_allowDuplicates->setChecked(true);
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
    fontSizeIndex[13] = 5;
    if (fontSizeIndex.contains(*m_pFontSize)) {
        ui->comboBox_fontsize->setCurrentIndex(fontSizeIndex.value(*m_pFontSize));
    }
    if (fontSizeIndex.contains(*m_pSubtitlesFontSize)) {
        ui->comboBox_subtitles_fontsize->setCurrentIndex(fontSizeIndex.value(*m_pSubtitlesFontSize));
    }

    ui->comboBox_theme->setCurrentIndex(*m_pTheme < 2 ? *m_pTheme : 0);
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

    QStringListModel *subtitlesFontModel = new QStringListModel(ui->comboBox_subtitles_font);
    subtitlesFontModel->setStringList(fontFamilies);
    ui->comboBox_subtitles_font->blockSignals(true);
    ui->comboBox_subtitles_font->setModel(subtitlesFontModel);
    const int subtitlesFontInd = ui->comboBox_subtitles_font->findText(*pSubtitlesFont);
    if (subtitlesFontInd != -1) {
        ui->comboBox_subtitles_font->setCurrentIndex(subtitlesFontInd);
    }
    ui->comboBox_subtitles_font->blockSignals(false);

    QListView *comboboxLangListView = new QListView(ui->comboBox_lang);
    QListView *comboboxThemeListView = new QListView(ui->comboBox_theme);
    QListView *comboboxFontListView = new QListView(ui->comboBox_font);
    QListView *comboboxFontSizeListView = new QListView(ui->comboBox_fontsize);
    QListView *comboboxSubtitlesFontListView = new QListView(ui->comboBox_subtitles_font);
    QListView *comboboxSubtitlesFontSizeListView = new QListView(ui->comboBox_subtitles_fontsize);
    QListView *comboboxPrefixTypeListView = new QListView(ui->comboBoxPrefixType);
    QListView *comboboxSuffixTypeListView = new QListView(ui->comboBoxSuffixType);
    ui->comboBox_lang->setView(comboboxLangListView);
    ui->comboBox_theme->setView(comboboxThemeListView);
    ui->comboBox_font->setView(comboboxFontListView);
    ui->comboBox_fontsize->setView(comboboxFontSizeListView);
    ui->comboBox_subtitles_font->setView(comboboxSubtitlesFontListView);
    ui->comboBox_subtitles_fontsize->setView(comboboxSubtitlesFontSizeListView);
    ui->comboBoxPrefixType->setView(comboboxPrefixTypeListView);
    ui->comboBoxSuffixType->setView(comboboxSuffixTypeListView);

    QRegExpValidator *prefixValidator = new QRegExpValidator(QRegExp("^[^\\\\/:*?\"<>|+%!@]*$"), ui->lineEditPrefix);
    QRegExpValidator *suffixValidator = new QRegExpValidator(QRegExp("^[^\\\\/:*?\"<>|+%!@]*$"), ui->lineEditSuffix);
    ui->lineEditPrefix->setValidator(prefixValidator);
    ui->lineEditSuffix->setValidator(suffixValidator);

    QString sb("background: #"
              + QString(m_pSubtitlesBackgroundColor->red() < 16? "0" : "") + QString::number(m_pSubtitlesBackgroundColor->red(),16)
              + QString(m_pSubtitlesBackgroundColor->green() < 16? "0" : "") + QString::number(m_pSubtitlesBackgroundColor->green(),16)
              + QString(m_pSubtitlesBackgroundColor->blue() < 16? "0" : "") + QString::number(m_pSubtitlesBackgroundColor->blue(),16) + ";");
    ui->subtitles_background_color->setStyleSheet(sb);
    ui->subtitles_background_color->update();

    QString s("background: #"
              + QString(m_pSubtitlesColor->red() < 16? "0" : "") + QString::number(m_pSubtitlesColor->red(),16)
              + QString(m_pSubtitlesColor->green() < 16? "0" : "") + QString::number(m_pSubtitlesColor->green(),16)
              + QString(m_pSubtitlesColor->blue() < 16? "0" : "") + QString::number(m_pSubtitlesColor->blue(),16) + ";");
    ui->subtitles_color->setStyleSheet(s);
    ui->subtitles_color->update();
}

void Settings::onCloseWindow()
{
    closeDialog();
}

void Settings::onButtonApply()
{
    /*===================== Font ==================*/
    *m_pFont = ui->comboBox_font->currentText();
    *m_pSubtitlesFont = ui->comboBox_subtitles_font->currentText();

    const int font_size_index = ui->comboBox_fontsize->currentIndex();
    const int subtitles_font_size_index = ui->comboBox_subtitles_fontsize->currentIndex();
    int arrFontSize[6] = {8, 9, 10, 11, 12, 13};
    *m_pFontSize = arrFontSize[font_size_index];
    *m_pSubtitlesFontSize = arrFontSize[subtitles_font_size_index];

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

    /*============= Multi Instances ==============*/
    int stts_multiInst = ui->checkBox_allowDuplicates->checkState();
    *m_pMultiInstances = (stts_multiInst == 2) ? true : false;

    /*============= Background for hard-burn subtitles ==============*/
    int subtitles_background = ui->checkBox_subtitles_background->checkState();
    *m_pSubtitlesBackground = (stts_multiInst == 2) ? true : false;

    *m_pSubtitlesBackgroundColor = m_pSubtitlesBackgroundColor_temp;
    *m_pSubtitlesColor = m_pSubtitlesColor_temp;

    QString s("background: #"
              + QString(m_pSubtitlesColor_temp.red() < 16? "0" : "") + QString::number(m_pSubtitlesColor_temp.red(),16)
              + QString(m_pSubtitlesColor_temp.green() < 16? "0" : "") + QString::number(m_pSubtitlesColor_temp.green(),16)
              + QString(m_pSubtitlesColor_temp.blue() < 16? "0" : "") + QString::number(m_pSubtitlesColor_temp.blue(),16)
              + QString(m_pSubtitlesColor_temp.alpha() < 16? "0" : "") + QString::number(m_pSubtitlesColor_temp.alpha(),16)
              + ";");
    ui->subtitles_color->setStyleSheet(s);
    ui->subtitles_color->update();

    QString sb("background: #"
              + QString(m_pSubtitlesBackgroundColor_temp.red() < 16? "0" : "") + QString::number(m_pSubtitlesBackgroundColor_temp.red(),16)
              + QString(m_pSubtitlesBackgroundColor_temp.green() < 16? "0" : "") + QString::number(m_pSubtitlesBackgroundColor_temp.green(),16)
              + QString(m_pSubtitlesBackgroundColor_temp.blue() < 16? "0" : "") + QString::number(m_pSubtitlesBackgroundColor_temp.blue(),16)
              + QString(m_pSubtitlesBackgroundColor_temp.alpha() < 16? "0" : "") + QString::number(m_pSubtitlesBackgroundColor_temp.alpha(),16)
              + ";");
    ui->subtitles_background_color->setStyleSheet(sb);
    ui->subtitles_background_color->update();

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
    ui->checkBox_allowDuplicates->setChecked(false);
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
    ui->comboBox_subtitles_fontsize->setCurrentIndex(5);
    QFont subtitles_font;
    QString appSubtitlesFontFamily = subtitles_font.defaultFamily();
    int subtitlesFontInd = ui->comboBox_subtitles_font->findText(appSubtitlesFontFamily);
    if (subtitlesFontInd != -1) {
        ui->comboBox_subtitles_font->setCurrentIndex(subtitlesFontInd);
    }
    QString s("background: #" + DEFAULTSUBTITLECOLOR);
    ui->subtitles_color->setStyleSheet(s);
    ui->subtitles_color->update();
    QString sb("background: #" + DEFAULTSUBTITLEBACKGROUNDCOLOR);
    ui->subtitles_background_color->setStyleSheet(sb);
    ui->subtitles_background_color->update();

}

void Settings::showEvent(QShowEvent *event)
{
    BaseWindow::showEvent(event);
    if (!m_windowActivated) {
        m_windowActivated = true;
        setMinimumSize(QSize(589, 550) * Helper::scaling());
        SETTINGS(stn);
        if (stn.childGroups().contains("SettingsWidget")) {
            stn.beginGroup("SettingsWidget");
            restoreGeometry(stn.value("SettingsWidget/geometry", geometry()).toByteArray());
            stn.endGroup();
        } else {
            resize(QSize(589, 654) * Helper::scaling());
        }
        QSizeF size(this->size());
        QPoint center = QPointF(size.width()/2, size.height()/2).toPoint();
        move(parentWidget()->geometry().center() - center);
        setStyleSheet(Helper::getCss(*m_pTheme));
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

void Settings::onComboBoxSubtitlesFont_indexChanged(int index)
{
    QFont font;
    const QString family = ui->comboBox_subtitles_font->itemText(index);
    font.setFamily(family);
    ui->comboBox_subtitles_font->setFont(font);
}

void Settings::subtitles_color_change()
{
    QColorDialog cdialog;
    QColor t(255, 255, 255, 255);
    cdialog.setOption(QColorDialog::ShowAlphaChannel, true);

    m_pSubtitlesColor_temp = cdialog.getColor(t);

    QString s("background: #"
              + QString(m_pSubtitlesColor_temp.red() < 16? "0" : "") + QString::number(m_pSubtitlesColor_temp.red(),16)
              + QString(m_pSubtitlesColor_temp.green() < 16? "0" : "") + QString::number(m_pSubtitlesColor_temp.green(),16)
              + QString(m_pSubtitlesColor_temp.blue() < 16? "0" : "") + QString::number(m_pSubtitlesColor_temp.blue(),16) + ";");
    ui->subtitles_color->setStyleSheet(s);
    ui->subtitles_color->update();
}

void Settings::subtitles_background_color_change()
{
    QColorDialog cdialog;
    cdialog.setOptions(QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);

    QColor t(255, 255, 255, 255);
    cdialog.setCurrentColor(t);
    m_pSubtitlesBackgroundColor_temp = cdialog.getColor(t);

    QString s("background: #"
              + QString(m_pSubtitlesBackgroundColor_temp.red() < 16? "0" : "") + QString::number(m_pSubtitlesBackgroundColor_temp.red(),16)
              + QString(m_pSubtitlesBackgroundColor_temp.green() < 16? "0" : "") + QString::number(m_pSubtitlesBackgroundColor_temp.green(),16)
              + QString(m_pSubtitlesBackgroundColor_temp.blue() < 16? "0" : "") + QString::number(m_pSubtitlesBackgroundColor_temp.blue(),16)
              + QString(m_pSubtitlesBackgroundColor_temp.alpha() < 16? "0" : "") + QString::number(m_pSubtitlesBackgroundColor_temp.alpha(),16)
              + ";");
    ui->subtitles_background_color->setStyleSheet(s);
    ui->subtitles_background_color->update();
}
