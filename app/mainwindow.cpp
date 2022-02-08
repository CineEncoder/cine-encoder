/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: mainwindow.cpp
 MODIFIED: January, 2022
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "donate.h"
#include "settings.h"
#include "preset.h"
#include "taskcomplete.h"
#include "dialog.h"


#if defined (Q_OS_UNIX)
    #ifndef UNICODE
        #define UNICODE
    #endif
    #include <unistd.h>
    #include <MediaInfo/MediaInfo.h>
    using namespace MediaInfoLib;
#elif defined(Q_OS_WIN64)
    #ifdef __MINGW64__
        #ifdef _UNICODE
            #define _itot _itow
        #else
            #define _itot itoa
        #endif
    #endif
    #include <windows.h>
    #include "MediaInfoDLL/MediaInfoDLL.h"
    using namespace MediaInfoDLL;
#endif



Widget::Widget(QWidget *parent):
    QWidget(parent),
    ui(new Ui::Widget),
    _windowActivated(false),
    _expandWindowsState(false),
    _clickPressedFlag(false),
    _clickPressedToResizeFlag(8, false)
{
    ui->setupUi(this);
#ifdef Q_OS_WIN64
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowSystemMenuHint |
                         Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint);
#else
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#endif
    this->setMouseTracking(true);
    this->setAcceptDrops(true);

    // **************************** Set front label ***********************************//

    QHBoxLayout *raiseLayout = new QHBoxLayout(ui->tableWidget);
    ui->tableWidget->setLayout(raiseLayout);
    raiseThumb = new QLabel(ui->tableWidget);
    raiseLayout->addWidget(raiseThumb);
    raiseThumb->setAlignment(Qt::AlignCenter);
    raiseThumb->setText(tr("No media"));
    raiseThumb->setStyleSheet("color: #09161E; font: 64pt; font-style: oblique;");

    audioThumb = new QLabel(ui->frameTab_2);
    ui->gridLayoutAudio->addWidget(audioThumb);
    audioThumb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    audioThumb->setAlignment(Qt::AlignCenter);
    audioThumb->setText(tr("No audio"));
    audioThumb->setStyleSheet("color: #09161E; font: 24pt; font-style: oblique;");

    subtitleThumb = new QLabel(ui->frameTab_3);
    ui->gridLayoutSubtitle->addWidget(subtitleThumb);
    subtitleThumb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    subtitleThumb->setAlignment(Qt::AlignCenter);
    subtitleThumb->setText(tr("No subtitle"));
    subtitleThumb->setStyleSheet("color: #09161E; font: 24pt; font-style: oblique;");

    // **************************** Create docks ***********************************//

    QGridLayout *layout = new QGridLayout(ui->frame_middle);
    ui->frame_middle->setLayout(layout);
    window = new QMainWindow(ui->frame_middle);
    layout->addWidget(window);
    layout->setContentsMargins(6, 2, 6, 2);
    layout->setVerticalSpacing(0);
    layout->setHorizontalSpacing(0);
    window->setObjectName("CentralWindow");
    window->setWindowFlags(Qt::Widget);
    window->setDockNestingEnabled(true);

    centralWidget = new QWidget(window);
    centralWidget->setObjectName("centralwidget");
    window->setCentralWidget(centralWidget);

    //window->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    //window->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    //window->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    //window->setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    QGridLayout *centralwidgetLayout = new QGridLayout(centralWidget);
    centralWidget->setLayout(centralwidgetLayout);
    centralwidgetLayout->addWidget(ui->frame_task);
    centralwidgetLayout->setContentsMargins(0, 0, 0, 0);

    QList<QString> dockNames = {tr("Presets"), tr("Preview"), tr("Source"), tr("Output"),
                                tr("Streams"), tr("Log"), tr("Metadata"), tr("Split")};
    QList<Qt::DockWidgetArea> dockArea = {Qt::LeftDockWidgetArea, Qt::BottomDockWidgetArea,
                                          Qt::BottomDockWidgetArea, Qt::BottomDockWidgetArea,
                                          Qt::RightDockWidgetArea, Qt::RightDockWidgetArea,
                                          Qt::RightDockWidgetArea, Qt::RightDockWidgetArea};
    QList<QString> objNames = {"Dock_presets", "Dock_preview", "Dock_source", "Dock_output",
                               "Dock_options", "Dock_log", "Dock_metadata", "Dock_split"};
    QList<QFrame*> dockFrames= {ui->frameLeft, ui->frame_preview, ui->frame_source, ui->frame_output,
                                ui->frameRight, ui->frameLog, ui->frameMetadata, ui->frameSplit};
    QWidget *arrWidgets[DOCKS_COUNT];
    QGridLayout *arrLayout[DOCKS_COUNT];
    for (int ind = 0; ind < DOCKS_COUNT; ind++) {
        docks[ind] = new QDockWidget(dockNames.at(ind), window);
        docks[ind]->setObjectName(objNames[ind]);
        docks[ind]->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
        docks[ind]->setFeatures(QDockWidget::AllDockWidgetFeatures);
        arrWidgets[ind] = new QWidget(docks[ind]);
        arrWidgets[ind]->setObjectName(QString("dockWidgetContents_") + QString::number(ind));
        docks[ind]->setWidget(arrWidgets[ind]);
        window->addDockWidget(dockArea[ind], docks[ind]);
        arrLayout[ind] = new QGridLayout(arrWidgets[ind]);
        arrWidgets[ind]->setLayout(arrLayout[ind]);
        arrLayout[ind]->addWidget(dockFrames[ind]);
        arrLayout[ind]->setContentsMargins(0, 0, 0, 0);
        arrLayout[ind]->setHorizontalSpacing(0);
        arrLayout[ind]->setVerticalSpacing(0);
    }

    // **************************** Set Event Filters ***********************************//

    ui->centralwidget->installEventFilter(this);
    ui->centralwidget->setAttribute(Qt::WA_Hover, true);
    ui->frame_main->installEventFilter(this);
    ui->frame_main->setAttribute(Qt::WA_Hover, true);
    ui->frame_main->setAttribute(Qt::WA_NoMousePropagation, true);
    ui->frame_top->installEventFilter(this);
    raiseThumb->installEventFilter(this);
    ui->labelThumb->installEventFilter(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (!_windowActivated) {
        _windowActivated = true;
        std::cout << "Window Activated ..." << std::endl;
        setParameters();
    }
}

void Widget::closeEvent(QCloseEvent *event) // Show prompt when close app
{
    event->ignore();
    if (call_dialog(tr("Quit program?"))) {

        if (encoder->getEncodingState() != QProcess::NotRunning) encoder->killEncoding();
        if (processThumbCreation->state() != QProcess::NotRunning) processThumbCreation->kill();

        QFile _prs_file(_preset_file);
        if (_prs_file.open(QIODevice::WriteOnly)) {
            QDataStream out(&_prs_file);
            out.setVersion(QDataStream::Qt_4_0);
            out << PRESETS_VERSION;
            out << _cur_param << _pos_top << _pos_cld << _preset_table;
            _prs_file.close();
        }

        // Save Version
        _settings->setValue("Version", SETTINGS_VERSION);

        // Save Main Widget
        _settings->beginGroup("MainWidget");
        _settings->setValue("MainWidget/geometry", this->saveGeometry());
        _settings->endGroup();

        // Save Main Window
        _settings->beginGroup("MainWindow");
        _settings->setValue("MainWindow/state", window->saveState());
        _settings->setValue("MainWindow/geometry", window->saveGeometry());
        _settings->beginWriteArray("MainWindow/docks_geometry");
            for (int ind = 0; ind < DOCKS_COUNT; ind++) {
                _settings->setArrayIndex(ind);
                _settings->setValue("MainWindow/docks_geometry/dock_size", docks[ind]->size());
            }
            _settings->endArray();
        _settings->endGroup();

        // Save Tables
        _settings->beginGroup("Tables");
        _settings->setValue("Tables/table_widget_state", ui->tableWidget->horizontalHeader()->saveState());
        _settings->setValue("Tables/tree_widget_state", ui->treeWidget->header()->saveState());
        _settings->endGroup();

        // Save Settings Widget
        _settings->beginGroup("SettingsWidget");
        _settings->setValue("SettingsWidget/geometry", _settingsWindowGeometry);
        _settings->endGroup();

        // Save Preset Widget
        _settings->beginGroup("PresetWidget");
        _settings->setValue("PresetWidget/geometry", _presetWindowGeometry);
        _settings->endGroup();

        // Save Settings
        _settings->beginGroup("Settings");
        _settings->setValue("Settings/prefix_type", _prefxType);
        _settings->setValue("Settings/suffix_type", _suffixType);
        _settings->setValue("Settings/prefix_name", _prefixName);
        _settings->setValue("Settings/suffix_name", _suffixName);
        _settings->setValue("Settings/timer_interval", _timer_interval);
        _settings->setValue("Settings/theme", _theme);
        _settings->setValue("Settings/protection", _protection);
        _settings->setValue("Settings/show_hdr_mode", _showHDR_mode);
        _settings->setValue("Settings/temp_folder", _temp_folder);
        _settings->setValue("Settings/output_folder", _output_folder);
        _settings->setValue("Settings/open_dir", _openDir);
        _settings->setValue("Settings/batch_mode", _batch_mode);
        _settings->setValue("Settings/tray", _hideInTrayFlag);
        _settings->setValue("Settings/language", _language);
        _settings->setValue("Settings/font", _font);
        _settings->setValue("Settings/font_size", _fontSize);
        _settings->setValue("Settings/enviroment", _desktopEnv);
        _settings->setValue("Settings/row_size", _rowSize);
        _settings->endGroup();

        if (_hideInTrayFlag) {
            trayIcon->hide();
        }
        event->accept();
    }
}

void Widget::paintEvent(QPaintEvent *event) // Disable QTab draw base
{
    if (event->type() == QEvent::Paint) {
        QList<QTabBar*> tabBars = window->findChildren<QTabBar*>();
        foreach (QTabBar *tabBar, tabBars) {
            if (tabBar->drawBase()) {
                 tabBar->setDrawBase(false);
            }
        }
    }
}

void Widget::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            if (_expandWindowsState) {
                this->showMaximized();
            }
            else {
                this->showNormal();
            }
            break;
        default:
            break;
    }
}

void Widget::setTrayIconActions()
{
    minimizeAction = new QAction(tr("Hide"), this);
    restoreAction = new QAction(tr("Show"), this);
    quitAction = new QAction(tr("Exit"), this);

    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect(restoreAction, &QAction::triggered, this, [this](){
        if (_expandWindowsState) {
            this->showMaximized();
        }
        else {
            this->showNormal();
        }
    });
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(quitAction);
}

void Widget::showTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(":/resources/icons/64x64/cine-encoder.png"));
    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
}

void Widget::desktopEnvDetection()
{
#if defined (Q_OS_UNIX)
    QProcess process_env;
    process_env.setProcessChannelMode(QProcess::MergedChannels);
    QStringList arguments;
    arguments << "XDG_CURRENT_DESKTOP";
    process_env.start("printenv", arguments);
    if (process_env.waitForFinished()) {
        QString line = process_env.readAllStandardOutput();
        if (line.indexOf("GNOME") != -1) {
            _desktopEnv = "gnome";
        } else {
            _desktopEnv = "other";
        }
    } else {
        std::cout << "printenv not found" << std::endl;
        _desktopEnv = "other";
    }
#else
    _desktopEnv = "other";
#endif
}

void Widget::createConnections()
{
    encoder = new Encoder(this);
    connect(encoder, &Encoder::onEncodingMode, this, &Widget::onEncodingMode);
    connect(encoder, &Encoder::onEncodingStarted, this, &Widget::onEncodingStarted);
    connect(encoder, &Encoder::onEncodingInitError, this, &Widget::onEncodingInitError);
    connect(encoder, &Encoder::onEncodingProgress, this, &Widget::onEncodingProgress);
    connect(encoder, &Encoder::onEncodingLog, this, &Widget::onEncodingLog);
    connect(encoder, &Encoder::onEncodingCompleted, this, &Widget::onEncodingCompleted);
    connect(encoder, &Encoder::onEncodingAborted, this, &Widget::onEncodingAborted);
    connect(encoder, &Encoder::onEncodingError, this, &Widget::onEncodingError);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(repeatHandler_Type_1()));
    timerCallSetThumbnail = new QTimer(this);
    timerCallSetThumbnail->setSingleShot(true);
    timerCallSetThumbnail->setInterval(800);
    connect(timerCallSetThumbnail, SIGNAL(timeout()), this, SLOT(repeatHandler_Type_2()));

    // ***************************** Top menu actions ***********************************//

    add_files = new QAction(tr("Add files"), this);
    remove_files = new QAction(tr("Remove from the list"), this);
    close_prog = new QAction(tr("Close"), this);
    connect(add_files, &QAction::triggered, this, &Widget::on_actionAdd_clicked);
    connect(remove_files, &QAction::triggered, this, &Widget::on_actionRemove_clicked);
    connect(close_prog, &QAction::triggered, this, &Widget::on_closeWindow_clicked);

    encode_files = new QAction(tr("Encode/Pause"), this);
    stop_encode = new QAction(tr("Stop"), this);
    connect(encode_files, &QAction::triggered, this, &Widget::on_actionEncode_clicked);
    connect(stop_encode, &QAction::triggered, this, &Widget::on_actionStop_clicked);

    edit_metadata = new QAction(tr("Edit metadata"), this);
    select_audio = new QAction(tr("Select audio streams"), this);
    select_subtitles = new QAction(tr("Select subtitles"), this);
    split_video = new QAction(tr("Split video"), this);
    connect(edit_metadata, &QAction::triggered, this, &Widget::showMetadataEditor);
    connect(select_audio, &QAction::triggered, this, &Widget::showAudioStreamsSelection);
    connect(select_subtitles, &QAction::triggered, this, &Widget::showSubtitlesSelection);
    connect(split_video, &QAction::triggered, this, &Widget::showVideoSplitter);

    settings = new QAction(tr("Settings"), this);
    connect(settings, &QAction::triggered, this, &Widget::on_actionSettings_clicked);

    reset_view = new QAction(tr("Reset state"), this);
    connect(reset_view, &QAction::triggered, this, &Widget::resetView);

    about = new QAction(tr("About"), this);
    donate = new QAction(tr("Donate"), this);
    connect(about, &QAction::triggered, this, &Widget::on_actionAbout_clicked);
    connect(donate, &QAction::triggered, this, &Widget::on_actionDonate_clicked);

    menuFiles = new QMenu(this);
    menuFiles->addAction(add_files);
    menuFiles->addAction(remove_files);
    menuFiles->addSeparator();
    menuFiles->addAction(close_prog);
    ui->menuFileButton->setMenu(menuFiles);

    menuEdit = new QMenu(this);
    menuEdit->addAction(encode_files);
    menuEdit->addAction(stop_encode);
    ui->menuEditButton->setMenu(menuEdit);

    menuTools = new QMenu(this);
    menuTools->addAction(edit_metadata);
    menuTools->addSeparator();
    menuTools->addAction(select_audio);
    menuTools->addAction(select_subtitles);
    menuTools->addSeparator();
    menuTools->addAction(split_video);
    ui->menuToolsButton->setMenu(menuTools);

    menuView = new QMenu(this);
    for (int i = 0; i < DOCKS_COUNT; i++) {
        menuView->addAction(docks[i]->toggleViewAction());
    }
    menuView->addAction(reset_view);
    ui->menuViewButton->setMenu(menuView);
    docks[DockIndex::LOG_DOCK]->toggleViewAction()->setChecked(false);
    docks[DockIndex::SPLIT_DOCK]->toggleViewAction()->setChecked(false);
    docks[DockIndex::LOG_DOCK]->setVisible(false);
    docks[DockIndex::SPLIT_DOCK]->setVisible(false);

    menuPreferences = new QMenu(this);
    menuPreferences->addAction(settings);
    ui->menuPreferencesButton->setMenu(menuPreferences);

    menuAbout = new QMenu(this);
    menuAbout->addAction(about);
    menuAbout->addSeparator();
    menuAbout->addAction(donate);
    ui->menuAboutButton->setMenu(menuAbout);

    // ***************************** Table menu actions ***********************************//

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    itemMenu = new QMenu(this);
    itemMenu->addAction(remove_files);
    itemMenu->addSeparator();
    itemMenu->addAction(encode_files);
    itemMenu->addSeparator();
    itemMenu->addAction(edit_metadata);
    itemMenu->addSeparator();
    itemMenu->addAction(select_audio);
    itemMenu->addAction(select_subtitles);
    itemMenu->addSeparator();
    itemMenu->addAction(split_video);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &Widget::provideContextMenu);

    // ***************************** Tree menu actions ***********************************//

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QAction* add_section = new QAction(tr("Add section"), this);
    QAction* add_preset = new QAction(tr("Add preset"), this);
    QAction* rename_section_preset = new QAction(tr("Rename"), this);
    QAction* remove_section_preset = new QAction(tr("Remove"), this);
    connect(add_section, &QAction::triggered, this, &Widget::add_section);
    connect(add_preset, &QAction::triggered, this, &Widget::add_preset);
    connect(rename_section_preset, &QAction::triggered, this, &Widget::renameSectionPreset);
    connect(remove_section_preset, &QAction::triggered, this, &Widget::on_actionRemove_preset_clicked);

    QAction* apply_preset = new QAction(tr("Apply"), this);
    QAction* edit_preset = new QAction(tr("Edit"), this);
    connect(apply_preset, &QAction::triggered, this, &Widget::on_buttonApplyPreset_clicked);
    connect(edit_preset, &QAction::triggered, this, &Widget::on_actionEdit_preset_clicked);

    sectionMenu = new QMenu(this);
    sectionMenu->addAction(add_section);
    sectionMenu->addAction(add_preset);
    sectionMenu->addAction(rename_section_preset);
    sectionMenu->addAction(remove_section_preset);

    presetMenu = new QMenu(this);
    presetMenu->addAction(add_section);
    presetMenu->addAction(add_preset);
    presetMenu->addAction(rename_section_preset);
    presetMenu->addAction(remove_section_preset);
    presetMenu->addSeparator();
    presetMenu->addAction(apply_preset);
    presetMenu->addAction(edit_preset);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &Widget::providePresetContextMenu);

    // ***************************** Preset menu actions ***********************************//

    addsection = new QAction(tr("Add section"), this);
    addpreset = new QAction(tr("Add new preset"), this);
    addsection->setIcon(QIcon(":/resources/icons/16x16/cil-folder.png"));
    addpreset->setIcon(QIcon(":/resources/icons/16x16/cil-file.png"));
    connect(addsection, &QAction::triggered, this, &Widget::add_section);
    connect(addpreset, &QAction::triggered, this, &Widget::add_preset);

    menu = new QMenu(this);
    menu->addAction(addsection);
    menu->addSeparator();
    menu->addAction(addpreset);
    ui->actionAdd_preset->setMenu(menu);

    // ************************ Metadata Elements Actions ****************************//

    QList<QLineEdit *> linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit *>();
    foreach (QLineEdit *lineEdit, linesEditMetadata) {
        QList<QAction*> actionList = lineEdit->findChildren<QAction*>();
        if (!actionList.isEmpty()) {
            connect(actionList.first(), &QAction::triggered, this, [this, lineEdit]() {
            lineEdit->clear();
            lineEdit->insert("");
            lineEdit->setModified(true);
            ui->frame_middle->setFocus();
            });
        }
    }

    // ************************ Audio Elements Actions ****************************//
    QList<QLabel*> labelsAudio = ui->frameTab_2->findChildren<QLabel*>();
    foreach (QLabel *label, labelsAudio) {
        label->setVisible(false);
    }
    audioThumb->setVisible(true);

    for (int stream = 0; stream < AMOUNT_AUDIO_STREAMS; stream++) {
        // Check Boxes
        QCheckBox *checkBoxAudio = ui->frameTab_2->findChild<QCheckBox *>("checkBoxAudio_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        connect(checkBoxAudio, &QCheckBox::clicked, this, [this, checkBoxAudio, stream](){
            if (_row != -1) {
                QString state_qstr = "0";
                int state = checkBoxAudio->checkState();
                if (state == 2) {
                    state_qstr = "1";
                }
                QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
                ui->tableWidget->setItem(_row, ColumnIndex::T_AUDIOCHECK_1 + stream, newItem_checkstate);
                _audioStreamCheckState[stream] = state_qstr.toInt();
            }
        });
        checkBoxAudio->setEnabled(true);
        checkBoxAudio->setVisible(false);
        // Line Edit Lang
        QLineEdit *lineEditLangAudio = ui->frameTab_2->findChild<QLineEdit *>("lineEditLangAudio_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        connect(lineEditLangAudio, &QLineEdit::editingFinished, this, [this, lineEditLangAudio, stream](){
            if (_row != -1) {
                if (!lineEditLangAudio->isModified()) {
                    return;
                }
                lineEditLangAudio->setModified(false);
                QString langAudio = lineEditLangAudio->text();
                QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
                ui->tableWidget->setItem(_row, ColumnIndex::T_AUDIOLANG_1 + stream, newItem_langAudio);
                _audioLang[stream] = langAudio;
            }
        });
        QList<QAction*> actionLangAudioList = lineEditLangAudio->findChildren<QAction*>();
        if (!actionLangAudioList.isEmpty()) {
            connect(actionLangAudioList.first(), &QAction::triggered, this, [this, lineEditLangAudio]() {
            lineEditLangAudio->clear();
            lineEditLangAudio->insert("");
            lineEditLangAudio->setModified(true);
            ui->frame_middle->setFocus();
            });
        }
        lineEditLangAudio->setEnabled(true);
        lineEditLangAudio->setVisible(false);
        // Line Edit Title
        QLineEdit *lineEditTitleAudio = ui->frameTab_2->findChild<QLineEdit *>("lineEditTitleAudio_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        connect(lineEditTitleAudio, &QLineEdit::editingFinished, this, [this, lineEditTitleAudio, stream](){
            if (_row != -1) {
                if (!lineEditTitleAudio->isModified()) {
                    return;
                }
                lineEditTitleAudio->setModified(false);
                QString titleAudio = lineEditTitleAudio->text();
                QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
                ui->tableWidget->setItem(_row, ColumnIndex::T_AUDIOTITLE_1 + stream, newItem_titleAudio);
                _audioTitle[stream] = titleAudio;
            }
        });
        QList<QAction*> actionTitleAudioList = lineEditTitleAudio->findChildren<QAction*>();
        if (!actionTitleAudioList.isEmpty()) {
            connect(actionTitleAudioList.first(), &QAction::triggered, this, [this, lineEditTitleAudio]() {
            lineEditTitleAudio->clear();
            lineEditTitleAudio->insert("");
            lineEditTitleAudio->setModified(true);
            ui->frame_middle->setFocus();
            });
        }
        lineEditTitleAudio->setEnabled(true);
        lineEditTitleAudio->setVisible(false);
    }

    // ************************ Subtitle Elements Actions ****************************//
    QList<QLabel*> labelsSubtitle = ui->frameTab_3->findChildren<QLabel*>();
    foreach (QLabel *label, labelsSubtitle) {
        label->setVisible(false);
    }
    subtitleThumb->setVisible(true);

    for (int stream = 0; stream < AMOUNT_SUBTITLES; stream++) {
        // Check Boxes
        QCheckBox *checkBoxSubtitle = ui->frameTab_3->findChild<QCheckBox *>("checkBoxSubtitle_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        connect(checkBoxSubtitle, &QCheckBox::clicked, this, [this, checkBoxSubtitle, stream](){
            if (_row != -1) {
                QString state_qstr = "0";
                int state = checkBoxSubtitle->checkState();
                if (state == 2) {
                    state_qstr = "1";
                }
                QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
                ui->tableWidget->setItem(_row, ColumnIndex::T_SUBCHECK_1 + stream, newItem_checkstate);
                _subtitleCheckState[stream] = state_qstr.toInt();
            }
        });
        checkBoxSubtitle->setEnabled(true);
        checkBoxSubtitle->setVisible(false);
        // Line Edit Lang
        QLineEdit *lineEditLangSubtitle = ui->frameTab_3->findChild<QLineEdit *>("lineEditLangSubtitle_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        connect(lineEditLangSubtitle, &QLineEdit::editingFinished, this, [this, lineEditLangSubtitle, stream](){
            if (_row != -1) {
                if (!lineEditLangSubtitle->isModified()) {
                    return;
                }
                lineEditLangSubtitle->setModified(false);
                QString langSubtitle = lineEditLangSubtitle->text();
                QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
                ui->tableWidget->setItem(_row, ColumnIndex::T_SUBLANG_1 + stream, newItem_langSubtitle);
                _subtitleLang[stream] = langSubtitle;
            }
        });
        QList<QAction*> actionLangSubtitleList = lineEditLangSubtitle->findChildren<QAction*>();
        if (!actionLangSubtitleList.isEmpty()) {
            connect(actionLangSubtitleList.first(), &QAction::triggered, this, [this, lineEditLangSubtitle]() {
            lineEditLangSubtitle->clear();
            lineEditLangSubtitle->insert("");
            lineEditLangSubtitle->setModified(true);
            ui->frame_middle->setFocus();
            });
        }
        lineEditLangSubtitle->setEnabled(true);
        lineEditLangSubtitle->setVisible(false);
        // Line Edit Title
        QLineEdit *lineEditTitleSubtitle = ui->frameTab_3->findChild<QLineEdit *>("lineEditTitleSubtitle_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        connect(lineEditTitleSubtitle, &QLineEdit::editingFinished, this, [this, lineEditTitleSubtitle, stream](){
            if (_row != -1) {
                if (!lineEditTitleSubtitle->isModified()) {
                    return;
                }
                lineEditTitleSubtitle->setModified(false);
                QString titleSubtitle = lineEditTitleSubtitle->text();
                QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
                ui->tableWidget->setItem(_row, ColumnIndex::T_TITLESUB_1 + stream, newItem_titleSubtitle);
                _subtitleTitle[stream] = titleSubtitle;
            }
        });
        QList<QAction*> actionTitleSubtitleList = lineEditTitleSubtitle->findChildren<QAction*>();
        if (!actionTitleSubtitleList.isEmpty()) {
            connect(actionTitleSubtitleList.first(), &QAction::triggered, this, [this, lineEditTitleSubtitle]() {
            lineEditTitleSubtitle->clear();
            lineEditTitleSubtitle->insert("");
            lineEditTitleSubtitle->setModified(true);
            ui->frame_middle->setFocus();
            });
        }
        lineEditTitleSubtitle->setEnabled(true);
        lineEditTitleSubtitle->setVisible(false);
    }
}

void Widget::setParameters()    /*** Set parameters ***/
{
    // ***************************** Set parameters ***********************************//
    createConnections();
    openingFiles.setParent(this);
    openingFiles.setModal(true);
    trayIcon = new QSystemTrayIcon(this);
    _new_param.resize(PARAMETERS_COUNT);
    _cur_param.resize(PARAMETERS_COUNT);
    _preset_table.resize(PARAMETERS_COUNT+1);
    for (int i = 0; i < PARAMETERS_COUNT+1; i++) {
      _preset_table[i].resize(5);
    }

    // ****************************** Initialize variables ************************************//
    QDesktopWidget *screenSize = QApplication::desktop();
    int screenWidth = screenSize->width();
    int screenHeight = screenSize->height();
    int widthMainWindow = 1024;
    int heightMainWindow = 650;
    int x_pos = static_cast<int>(round(static_cast<float>(screenWidth - widthMainWindow)/2));
    int y_pos = static_cast<int>(round(static_cast<float>(screenHeight - heightMainWindow)/2));
    _desktopEnv = "default";
    _settingsWindowGeometry = "default";
    _presetWindowGeometry = "default";
    _openDir = QDir::homePath();
    _settings_path = QDir::homePath() + QString("/CineEncoder");
    _thumb_path = _settings_path + QString("/thumbnails");
    _preset_file = _settings_path + QString("/presets.ini");
    _settings = new QSettings(_settings_path + QString("/settings.ini"), QSettings::IniFormat, this);
    _status_encode_btn = EncodingStatus::START;
    _timer_interval = 30;
    _curTime = 0;
    _language = "";
    _font = "";
    _fontSize = 8;
    _curFilename = "";
    _curPath = "";
    _input_file = "";
    _output_folder = "";
    _output_file = "";
    _temp_folder = "";
    _temp_file = "";
    _prefixName = "output";
    _suffixName = "_encoded_";
    _prefxType = 0;
    _suffixType = 0;
    _pos_top = -1;
    _pos_cld = -1;
    _rowSize = 25;
    _hideInTrayFlag = false;
    _protection = false;
    _batch_mode = false;
    _showHDR_mode = false;
    _row = -1;
    _theme = 3;
    QListView *comboboxPresetListView = new QListView(ui->comboBoxPreset);
    QListView *comboboxModeListView = new QListView(ui->comboBoxMode);
    QListView *comboboxViewListView = new QListView(ui->comboBoxView);
    ui->comboBoxPreset->setView(comboboxPresetListView);
    ui->comboBoxMode->setView(comboboxModeListView);
    ui->comboBoxView->setView(comboboxViewListView);
    ui->comboBoxPreset->setVisible(false);
    ui->comboBoxView->setVisible(false);
    animation = new QMovie(this);
    animation->setFileName(":/resources/icons/Animated/cil-spinner-circle.gif");
    animation->setScaledSize(QSize(18, 18));
    processThumbCreation = new QProcess(this);

    // ****************************** Setup widgets ************************************//
    ui->labelAnimation->setMovie(animation);
    ui->labelAnimation->hide();
    ui->label_Progress->hide();
    ui->label_Remaining->hide();
    ui->label_RemTime->hide();
    ui->progressBar->hide();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->verticalHeader()->setFixedWidth(0);
    ui->tableWidget->setColumnWidth(ColumnIndex::FILENAME, 250);
    ui->tableWidget->setColumnWidth(ColumnIndex::FORMAT, 80);
    ui->tableWidget->setColumnWidth(ColumnIndex::RESOLUTION, 85);
    ui->tableWidget->setColumnWidth(ColumnIndex::DURATION, 70);
    ui->tableWidget->setColumnWidth(ColumnIndex::FPS, 70);
    ui->tableWidget->setColumnWidth(ColumnIndex::AR, 60);
    ui->tableWidget->setColumnWidth(ColumnIndex::STATUS, 80);
    ui->tableWidget->setIconSize(QSize(16, 16));

    for (int i = ColumnIndex::COLORRANGE; i <= ColumnIndex::MAXFALL; i++) {
        ui->tableWidget->setColumnWidth(i, 82);
    }

    for (int i = ColumnIndex::BITRATE; i <= ColumnIndex::COLORSPACE; i++) {
        ui->tableWidget->hideColumn(i);
    }

    for (int i = ColumnIndex::T_DUR; i <= ColumnIndex::T_ENDTIME; i++) {
        ui->tableWidget->hideColumn(i);
    }

    // ****************************** Create folders ************************************//
    if (!QDir(_settings_path).exists()) {
        QDir().mkdir(_settings_path);
        std::cout << "Setting path not existed and was created ..." << std::endl;  // Debug info //
    }
    if (QDir(_thumb_path).exists()) {
        unsigned int count_thumb = QDir(_thumb_path).count();
        std::cout << "Number of thumbnails: " << count_thumb << std::endl; // Debug info //
        if (count_thumb > 300) {
            QDir(_thumb_path).removeRecursively();
            std::cout << "Thumbnails removed... " << std::endl; // Debug info //
        }
    }
    if (!QDir(_thumb_path).exists()) {
        QDir().mkdir(_thumb_path);
        std::cout << "Thumbnail path not existed and was created ..." << std::endl;  // Debug info //
    }

    // ****************************** Read presets ************************************//
    QFile _prs_file(_preset_file);
    if (_prs_file.open(QIODevice::ReadOnly)) {
        QDataStream in(&_prs_file);
        in.setVersion(QDataStream::Qt_4_0);
        int ver;
        in >> ver;
        if (ver == PRESETS_VERSION) {
            in >> _cur_param >> _pos_top >> _pos_cld >> _preset_table;
            _prs_file.close();
        }
        else {
            _prs_file.close();
            set_defaults();
        }

    } else {
        set_defaults();
    }

    // ************************** Read settings ******************************//
    QList<int> dockSizesX = {};
    QList<int> dockSizesY = {};
    if (_settings->value("Version").toInt() == SETTINGS_VERSION) {
        // Restore Main Widget
        _settings->beginGroup("MainWidget");
        this->restoreGeometry(_settings->value("MainWidget/geometry").toByteArray());
        _settings->endGroup();

        // Restore Main Window
        _settings->beginGroup("MainWindow");
        window->restoreState(_settings->value("MainWindow/state").toByteArray());
        int arraySize = _settings->beginReadArray("MainWindow/docks_geometry");
            for (int i = 0; i < arraySize && i < DOCKS_COUNT; i++) {
                _settings->setArrayIndex(i);
                QSize size = _settings->value("MainWindow/docks_geometry/dock_size").toSize();
                dockSizesX.append(size.width());
                dockSizesY.append(size.height());
            }
            _settings->endArray();
        _settings->endGroup();

        // Restore Tables
        _settings->beginGroup("Tables");
        ui->tableWidget->horizontalHeader()->restoreState(_settings->value("Tables/table_widget_state").toByteArray());
        ui->treeWidget->header()->restoreState(_settings->value("Tables/tree_widget_state").toByteArray());
        _settings->endGroup();

        // Restore Settings Widget
        _settings->beginGroup("SettingsWidget");
        _settingsWindowGeometry = _settings->value("SettingsWidget/geometry").toByteArray();
        _settings->endGroup();

        // Restore Preset Widget
        _settings->beginGroup("PresetWidget");
        _presetWindowGeometry = _settings->value("PresetWidget/geometry").toByteArray();
        _settings->endGroup();

        // Restore Settings
        _settings->beginGroup("Settings");
        _prefxType = _settings->value("Settings/prefix_type").toInt();
        _suffixType = _settings->value("Settings/suffix_type").toInt();
        _prefixName = _settings->value("Settings/prefix_name").toString();
        _suffixName = _settings->value("Settings/suffix_name").toString();
        _timer_interval = _settings->value("Settings/timer_interval").toInt();
        _theme = _settings->value("Settings/theme").toInt();
        _protection = _settings->value("Settings/protection").toBool();
        _showHDR_mode = _settings->value("Settings/show_hdr_mode").toBool();
        _temp_folder = _settings->value("Settings/temp_folder").toString();
        _output_folder = _settings->value("Settings/output_folder").toString();
        _openDir = _settings->value("Settings/open_dir").toString();
        _batch_mode = _settings->value("Settings/batch_mode").toBool();
        _hideInTrayFlag = _settings->value("Settings/tray").toBool();
        _language = _settings->value("Settings/language").toString();
        _font = _settings->value("Settings/font").toString();
        _fontSize = _settings->value("Settings/font_size").toInt();
        _desktopEnv = _settings->value("Settings/enviroment").toString();
        _rowSize = _settings->value("Settings/row_size").toInt();
        _settings->endGroup();

    } else {
        this->setGeometry(x_pos, y_pos, widthMainWindow, heightMainWindow);
    }

    // ***************************** Preset parameters ***********************************//
    ui->treeWidget->clear();
    ui->treeWidget->setHeaderHidden(false);
    ui->treeWidget->setAlternatingRowColors(true);
    ui->treeWidget->setColumnWidth(0, 230);
    ui->treeWidget->setColumnWidth(1, 55);
    ui->treeWidget->setColumnWidth(2, 55);
    ui->treeWidget->setColumnWidth(3, 55);
    ui->treeWidget->setColumnWidth(4, 55);
    ui->treeWidget->setColumnWidth(5, 55);
    ui->treeWidget->setColumnWidth(6, 55);
    int NUM_ROWS = _preset_table[0].size();
    int NUM_COLUMNS = _preset_table.size();
    QString type;
    QFont parentFont;
    parentFont.setBold(true);
    for (int i = 0; i < NUM_ROWS; i++) {
        type = _preset_table[PARAMETERS_COUNT][i];
        if (type == "TopLewelItem") {
            QTreeWidgetItem *root = new QTreeWidgetItem();
            root->setText(0, _preset_table[0][i]);
            root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
            root->setFont(0, parentFont);
            setPresetIcon(root, true);
            ui->treeWidget->addTopLevelItem(root);
            ui->treeWidget->setCurrentItem(root);
            root->setFirstColumnSpanned(true);
        }
        if (type == "ChildItem") {
            QTreeWidgetItem *item = ui->treeWidget->currentItem();
            QTreeWidgetItem *child = new QTreeWidgetItem();
            for (int j = 0; j < PARAMETERS_COUNT; j++) {
                child->setText(j + 7, _preset_table[j][i]);
            }
            QString savedPresetName = child->text(30 + 7);
            child->setText(0, savedPresetName);
            updateInfoFields(_preset_table[1][i],
                             _preset_table[2][i],
                             _preset_table[3][i],
                             _preset_table[4][i],
                             _preset_table[11][i],
                             _preset_table[12][i],
                             _preset_table[21][i],
                             child, false);
            setItemStyle(child);
            item->addChild(child);
        }
    }
    if (_pos_top != -1 && _pos_cld != -1) {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(_pos_top)->child(_pos_cld);
        ui->treeWidget->setCurrentItem(item);
    }
    std::cout << NUM_ROWS << " x " << NUM_COLUMNS << std::endl; // Table size
    for (int i = 7; i <= 40; i++) {
        ui->treeWidget->hideColumn(i);
    }

    // ***************************** Other parameters ***********************************//
    if (dockSizesX.count() < DOCKS_COUNT || dockSizesY.count() < DOCKS_COUNT) {
        float coeffX[DOCKS_COUNT] = {0.25f, 0.04f, 0.48f, 0.48f, 0.25f, 0.25f, 0.25f, 0.25f};
        float coeffY[DOCKS_COUNT] = {0.9f, 0.1f, 0.1f, 0.1f, 0.9f, 0.9f, 0.9f, 0.9f};
        for (int ind = 0; ind < DOCKS_COUNT; ind++) {
            int dockWidth = static_cast<int>(coeffX[ind] * widthMainWindow);
            int dockHeight = static_cast<int>(coeffY[ind] * heightMainWindow);
            dockSizesX.append(dockWidth);
            dockSizesY.append(dockHeight);
        }
    }
    QTimer::singleShot(200, this, [this, dockSizesX, dockSizesY](){
        setDocksParameters(dockSizesX, dockSizesY);
    });

    if (_timer_interval < 15) _timer_interval = 30;
    timer->setInterval(_timer_interval*1000);

    if (_rowSize != 0) ui->horizontalSlider_resize->setValue(_rowSize);

    if (_fontSize == 0) _fontSize = 8;

    if (_language == "") {
        QLocale locale = QLocale::system();
        if (locale.language() == QLocale::Chinese) {
            _language = "zh";
        }
        else if (locale.language() == QLocale::German) {
            _language = "de";
        }
        else if (locale.language() == QLocale::Russian) {
            _language = "ru";
        }
        else {
            _language = "en";
        }
    }

    if (_desktopEnv == "default") {
        std::cout << "Detect desktop env. ..." << std::endl;
        desktopEnvDetection();
    }
    std::cout << "Desktop env.: " << _desktopEnv.toStdString() << std::endl;

    if (this->isMaximized()) _expandWindowsState = true;

    if (_batch_mode) {
        ui->comboBoxMode->blockSignals(true);
        ui->comboBoxMode->setCurrentIndex(1);
        ui->comboBoxMode->blockSignals(false);
    }
    setTrayIconActions();
    showTrayIcon();
    if (_hideInTrayFlag) trayIcon->show();
    setTheme(_theme);
}

void Widget::setDocksParameters(QList<int> dockSizesX, QList<int> dockSizesY)
{
    // ************************** Set Docks Parameters ********************************//
    QList<QDockWidget*> docksVis;
    QList<int> dockVisSizesX;
    QList<int> dockVisSizesY;
    for (int i = 0; i < DOCKS_COUNT; i++) {
        if (docks[i]->isVisible() && !docks[i]->isFloating()){
            docksVis.append(docks[i]);
            dockVisSizesX.append(dockSizesX.at(i));
            dockVisSizesY.append(dockSizesY.at(i));
        }
    }
    window->resizeDocks(docksVis, dockVisSizesX, Qt::Horizontal);
    window->resizeDocks(docksVis, dockVisSizesY, Qt::Vertical);
    for (int i = 0; i < DOCKS_COUNT; i++) {
        if (docks[i]->isVisible() && docks[i]->isFloating()){
            docks[i]->setFloating(false); // Bypassing the error with detached docks in some Linux distributions
            docks[i]->setFloating(true);
        }
    }
}

void Widget::on_closeWindow_clicked()    /*** Close window signal ***/
{
    this->close();
}

void Widget::setExpandIcon()
{
    switch (_theme) {
    case Theme::GRAY:
    case Theme::DARK:
    case Theme::WAVE:
        if (_expandWindowsState) {
            ui->expandWindow->setIcon(QIcon(":/resources/icons/16x16/cil-clone.png"));
        } else {
            ui->expandWindow->setIcon(QIcon(":/resources/icons/16x16/cil-media-stop.png"));}
        break;
    case Theme::DEFAULT:
        if (_expandWindowsState) {
            ui->expandWindow->setIcon(QIcon(":/resources/icons/16x16/cil-clone_black.png"));
        } else {
            ui->expandWindow->setIcon(QIcon(":/resources/icons/16x16/cil-media-stop_black.png"));}
        break;
    }
}

void Widget::on_expandWindow_clicked()    /*** Expand window ***/
{
    if (!this->isMaximized()) {
        _expandWindowsState = true;
        this->showMaximized();
    } else {
        _expandWindowsState = false;
        this->showNormal();
    }
    setExpandIcon();
}

void Widget::on_hideWindow_clicked()    /*** Hide window ***/
{
    if (_hideInTrayFlag) {
        this->hide();
    } else {
        this->showMinimized();
    }
}

void Widget::on_actionAbout_clicked()   /*** About ***/
{
    About about(this);
    about.setModal(true);
    about.setParameters();
    about.exec();
}

void Widget::on_actionDonate_clicked()   /*** Donate ***/
{
    Donate donate(this);
    donate.setModal(true);
    donate.setParameters();
    donate.exec();
}

void Widget::on_actionSettings_clicked()
{
    Settings settings(this);
    settings.setParameters(&_settingsWindowGeometry,
                           &_output_folder,
                           &_temp_folder,
                           &_protection,
                           &_showHDR_mode,
                           &_timer_interval,
                           &_theme,
                           &_prefixName,
                           &_suffixName,
                           &_prefxType,
                           &_suffixType,
                           &_hideInTrayFlag,
                           &_language,
                           _desktopEnv,
                           &_fontSize,
                           &_font);
    settings.setModal(true);
    if (settings.exec() == QDialog::Accepted) {
        timer->setInterval(_timer_interval*1000);
        setTheme(_theme);
        if (_hideInTrayFlag) {
            trayIcon->show();
        } else {
            trayIcon->hide();
        }
        if (_row != -1) {
            get_output_filename();
        }
        _message = tr("You need to restart the program for the settings to take effect.");
        call_task_complete(_message, false);
    }
}

void Widget::showOpeningFiles(bool status)
{
    QPoint position;
    QPoint posMainWindow = this->rect().topLeft();
    QSize sizeMainWindow = this->size();
    int x_pos = posMainWindow.x() + static_cast<int>(round(static_cast<float>(sizeMainWindow.width())/2));
    int y_pos = posMainWindow.y() + static_cast<int>(round(static_cast<float>(sizeMainWindow.height())/2));
    position.setX(x_pos);
    position.setY(y_pos);
    openingFiles.setParameters(status, position);
}

void Widget::showOpeningFiles(QString text)
{
    openingFiles.setText(text);
}

void Widget::showOpeningFiles(int percent)
{
    openingFiles.setPercent(percent);
}

void Widget::showMetadataEditor()
{
    if (!docks[DockIndex::METADATA_DOCK]->isVisible()) {
        docks[DockIndex::METADATA_DOCK]->setVisible(true);
    }
    docks[DockIndex::METADATA_DOCK]->setFloating(true);
}

void Widget::showAudioStreamsSelection()
{
    if (!docks[DockIndex::STREAMS_DOCK]->isVisible()) {
        docks[DockIndex::STREAMS_DOCK]->setVisible(true);
    }
    docks[DockIndex::STREAMS_DOCK]->setFloating(true);
    ui->tabWidgetRight->setCurrentIndex(0);
}

void Widget::showSubtitlesSelection()
{
    if (!docks[DockIndex::STREAMS_DOCK]->isVisible()) {
        docks[DockIndex::STREAMS_DOCK]->setVisible(true);
    }
    docks[DockIndex::STREAMS_DOCK]->setFloating(true);
    ui->tabWidgetRight->setCurrentIndex(1);
}

void Widget::showVideoSplitter()
{
    if (!docks[DockIndex::SPLIT_DOCK]->isVisible()) {
        docks[DockIndex::SPLIT_DOCK]->setVisible(true);
    }
    docks[DockIndex::SPLIT_DOCK]->setFloating(true);
}

void Widget::get_current_data() /*** Get current data ***/
{
    _dur = (ui->tableWidget->item(_row, ColumnIndex::T_DUR)->text()).toDouble();
    _stream_size = ui->tableWidget->item(_row, ColumnIndex::T_STREAMSIZE)->text();
    _width = ui->tableWidget->item(_row, ColumnIndex::T_WIDTH)->text();
    _height = ui->tableWidget->item(_row, ColumnIndex::T_HEIGHT)->text();
    _fmt = ui->tableWidget->item(_row, ColumnIndex::FORMAT)->text();
    _fps = ui->tableWidget->item(_row, ColumnIndex::FPS)->text();

    double fps_double = _fps.toDouble();
    _fr_count = static_cast<int>(round(_dur * fps_double));

    _startTime = (ui->tableWidget->item(_row, ColumnIndex::T_STARTTIME)->text()).toDouble();
    _endTime = (ui->tableWidget->item(_row, ColumnIndex::T_ENDTIME)->text()).toDouble();

    //std::cout << "DURATION SEC COUNT <<<<<<<<<<<<<<<<<< : " << _dur << std::endl;
    //std::cout << "FRAME RATE COUNT <<<<<<<<<<<<<<<<<< : " << _fr_count << std::endl;

    QString curCodec = _fmt;
    QString curFps = _fps;
    QString curRes = ui->tableWidget->item(_row, ColumnIndex::RESOLUTION)->text();
    QString curAr = ui->tableWidget->item(_row, ColumnIndex::AR)->text();
    QString curBitrate = ui->tableWidget->item(_row, ColumnIndex::BITRATE)->text();
    QString curColorSampling = ui->tableWidget->item(_row, ColumnIndex::SUBSAMPLING)->text();
    QString curDepth = ui->tableWidget->item(_row, ColumnIndex::BITDEPTH)->text();
    QString curSpace = ui->tableWidget->item(_row, ColumnIndex::COLORSPACE)->text();

    _curPath = ui->tableWidget->item(_row, ColumnIndex::PATH)->text();
    _curFilename = ui->tableWidget->item(_row, ColumnIndex::FILENAME)->text();
    _input_file = _curPath + QString("/") + _curFilename;
    _hdr[CUR_COLOR_RANGE] = ui->tableWidget->item(_row, ColumnIndex::COLORRANGE)->text();       // color range
    _hdr[CUR_COLOR_PRIMARY] = ui->tableWidget->item(_row, ColumnIndex::COLORPRIM)->text();      // color primary
    _hdr[CUR_COLOR_MATRIX] = ui->tableWidget->item(_row, ColumnIndex::COLORMATRIX)->text();     // color matrix
    _hdr[CUR_TRANSFER] = ui->tableWidget->item(_row, ColumnIndex::TRANSFER)->text();            // transfer
    _hdr[CUR_MAX_LUM] = ui->tableWidget->item(_row, ColumnIndex::MAXLUM)->text();               // max lum
    _hdr[CUR_MIN_LUM] = ui->tableWidget->item(_row, ColumnIndex::MINLUM)->text();               // min lum
    _hdr[CUR_MAX_CLL] = ui->tableWidget->item(_row, ColumnIndex::MAXCLL)->text();               // max cll
    _hdr[CUR_MAX_FALL] = ui->tableWidget->item(_row, ColumnIndex::MAXFALL)->text();             // max fall
    _hdr[CUR_MASTER_DISPLAY] = ui->tableWidget->item(_row, ColumnIndex::MASTERDISPLAY)->text(); // master display
    _hdr[CUR_CHROMA_COORD] = ui->tableWidget->item(_row, ColumnIndex::T_CHROMACOORD)->text();   // chr coord
    _hdr[CUR_WHITE_COORD] = ui->tableWidget->item(_row, ColumnIndex::T_WHITECOORD)->text();     // white coord

    _videoMetadata[VIDEO_TITLE] = ui->tableWidget->item(_row, ColumnIndex::T_VIDEOTITLE)->text();
    _videoMetadata[VIDEO_AUTHOR] = ui->tableWidget->item(_row, ColumnIndex::T_VIDEOAUTHOR)->text();
    _videoMetadata[VIDEO_YEAR] = ui->tableWidget->item(_row, ColumnIndex::T_VIDEOYEAR)->text();
    _videoMetadata[VIDEO_PERFORMER] = ui->tableWidget->item(_row, ColumnIndex::T_VIDEOPERF)->text();
    _videoMetadata[VIDEO_DESCRIPTION] = ui->tableWidget->item(_row, ColumnIndex::T_VIDEODESCR)->text();
    _videoMetadata[VIDEO_MOVIENAME] = ui->tableWidget->item(_row, ColumnIndex::T_VIDEOMOVIENAME)->text();

    for (int p = 0; p < AMOUNT_AUDIO_STREAMS; p++) {
        _audioStreamCheckState[p] = (ui->tableWidget->item(_row, p + ColumnIndex::T_AUDIOCHECK_1)->text()).toInt();
        _audioLang[p] = ui->tableWidget->item(_row, p + ColumnIndex::T_AUDIOLANG_1)->text();
        _audioTitle[p] = ui->tableWidget->item(_row, p + ColumnIndex::T_AUDIOTITLE_1)->text();
    }

    for (int p = 0; p < AMOUNT_SUBTITLES; p++) {
        _subtitleCheckState[p] = (ui->tableWidget->item(_row, p + ColumnIndex::T_SUBCHECK_1)->text()).toInt();
        _subtitleLang[p] = ui->tableWidget->item(_row, p + ColumnIndex::T_SUBLANG_1)->text();
        _subtitleTitle[p] = ui->tableWidget->item(_row, p + ColumnIndex::T_TITLESUB_1)->text();
    }

    //******************************** Set icons *****************************//

    double halfTime = _dur/2;
    QString tmb_file = setThumbnail(_curFilename, halfTime, PreviewRes::RES_HIGH, PreviewDest::PREVIEW);

    QSize imageSize = QSize(85, 48);
    if (_rowSize == 25) {
        QString icons[4][5] = {
            {"cil-hdr",       "cil-camera-roll",       "cil-hd",       "cil-4k",       "cil-file"},
            {"cil-hdr",       "cil-camera-roll",       "cil-hd",       "cil-4k",       "cil-file"},
            {"cil-hdr",       "cil-camera-roll",       "cil-hd",       "cil-4k",       "cil-file"},
            {"cil-hdr_black", "cil-camera-roll_black", "cil-hd_black", "cil-4k_black", "cil-file_black"}
        };

        if (_hdr[CUR_MASTER_DISPLAY] != "") {
            tmb_file = ":/resources/icons/16x16/" + icons[_theme][0] + ".png";
        }
        else if (_height.toInt() >=1 && _height.toInt() < 720) {
            tmb_file = ":/resources/icons/16x16/" + icons[_theme][1] + ".png";
        }
        else if (_height.toInt() >= 720 && _height.toInt() < 2160) {
            tmb_file = ":/resources/icons/16x16/" + icons[_theme][2] + ".png";
        }
        else if (_height.toInt() >= 2160) {
            tmb_file = ":/resources/icons/16x16/" + icons[_theme][3] + ".png";
        }
        else {
            tmb_file = ":/resources/icons/16x16/" + icons[_theme][4] + ".png";
        }
        imageSize = QSize(16, 16);
    }
    QPixmap pxmp(tmb_file);
    QPixmap scaled = pxmp.scaled(imageSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->tableWidget->item(_row, ColumnIndex::FILENAME)->setIcon(QIcon(scaled));

    //******************************** Set description *****************************//

    ui->label_source->setText(_curFilename);

    if (curCodec != "") {
        curCodec += ", ";
    }
    if (curRes != "") {
        curRes += ", ";
    }
    if (curFps != "") {
        curFps += " fps, ";
    }
    if (curAr != "") {
        curAr += ", ";
    }
    if (curSpace != "") {
        curSpace += ", ";
    }
    if (curColorSampling != "") {
        curColorSampling += ", ";
    }
    if (curDepth != "") {
        curDepth += tr(" bit, ");
    }
    if (curBitrate != "") {
        curBitrate += tr(" kbps; ");
    }
    QString sourceParam = curCodec + curRes + curFps + curAr + curSpace + curColorSampling + curDepth + curBitrate;

    int countAudioStreams = 0;
    QString curAudioStream("");
    while (countAudioStreams < AMOUNT_AUDIO_STREAMS) {
        curAudioStream = ui->tableWidget->item(_row, countAudioStreams + ColumnIndex::T_AUDIO_1)->text();
        if (curAudioStream == "") {
            break;
        }
        countAudioStreams++;
        sourceParam += tr("Audio #") + QString::number(countAudioStreams) + QString(": ") + curAudioStream + QString("; ");
    }

    int countSubtitles = 0;
    QString curSubtitle("");
    while (countSubtitles < AMOUNT_SUBTITLES) {
        curSubtitle = ui->tableWidget->item(_row, countSubtitles + ColumnIndex::T_SUBTITLE_1)->text();
        if (curSubtitle == "") {
            break;
        }
        countSubtitles++;
    }


    if (curCodec == "Undef, ") {
        sourceParam = tr("Undefined");
    }
    ui->textBrowser_1->clear();
    ui->textBrowser_1->setText(sourceParam);
    get_output_filename();

    //******************************** Set time widgets *****************************//

    ui->horizontalSlider->setMaximum(_fr_count);
    ui->lineEditStartTime->setText(timeConverter(_startTime));
    ui->lineEditEndTime->setText(timeConverter(_endTime));

    //******************************** Set video widgets *****************************//

    QList<QLineEdit *> linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit *>();
    foreach (QLineEdit *lineEdit, linesEditMetadata) {
        lineEdit->setEnabled(true);
    }

    ui->lineEditTitleVideo->setText(_videoMetadata[VIDEO_TITLE]);
    ui->lineEditAuthorVideo->setText(_videoMetadata[VIDEO_AUTHOR]);
    ui->lineEditYearVideo->setText(_videoMetadata[VIDEO_YEAR]);
    ui->lineEditPerfVideo->setText(_videoMetadata[VIDEO_PERFORMER]);
    ui->lineEditDescriptionVideo->setText(_videoMetadata[VIDEO_DESCRIPTION]);
    ui->lineEditMovieNameVideo->setText(_videoMetadata[VIDEO_MOVIENAME]);

    foreach (QLineEdit *lineEdit, linesEditMetadata) {
        lineEdit->setCursorPosition(0);
    }

    //******************************** Set audio widgets *****************************//
    if (countAudioStreams > 0) {
        audioThumb->setVisible(false);
    }
    for (int q = 0; q < AMOUNT_AUDIO_STREAMS; q++) {
        QCheckBox *checkBoxAudio = ui->frameTab_2->findChild<QCheckBox *>("checkBoxAudio_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        checkBoxAudio->setChecked(bool(_audioStreamCheckState[q]));
        QLineEdit *lineEditLangAudio = ui->frameTab_2->findChild<QLineEdit *>("lineEditLangAudio_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        lineEditLangAudio->setText(_audioLang[q]);
        lineEditLangAudio->setCursorPosition(0);
        QLineEdit *lineEditTitleAudio = ui->frameTab_2->findChild<QLineEdit *>("lineEditTitleAudio_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        lineEditTitleAudio->setText(_audioTitle[q]);
        lineEditTitleAudio->setCursorPosition(0);
        QLabel *labelAudio = ui->frameTab_2->findChild<QLabel *>("labelTitleAudio_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        if (q < countAudioStreams) {
            checkBoxAudio->setVisible(true);
            lineEditLangAudio->setVisible(true);
            lineEditTitleAudio->setVisible(true);
            labelAudio->setVisible(true);
        }
    }

    //******************************** Set subtitle widgets *****************************//
    if (countSubtitles > 0) {
        subtitleThumb->setVisible(false);
    }
    for (int q = 0; q < AMOUNT_SUBTITLES; q++) {
        QCheckBox *checkBoxSubtitle = ui->frameTab_3->findChild<QCheckBox *>("checkBoxSubtitle_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        checkBoxSubtitle->setChecked(bool(_subtitleCheckState[q]));
        QLineEdit *lineEditLangSubtitle = ui->frameTab_3->findChild<QLineEdit *>("lineEditLangSubtitle_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        lineEditLangSubtitle->setText(_subtitleLang[q]);
        lineEditLangSubtitle->setCursorPosition(0);
        QLineEdit *lineEditTitleSubtitle = ui->frameTab_3->findChild<QLineEdit *>("lineEditTitleSubtitle_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        lineEditTitleSubtitle->setText(_subtitleTitle[q]);
        lineEditTitleSubtitle->setCursorPosition(0);
        QLabel *labelSubtitle = ui->frameTab_3->findChild<QLabel *>("labelTitleSub_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        if (q < countSubtitles) {
            checkBoxSubtitle->setVisible(true);
            lineEditLangSubtitle->setVisible(true);
            lineEditTitleSubtitle->setVisible(true);
            labelSubtitle->setVisible(true);
        }
    }
}

void Widget::setTheme(int &ind_theme)   /*** Set theme ***/
{
    QFile file;
    QString list("");
    switch (ind_theme) {
    case Theme::GRAY:
        file.setFileName(":/resources/css/style_0.css");
        raiseThumb->setStyleSheet("color: #09161E; font: 64pt; font-style: oblique;");
        audioThumb->setStyleSheet("color: #09161E; font: 24pt; font-style: oblique;");
        subtitleThumb->setStyleSheet("color: #09161E; font: 24pt; font-style: oblique;");
        break;
    case Theme::DARK:
        file.setFileName(":/resources/css/style_1.css");
        raiseThumb->setStyleSheet("color: #09161E; font: 64pt; font-style: oblique;");
        audioThumb->setStyleSheet("color: #09161E; font: 24pt; font-style: oblique;");
        subtitleThumb->setStyleSheet("color: #09161E; font: 24pt; font-style: oblique;");
        break;
    case Theme::WAVE:
        file.setFileName(":/resources/css/style_2.css");
        raiseThumb->setStyleSheet("color: #09161E; font: 64pt; font-style: oblique;");
        audioThumb->setStyleSheet("color: #09161E; font: 24pt; font-style: oblique;");
        subtitleThumb->setStyleSheet("color: #09161E; font: 24pt; font-style: oblique;");
        break;
    case Theme::DEFAULT:
        file.setFileName(":/resources/css/style_3.css");
        raiseThumb->setStyleSheet("color: #E3E3E3; font: 64pt; font-style: oblique;");
        audioThumb->setStyleSheet("color: #E3E3E3; font: 24pt; font-style: oblique;");
        subtitleThumb->setStyleSheet("color: #E3E3E3; font: 24pt; font-style: oblique;");
        break;
    }
    if (file.open(QFile::ReadOnly)) {
        list = QString::fromUtf8(file.readAll());
        file.close();
    }
    this->setStyleSheet(styleCreator(list));
    int i = 11;
    if (!_showHDR_mode)
    {
        while (i <= 19) {
            ui->tableWidget->hideColumn(i);
            i++;
        }
    } else {
        while (i <= 19) {
            ui->tableWidget->showColumn(i);
            i++;
        }
    }
    setExpandIcon();
}

QString Widget::styleCreator(const QString &list)    /*** Parsing CSS ***/
{
    QString style = list;
    QStringList varDetect;
    QStringList splitList;
    QStringList varSplitter;
    QStringList varNames;
    QStringList varValues;
    splitList << list.split(";");

    for (int i = 0; i < splitList.size(); i++) {
        if (splitList[i].indexOf("@") != -1 && splitList[i].indexOf("=") != -1) {
            varDetect.append(splitList[i]);
        }
    }
    for (int i = 0; i < varDetect.size(); i++) {

        varNames.append(varDetect[i].split("=")[0].remove(" ").remove("\n"));
        varValues.append(varDetect[i].split("=")[1].remove(" ").remove("\n"));
        style = style.remove(varDetect[i] + QString(";"));
    }
    for (int i = 0; i < varNames.size(); i++) {
        style = style.replace(varNames[i], varValues[i]);
    }
    //std::cout << style.toStdString() << std::endl;
    return style;
}

void Widget::setStatus(QString status)
{
    QTableWidgetItem *newItem_status = new QTableWidgetItem(status);
    newItem_status->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(_row, ColumnIndex::STATUS, newItem_status);
}

void Widget::restore_initial_state()    /*** Restore initial state ***/
{
    ui->treeWidget->setEnabled(true);
    animation->stop();
    add_files->setEnabled(true);
    remove_files->setEnabled(true);
    settings->setEnabled(true);
    ui->lineEditCurTime->setEnabled(true);
    ui->lineEditStartTime->setEnabled(true);
    ui->lineEditEndTime->setEnabled(true);
    ui->buttonFramePrevious->setEnabled(true);
    ui->buttonFrameNext->setEnabled(true);
    ui->horizontalSlider->setEnabled(true);
    ui->buttonSetStartTime->setEnabled(true);
    ui->buttonSetEndTime->setEnabled(true);
    ui->tableWidget->setEnabled(true);
    ui->buttonSortUp->setEnabled(true);
    ui->buttonSortDown->setEnabled(true);
    ui->actionAdd->setEnabled(true);
    ui->actionRemove->setEnabled(true);

    ui->actionAdd_preset->setEnabled(true);
    ui->actionRemove_preset->setEnabled(true);
    ui->actionEdit_preset->setEnabled(true);
    ui->buttonApplyPreset->setEnabled(true);
    ui->comboBoxMode->setEnabled(true);
    ui->buttonHotInputFile->setEnabled(true);
    ui->buttonHotOutputFile->setEnabled(true);
    ui->horizontalSlider_resize->setEnabled(true);

    ui->actionClearMetadata->setEnabled(true);
    ui->actionUndoMetadata->setEnabled(true);
    QList<QLineEdit*> linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit*>();
    foreach (QLineEdit *lineEdit, linesEditMetadata) {
        lineEdit->setEnabled(true);
    }

    ui->actionUndoTitles->setEnabled(true);
    ui->actionClearAudioTitles->setEnabled(true);
    for (int stream = 0; stream < AMOUNT_AUDIO_STREAMS; stream++) {
        QCheckBox *checkBoxAudio = ui->frameTab_2->findChild<QCheckBox *>("checkBoxAudio_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        checkBoxAudio->setEnabled(true);
        QLineEdit *lineEditLangAudio = ui->frameTab_2->findChild<QLineEdit *>("lineEditLangAudio_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        lineEditLangAudio->setEnabled(true);
        QLineEdit *lineEditTitleAudio = ui->frameTab_2->findChild<QLineEdit *>("lineEditTitleAudio_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        lineEditTitleAudio->setEnabled(true);
    }

    ui->actionClearSubtitleTitles->setEnabled(true);
    for (int stream = 0; stream < AMOUNT_SUBTITLES; stream++) {
        QCheckBox *checkBoxSubtitle = ui->frameTab_3->findChild<QCheckBox *>("checkBoxSubtitle_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        checkBoxSubtitle->setEnabled(true);
        QLineEdit *lineEditLangSubtitle = ui->frameTab_3->findChild<QLineEdit *>("lineEditLangSubtitle_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        lineEditLangSubtitle->setEnabled(true);
        QLineEdit *lineEditTitleSubtitle = ui->frameTab_3->findChild<QLineEdit *>("lineEditTitleSubtitle_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        lineEditTitleSubtitle->setEnabled(true);
    }
    ui->actionResetLabels->setEnabled(true);
    ui->actionSettings->setEnabled(true);
    _status_encode_btn = EncodingStatus::START;
    ui->actionEncode->setIcon(QIcon(":/resources/icons/16x16/cil-play.png"));
    ui->actionEncode->setToolTip(tr("Encode"));
}

bool Widget::eventFilter(QObject *watched, QEvent *event)    /*** Resize and move window ***/
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            ui->frame_middle->setFocus();
            return true;
        }
        return QWidget::eventFilter(watched, event);
    }

    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->button() == Qt::LeftButton) {
            QGuiApplication::restoreOverrideCursor();
            _clickPressedFlag = false;
            _clickPressedToResizeFlag.fill(false);
            return true;
        }
        return QWidget::eventFilter(watched, event);
    }  

    if (watched == ui->centralwidget) {     // ******** Resize window realisation ******* //
        if (!this->isMaximized()) {
            if (event->type() == QEvent::HoverLeave) {
                QGuiApplication::restoreOverrideCursor();
                return true;

            }
            if (event->type() == QEvent::HoverMove && _clickPressedToResizeFlag.indexOf(true) == -1) {
                const QPoint &&mouseCoordinate = ui->centralwidget->mapFromGlobal(QCursor::pos());
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
                    return QWidget::eventFilter(watched, event);
                }
                if (mouseCoordinate.x() > 6 && mouseCoordinate.x() < (width() - 6)) {
                    if (mouseCoordinate.y() < 6 || mouseCoordinate.y() > (height() - 6)) {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                        return true;
                    }
                    QGuiApplication::restoreOverrideCursor();
                    return QWidget::eventFilter(watched, event);
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
                    return QWidget::eventFilter(watched, event);
                }
                QGuiApplication::restoreOverrideCursor();
                return QWidget::eventFilter(watched, event);

            }
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->button() == Qt::LeftButton) {
                    _oldPosX = this->pos().x();
                    _oldPosY = this->pos().y();
                    _oldWidth = this->width();
                    _oldHeight = this->height();
                    _globalMouseClickCoordinate = mouse_event->globalPos();
                    const QPoint &&mouseClickCoordinate = mouse_event->pos();
                    if (mouseClickCoordinate.x() < 6) {
                        if (mouseClickCoordinate.y() < 6) {
                            _clickPressedToResizeFlag[Resize::LEFT_TOP] = true;
                            return true;
                        }
                        if (mouseClickCoordinate.y() > 6 && mouseClickCoordinate.y() < (_oldHeight - 6)) {
                            _clickPressedToResizeFlag[Resize::LEFT] = true;
                            return true;
                        }
                        if (mouseClickCoordinate.y() > (_oldHeight - 6)) {
                            _clickPressedToResizeFlag[Resize::LEFT_BOTTOM] = true;
                            return true;
                        }
                        return QWidget::eventFilter(watched, event);
                    }
                    if (mouseClickCoordinate.x() > 6 && mouseClickCoordinate.x() < (_oldWidth - 6)) {
                        if (mouseClickCoordinate.y() < 6) {
                            _clickPressedToResizeFlag[Resize::TOP] = true;
                            return true;
                        }
                        if (mouseClickCoordinate.y() > (_oldHeight - 6)) {
                            _clickPressedToResizeFlag[Resize::BOTTOM] = true;
                            return true;
                        }
                        return QWidget::eventFilter(watched, event);
                    }
                    if (mouseClickCoordinate.x() > (_oldWidth - 6)) {
                        if (mouseClickCoordinate.y() < 6) {
                            _clickPressedToResizeFlag[Resize::RIGHT_TOP] = true;
                            return true;
                        }
                        if (mouseClickCoordinate.y() > 6 && mouseClickCoordinate.y() < (_oldHeight - 6)) {
                            _clickPressedToResizeFlag[Resize::RIGHT] = true;
                            return true;
                        }
                        if (mouseClickCoordinate.y() > (_oldHeight - 6)) {
                            _clickPressedToResizeFlag[Resize::RIGHT_BOTTOM] = true;
                            return true;
                        }
                        return QWidget::eventFilter(watched, event);
                    }
                    return QWidget::eventFilter(watched, event);
                }
                return QWidget::eventFilter(watched, event);
            }
            if (event->type() == QEvent::MouseMove) {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->buttons() & Qt::LeftButton) {
                    const int &&index = _clickPressedToResizeFlag.indexOf(true);
                    if (index != -1) {
                        const int &&deltaX = mouse_event->globalPos().x();
                        const int &&deltaY = mouse_event->globalPos().y();
                        const int &&deltaWidth = deltaX - _globalMouseClickCoordinate.x();
                        const int &&deltaHeight = deltaY - _globalMouseClickCoordinate.y();
                        switch (index) {
                        case Resize::LEFT:
                            setGeometry(deltaX, _oldPosY, _oldWidth - deltaWidth, _oldHeight);
                            break;
                        case Resize::LEFT_TOP:
                            setGeometry(deltaX, deltaY, _oldWidth - deltaWidth, _oldHeight - deltaHeight);
                            break;
                        case Resize::LEFT_BOTTOM:
                            setGeometry(deltaX, _oldPosY, _oldWidth - deltaWidth, _oldHeight + deltaHeight);
                            break;
                        case Resize::TOP:
                            setGeometry(_oldPosX, deltaY, _oldWidth, _oldHeight - deltaHeight);
                            break;
                        case Resize::RIGHT:
                            setGeometry(_oldPosX, _oldPosY, _oldWidth + deltaWidth, _oldHeight);
                            break;
                        case Resize::RIGHT_TOP:
                            setGeometry(_oldPosX, deltaY, _oldWidth + deltaWidth, _oldHeight - deltaHeight);
                            break;
                        case Resize::RIGHT_BOTTOM:
                            setGeometry(_oldPosX, _oldPosY, _oldWidth + deltaWidth, _oldHeight + deltaHeight);
                            break;
                        case Resize::BOTTOM:
                            setGeometry(_oldPosX, _oldPosY, _oldWidth, _oldHeight + deltaHeight);
                            break;
                        }
                        return true;
                    }
                    return QWidget::eventFilter(watched, event);
                }
                return QWidget::eventFilter(watched, event);
            }
            return QWidget::eventFilter(watched, event);
        }
        return QWidget::eventFilter(watched, event);
    }

    if (watched == ui->frame_main) {    // ***** Restore cursor realisation ******** //
        if (event->type() == QEvent::HoverMove) {
            QGuiApplication::restoreOverrideCursor();
            return true;
        }
        return QWidget::eventFilter(watched, event);
    }

    if (watched == ui->frame_top) {     // ********* Drag and expand window realisation ********* //
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton) {
                _mouseClickCoordinate = mouse_event->pos();
                _clickPressedFlag = true;
                return true;
            }
            return QWidget::eventFilter(watched, event);
        }
        if ((event->type() == QEvent::MouseMove) && _clickPressedFlag) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton) {
                if (this->isMaximized()) on_expandWindow_clicked();
                this->move(mouse_event->globalPos() - _mouseClickCoordinate);
                return true;
            }
            return QWidget::eventFilter(watched, event);
        }
        if (event->type() == QEvent::MouseButtonDblClick) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton) {
                on_expandWindow_clicked();
                return true;
            }
            return QWidget::eventFilter(watched, event);
        }
        return QWidget::eventFilter(watched, event);
    }

    if (watched == raiseThumb) {    // ********* Click thumb realisation ********** //
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton) {
                on_actionAdd_clicked();
                return true;
            }
            return QWidget::eventFilter(watched, event);
        }
        return QWidget::eventFilter(watched, event);
    }

    if (watched == ui->labelThumb) {
        if (event->type() == QEvent::Resize) {
            if (!preview_pixmap.isNull()) {
                QPixmap pix_scaled;
                pix_scaled = preview_pixmap.scaled(ui->frame_preview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                ui->labelThumb->setPixmap(pix_scaled);
            }
        }
        return QWidget::eventFilter(watched, event);
    }
    return QWidget::eventFilter(watched, event);
}

/************************************************
** Encoder
************************************************/

void Widget::initEncoding()
{
    ui->textBrowser_log->clear();
    const QString globalTitle = ui->lineEditGlobalTitle->text();
    encoder->initEncoding(_temp_file,
                          _input_file,
                          _output_file,
                          _width,
                          _height,
                          _fps,
                          _startTime,
                          _endTime,
                          _dur,
                          extension,
                          globalTitle,
                          _cur_param,
                          _hdr,
                          _videoMetadata,
                          _audioLang,
                          _audioTitle,
                          _subtitleLang,
                          _subtitleTitle,
                          _audioStreamCheckState,
                          _subtitleCheckState,
                          &_fr_count);
}

void Widget::onEncodingMode(const QString &mode)
{
    ui->label_Progress->setText(mode);
}

void Widget::onEncodingStarted()
{
    setStatus(tr("Encoding"));
    ui->treeWidget->setEnabled(false);
    add_files->setEnabled(false);
    remove_files->setEnabled(false);
    settings->setEnabled(false);
    ui->lineEditCurTime->setEnabled(false);
    ui->lineEditStartTime->setEnabled(false);
    ui->lineEditEndTime->setEnabled(false);
    ui->buttonFramePrevious->setEnabled(false);
    ui->buttonFrameNext->setEnabled(false);
    ui->horizontalSlider->setEnabled(false);
    ui->buttonSetStartTime->setEnabled(false);
    ui->buttonSetEndTime->setEnabled(false);
    ui->tableWidget->setEnabled(false);
    ui->buttonSortUp->setEnabled(false);
    ui->buttonSortDown->setEnabled(false);
    ui->actionAdd->setEnabled(false);
    ui->actionRemove->setEnabled(false);

    ui->actionAdd_preset->setEnabled(false);
    ui->actionRemove_preset->setEnabled(false);
    ui->actionEdit_preset->setEnabled(false);
    ui->buttonApplyPreset->setEnabled(false);
    ui->comboBoxMode->setEnabled(false);
    ui->buttonHotInputFile->setEnabled(false);
    ui->buttonHotOutputFile->setEnabled(false);
    ui->horizontalSlider_resize->setEnabled(false);

    ui->actionClearMetadata->setEnabled(false);
    ui->actionUndoMetadata->setEnabled(false);
    QList<QLineEdit*> linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit*>();
    foreach (QLineEdit *lineEdit, linesEditMetadata) {
        lineEdit->setEnabled(false);
    }

    ui->actionUndoTitles->setEnabled(false);
    ui->actionClearAudioTitles->setEnabled(false);
    for (int stream = 0; stream < AMOUNT_AUDIO_STREAMS; stream++) {
        QCheckBox *checkBoxAudio = ui->frameTab_2->findChild<QCheckBox *>("checkBoxAudio_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        checkBoxAudio->setEnabled(false);
        QLineEdit *lineEditLangAudio = ui->frameTab_2->findChild<QLineEdit *>("lineEditLangAudio_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        lineEditLangAudio->setEnabled(false);
        QLineEdit *lineEditTitleAudio = ui->frameTab_2->findChild<QLineEdit *>("lineEditTitleAudio_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        lineEditTitleAudio->setEnabled(false);
    }

    ui->actionClearSubtitleTitles->setEnabled(false);
    for (int stream = 0; stream < AMOUNT_SUBTITLES; stream++) {
        QCheckBox *checkBoxSubtitle = ui->frameTab_3->findChild<QCheckBox *>("checkBoxSubtitle_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        checkBoxSubtitle->setEnabled(false);
        QLineEdit *lineEditLangSubtitle = ui->frameTab_3->findChild<QLineEdit *>("lineEditLangSubtitle_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        lineEditLangSubtitle->setEnabled(false);
        QLineEdit *lineEditTitleSubtitle = ui->frameTab_3->findChild<QLineEdit *>("lineEditTitleSubtitle_"
            + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
        lineEditTitleSubtitle->setEnabled(false);
    }

    ui->actionResetLabels->setEnabled(false);
    ui->actionSettings->setEnabled(false);
    ui->labelAnimation->show();
    animation->start();
    ui->label_Progress->show();
    ui->label_Remaining->show();
    ui->label_RemTime->show();
    ui->progressBar->show();
}

void Widget::onEncodingInitError(const QString &message)
{
    restore_initial_state();
    call_task_complete(message, false);
    /*_status_encode_btn = "start";
    ui->actionEncode->setIcon(QIcon(":/resources/icons/16x16/cil-play.png"));
    ui->actionEncode->setToolTip(tr("Encode"));
    call_task_complete(_message, false);*/
}

void Widget::onEncodingProgress(const int &percent, const float &rem_time)
{
    ui->progressBar->setValue(percent);
    ui->label_RemTime->setText(timeConverter(rem_time));
}

void Widget::onEncodingLog(const QString &log)
{
    ui->textBrowser_log->append(log);
}

void Widget::onEncodingCompleted()
{
    std::cout << "Completed ..." << std::endl;  //  Debug info //
    setStatus(tr("Done!"));
    animation->stop();
    ui->label_Progress->hide();
    ui->label_Remaining->hide();
    ui->label_RemTime->hide();
    ui->labelAnimation->hide();
    ui->progressBar->hide();
    if (_batch_mode == true) {
        int row = ui->tableWidget->currentRow();
        int numRows = ui->tableWidget->rowCount();
        if (numRows > (row + 1)) {
            ui->tableWidget->selectRow(row + 1);
            initEncoding();
        } else {
            restore_initial_state();
            time_t end_t = time(nullptr);
            float elps_t = static_cast<float>(end_t - _strt_t);
            if (elps_t < 0.0f) {
                elps_t = 0.0f;
            }
            if (_protection == true) {
                timer->stop();
            }
            _message = tr("Task completed!\n\n Elapsed time: ") + timeConverter(elps_t);
            call_task_complete(_message, false);
        }
    } else {
        restore_initial_state();
        time_t end_t = time(nullptr);
        float elps_t = static_cast<float>(end_t - _strt_t);
        if (elps_t < 0.0f) {
            elps_t = 0.0f;
        }
        if (_protection == true) {
            timer->stop();
        }
        _message = tr("Task completed!\n\n Elapsed time: ") + timeConverter(elps_t);
        call_task_complete(_message, false);
    }
    QDir().remove(QDir::homePath() + QString("/ffmpeg2pass-0.log"));
    QDir().remove(QDir::homePath() + QString("/ffmpeg2pass-0.log.mbtree"));
    QDir().remove(QDir::homePath() + QString("/x265_2pass.log"));
    QDir().remove(QDir::homePath() + QString("/x265_2pass.log.cutree"));
}

void Widget::onEncodingAborted()
{
    std::cout << "Stop execute ..." << std::endl;  //  Debug info //
    if (_protection) timer->stop();
    setStatus(tr("Stop"));
    ui->label_Progress->hide();
    ui->label_Remaining->hide();
    ui->label_RemTime->hide();
    ui->labelAnimation->hide();
    ui->progressBar->hide();
    _message = tr("The current encoding process has been canceled!\n");
    restore_initial_state();
    call_task_complete(_message, false);
}

void Widget::onEncodingError(const QString &error_message)
{
    std::cout << "Error_1 ..." << std::endl;  //  Debug info //
    if (_protection) timer->stop();
    setStatus(tr("Error!"));
    restore_initial_state();
    if (error_message != "") {
        _message = tr("An error occurred: ") + error_message;
    } else {
        _message = tr("Unexpected error occurred!");
    }
    call_task_complete(_message, false);
}

void Widget::pause()    // Pause encoding
{
    if (_protection) timer->stop();
    if (encoder->getEncodingState() != QProcess::NotRunning) {
        setStatus(tr("Pause"));
        animation->stop();
        encoder->pauseEncoding();
    }
}

void Widget::resume()   // Resume encoding
{
    if (_protection) timer->start();
    if (encoder->getEncodingState() != QProcess::NotRunning) {
        setStatus(tr("Encoding"));
        animation->start();
        encoder->resumeEncoding();
    }
}

void Widget::repeatHandler_Type_1()  /*** Repeat handler ***/
{
    std::cout<< "Call by timer..." << std::endl;
    on_actionEncode_clicked();
    call_task_complete(tr("Pause"), true);
    on_actionEncode_clicked();
}

/************************************************
** Task Window
************************************************/

void Widget::on_actionAdd_clicked() /*** Add files ***/
{
    QFileDialog openFilesWindow(nullptr);
    openFilesWindow.setWindowTitle("Open Files");
    openFilesWindow.setMinimumWidth(600);
    openFilesWindow.setWindowFlags(Qt::Dialog | Qt::SubWindow);
    openFilesWindow.setOption(QFileDialog::DontResolveSymlinks, true);
    if (_desktopEnv == "gnome") openFilesWindow.setOption(QFileDialog::DontUseNativeDialog, true);
    openFilesWindow.setFileMode(QFileDialog::ExistingFiles);
    openFilesWindow.setAcceptMode(QFileDialog::AcceptOpen);
    openFilesWindow.setDirectory(_openDir);
    openFilesWindow.setNameFilter(tr("Video Files: *.avi, *.m2ts, *.m4v, *.mkv, *.mov, *.mp4, "
                                     "*.mpeg, *.mpg, *.mxf, *.ts, *.webm (*.avi *.m2ts *.m4v "
                                     "*.mkv *.mov *.mp4 *.mpeg *.mpg *.mxf *.ts *.webm);;All files (*.*)"));
    if (openFilesWindow.exec() == QFileDialog::Accepted) {
        const QStringList openFileNames = openFilesWindow.selectedFiles();
        openFiles(openFileNames);
    }
}

void Widget::on_actionRemove_clicked()  /*** Remove file from table ***/
{
    _row = ui->tableWidget->currentRow();
    if (_row != -1) {
        ui->tableWidget->removeRow(_row);
    }
}

void Widget::on_buttonCloseTaskWindow_clicked()
{
    on_closeWindow_clicked();
}

void Widget::on_buttonSortDown_clicked()    /*** Sort table ***/
{
    ui->tableWidget->sortByColumn(ColumnIndex::FILENAME, Qt::DescendingOrder);
}

void Widget::on_buttonSortUp_clicked()    /*** Sort table ***/
{
    ui->tableWidget->sortByColumn(ColumnIndex::FILENAME, Qt::AscendingOrder);
}

void Widget::on_actionEncode_clicked()  /*** Encode button ***/
{
    switch (_status_encode_btn) {
    case EncodingStatus::START: {
        std::cout << "Status encode btn: start" << std::endl;  // Debug info //
        int cnt = ui->tableWidget->rowCount();
        if (cnt == 0) {
            _message = tr("Select input file first!");
            call_task_complete(_message, false);
            return;
        }
        if (_pos_cld == -1) {
            _message = tr("Select preset first!");
            call_task_complete(_message, false);
            return;
        }
        _status_encode_btn = EncodingStatus::PAUSE;
        ui->actionEncode->setIcon(QIcon(":/resources/icons/16x16/cil-pause.png"));
        ui->actionEncode->setToolTip(tr("Pause"));
        _strt_t = time(nullptr);
        if (_protection == true) {
            timer->start();
        }
        initEncoding();
        break;
    }
    case EncodingStatus::PAUSE: {
        std::cout << "Status encode btn: pause" << std::endl;  // Debug info //
        pause();
        _status_encode_btn = EncodingStatus::RESUME;
        ui->actionEncode->setIcon(QIcon(":/resources/icons/16x16/cil-forward.png"));
        ui->actionEncode->setToolTip(tr("Resume"));
        break;
    }
    case EncodingStatus::RESUME: {
        std::cout << "Status encode btn: resume" << std::endl;  // Debug info //
        resume();
        _status_encode_btn = EncodingStatus::PAUSE;
        ui->actionEncode->setIcon(QIcon(":/resources/icons/16x16/cil-pause.png"));
        ui->actionEncode->setToolTip(tr("Pause"));
        break;
    }
    default:
        break;
    }
}

void Widget::on_actionStop_clicked()    /*** Stop ***/
{
    std::cout << "Call Stop ..." << std::endl;  //  Debug info //
    if (encoder->getEncodingState() != QProcess::NotRunning) {
        _message = tr("Stop encoding?");
        bool confirm = call_dialog(_message);
        if (confirm) {
            encoder->stopEncoding();
        }
    }
}

void Widget::openFiles(const QStringList &openFileNames)    /*** Open files ***/
{
    showOpeningFiles(true);
    ui->labelAnimation->hide();
    ui->label_Progress->hide();
    ui->label_Remaining->hide();
    ui->label_RemTime->hide();
    ui->progressBar->hide();
    MediaInfo MI;
    int i = 1;
    int countFileNames = openFileNames.size();
    while (i <= countFileNames)
    {
        const int numRows = ui->tableWidget->rowCount();
        ui->tableWidget->setRowCount(numRows + 1);
        const QString file = openFileNames.at(i-1);
        const QString inputFolder = QFileInfo(file).absolutePath();
        const QString inputFile = QFileInfo(file).fileName();
        showOpeningFiles(inputFile);
        showOpeningFiles(0);
        QApplication::processEvents();
        if (i == 1) _openDir = inputFolder;
        MI.Open(file.toStdWString());
        QString duration_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"Duration"));
        double duration_double = 0.001 * duration_qstr.toDouble();
        float duration_float = static_cast<float>(duration_double);
        QString durationTime = timeConverter(duration_float);
        QString bitrate_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"BitRate"));
        int bitrate_int = static_cast<int>(0.001 * bitrate_qstr.toDouble());        
        QString stream_size_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"StreamSize"));
        QString mastering_display_luminance_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"MasteringDisplay_Luminance"));
        QString mastering_display_luminance_rep = mastering_display_luminance_qstr.replace("min: ", "").replace("max: ", "").replace(" cd/m2", "");
        QString min_luminance = mastering_display_luminance_rep.split(", ")[0];
        QString max_luminance = mastering_display_luminance_rep.replace(min_luminance, "").replace(", ", "");
        QString color_primaries = QString::fromStdWString(MI.Get(Stream_Video, 0, L"colour_primaries"));
        QString matrix_coefficients = QString::fromStdWString(MI.Get(Stream_Video, 0, L"matrix_coefficients"));
        QString transfer_characteristics = QString::fromStdWString(MI.Get(Stream_Video, 0, L"transfer_characteristics"));
        QString mastering_display_color_primaries = QString::fromStdWString(MI.Get(Stream_Video, 0, L"MasteringDisplay_ColorPrimaries"));
        QString color_range = QString::fromStdWString(MI.Get(Stream_Video, 0, L"colour_range"));
        QString maxCll = QString::fromStdWString(MI.Get(Stream_Video, 0, L"MaxCLL"));
        QString maxFall = QString::fromStdWString(MI.Get(Stream_Video, 0, L"MaxFALL"));
        QString width_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"Width"));
        QString height_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"Height"));
        QString chroma_subsampling = QString::fromStdWString(MI.Get(Stream_Video, 0, L"ChromaSubsampling"));
        QString aspect_ratio = QString::fromStdWString(MI.Get(Stream_Video, 0, L"DisplayAspectRatio"));
        QString color_space = QString::fromStdWString(MI.Get(Stream_Video, 0, L"ColorSpace"));
        QString bit_depth = QString::fromStdWString(MI.Get(Stream_Video, 0, L"BitDepth"));
        QString fmt_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"Format"));
        QString fps_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"FrameRate"));
        QString size = width_qstr + "x" + height_qstr;
        QString videoTitle = QString::fromStdWString(MI.Get(Stream_Video, 0, L"Title"));
        QString videoAuthor = QString::fromStdWString(MI.Get(Stream_General, 0, L"AUTHOR"));
        QString videoYear = QString::fromStdWString(MI.Get(Stream_General, 0, L"YEAR"));
        QString videoPerformer = QString::fromStdWString(MI.Get(Stream_General, 0, L"Performer"));
        QString videoDescription = QString::fromStdWString(MI.Get(Stream_General, 0, L"Description"));
        QString videoMovieName = QString::fromStdWString(MI.Get(Stream_General, 0, L"Movie"));

        int len = mastering_display_color_primaries.length();
        QString white_coord = "";
        QString chroma_coord = "";
        if (len > 15) {
            QStringList mdcp = mastering_display_color_primaries.split(",");
            QString r = mdcp[0].replace("R: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
            QString g = mdcp[1].replace(" G: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
            QString b = mdcp[2].replace(" B: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
            white_coord = mdcp[3].replace(" White point: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
            chroma_coord = r + "," + g + "," + b;
            mastering_display_color_primaries = "Undefined";
        }

        if (fmt_qstr == "") {
            fmt_qstr = "Undef";
        }
        if (fps_qstr == "") {
            fps_qstr = "Undef";
        }
        if (width_qstr == "") {
            size = "Undef";
        }
        if (durationTime == "00:00:00") {
            durationTime = "Undef";
        }

        QTableWidgetItem *newItem_file = new QTableWidgetItem(inputFile);
        QTableWidgetItem *newItem_folder = new QTableWidgetItem(inputFolder);
        QTableWidgetItem *newItem_fmt = new QTableWidgetItem(fmt_qstr);
        QTableWidgetItem *newItem_resolution = new QTableWidgetItem(size);
        QTableWidgetItem *newItem_duration_time = new QTableWidgetItem(durationTime);
        QTableWidgetItem *newItem_fps = new QTableWidgetItem(fps_qstr);
        QTableWidgetItem *newItem_aspect_ratio = new QTableWidgetItem(aspect_ratio);
        QTableWidgetItem *newItem_bitrate = new QTableWidgetItem(QString::number(bitrate_int));
        QTableWidgetItem *newItem_subsampling = new QTableWidgetItem(chroma_subsampling);
        QTableWidgetItem *newItem_bit_depth = new QTableWidgetItem(bit_depth);
        QTableWidgetItem *newItem_color_space = new QTableWidgetItem(color_space);
        QTableWidgetItem *newItem_color_range = new QTableWidgetItem(color_range);
        QTableWidgetItem *newItem_color_primaries = new QTableWidgetItem(color_primaries.replace(".", ""));
        QTableWidgetItem *newItem_color_matrix = new QTableWidgetItem(matrix_coefficients.replace(" ", "").replace(".", "").replace("on-", "").replace("onstant", ""));
        QTableWidgetItem *newItem_transfer_characteristics = new QTableWidgetItem(transfer_characteristics.replace(".", ""));
        QTableWidgetItem *newItem_max_lum = new QTableWidgetItem(max_luminance);
        QTableWidgetItem *newItem_min_lum = new QTableWidgetItem(min_luminance);
        QTableWidgetItem *newItem_max_cll = new QTableWidgetItem(maxCll.replace(" cd/m2", ""));
        QTableWidgetItem *newItem_max_fall = new QTableWidgetItem(maxFall.replace(" cd/m2", ""));
        QTableWidgetItem *newItem_mastering_display = new QTableWidgetItem(mastering_display_color_primaries);
        QTableWidgetItem *newItem_chroma_coord = new QTableWidgetItem(chroma_coord);
        QTableWidgetItem *newItem_white_coord = new QTableWidgetItem(white_coord);
        QTableWidgetItem *newItem_duration = new QTableWidgetItem(QString::number(duration_double, 'f', 3));
        QTableWidgetItem *newItem_stream_size = new QTableWidgetItem(stream_size_qstr);
        QTableWidgetItem *newItem_width = new QTableWidgetItem(width_qstr);
        QTableWidgetItem *newItem_height = new QTableWidgetItem(height_qstr);
        QTableWidgetItem *newItem_videoTitle = new QTableWidgetItem(videoTitle);
        QTableWidgetItem *newItem_videoAuthor = new QTableWidgetItem(videoAuthor);
        QTableWidgetItem *newItem_videoYear = new QTableWidgetItem(videoYear);
        QTableWidgetItem *newItem_videoPerformer = new QTableWidgetItem(videoPerformer);
        QTableWidgetItem *newItem_videoDescription = new QTableWidgetItem(videoDescription);
        QTableWidgetItem *newItem_videoMovieName = new QTableWidgetItem(videoMovieName);
        QTableWidgetItem *newItem_startTime = new QTableWidgetItem("0");
        QTableWidgetItem *newItem_endTime = new QTableWidgetItem("0");

        newItem_fmt->setTextAlignment(Qt::AlignCenter);
        newItem_resolution->setTextAlignment(Qt::AlignCenter);
        newItem_duration_time->setTextAlignment(Qt::AlignCenter);
        newItem_fps->setTextAlignment(Qt::AlignCenter);
        newItem_aspect_ratio->setTextAlignment(Qt::AlignCenter);
        newItem_bitrate->setTextAlignment(Qt::AlignCenter);
        newItem_subsampling->setTextAlignment(Qt::AlignCenter);
        newItem_bit_depth->setTextAlignment(Qt::AlignCenter);
        newItem_color_space->setTextAlignment(Qt::AlignCenter);
        newItem_color_range->setTextAlignment(Qt::AlignCenter);
        newItem_color_primaries->setTextAlignment(Qt::AlignCenter);
        newItem_color_matrix->setTextAlignment(Qt::AlignCenter);
        newItem_transfer_characteristics->setTextAlignment(Qt::AlignCenter);
        newItem_max_lum->setTextAlignment(Qt::AlignCenter);
        newItem_min_lum->setTextAlignment(Qt::AlignCenter);
        newItem_max_cll->setTextAlignment(Qt::AlignCenter);
        newItem_max_fall->setTextAlignment(Qt::AlignCenter);
        newItem_mastering_display->setTextAlignment(Qt::AlignCenter);

        ui->tableWidget->setItem(numRows, ColumnIndex::FILENAME, newItem_file);
        ui->tableWidget->setItem(numRows, ColumnIndex::PATH, newItem_folder);
        ui->tableWidget->setItem(numRows, ColumnIndex::FORMAT, newItem_fmt);
        ui->tableWidget->setItem(numRows, ColumnIndex::RESOLUTION, newItem_resolution);
        ui->tableWidget->setItem(numRows, ColumnIndex::DURATION, newItem_duration_time);
        ui->tableWidget->setItem(numRows, ColumnIndex::FPS, newItem_fps);
        ui->tableWidget->setItem(numRows, ColumnIndex::AR, newItem_aspect_ratio);
        ui->tableWidget->setItem(numRows, ColumnIndex::BITRATE, newItem_bitrate);
        ui->tableWidget->setItem(numRows, ColumnIndex::SUBSAMPLING, newItem_subsampling);
        ui->tableWidget->setItem(numRows, ColumnIndex::BITDEPTH, newItem_bit_depth);
        ui->tableWidget->setItem(numRows, ColumnIndex::COLORSPACE, newItem_color_space);
        ui->tableWidget->setItem(numRows, ColumnIndex::COLORRANGE, newItem_color_range);
        ui->tableWidget->setItem(numRows, ColumnIndex::COLORPRIM, newItem_color_primaries);
        ui->tableWidget->setItem(numRows, ColumnIndex::COLORMATRIX, newItem_color_matrix);
        ui->tableWidget->setItem(numRows, ColumnIndex::TRANSFER, newItem_transfer_characteristics);
        ui->tableWidget->setItem(numRows, ColumnIndex::MAXLUM, newItem_max_lum);
        ui->tableWidget->setItem(numRows, ColumnIndex::MINLUM, newItem_min_lum);
        ui->tableWidget->setItem(numRows, ColumnIndex::MAXCLL, newItem_max_cll);
        ui->tableWidget->setItem(numRows, ColumnIndex::MAXFALL, newItem_max_fall);
        ui->tableWidget->setItem(numRows, ColumnIndex::MASTERDISPLAY, newItem_mastering_display);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_CHROMACOORD, newItem_chroma_coord);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_WHITECOORD, newItem_white_coord);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_DUR, newItem_duration);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_STREAMSIZE, newItem_stream_size);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_WIDTH, newItem_width);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_HEIGHT, newItem_height);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_VIDEOTITLE, newItem_videoTitle);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_VIDEOAUTHOR, newItem_videoAuthor);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_VIDEOYEAR, newItem_videoYear);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_VIDEOPERF, newItem_videoPerformer);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_VIDEODESCR, newItem_videoDescription);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_VIDEOMOVIENAME, newItem_videoMovieName);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_STARTTIME, newItem_startTime);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_ENDTIME, newItem_endTime);

        int smplrt_int;
        QString audioFormat("");
        QString audioLang("");
        QString audioTitle("");
        QString smplrt("");
        QString smplrt_qstr("");
        QString audioCheckstate;
        for (int j = 0; j < AMOUNT_AUDIO_STREAMS; j++) {
            audioFormat = QString::fromStdWString(MI.Get(Stream_Audio, size_t(j), L"Format"));
            audioLang = QString::fromStdWString(MI.Get(Stream_Audio, size_t(j), L"Language"));
            audioTitle = QString::fromStdWString(MI.Get(Stream_Audio, size_t(j), L"Title"));
            smplrt_qstr = QString::fromStdWString(MI.Get(Stream_Audio, size_t(j), L"SamplingRate"));
            smplrt_int = static_cast<int>(smplrt_qstr.toFloat() / 1000);
            audioCheckstate = "0";

            if (smplrt_int != 0) {
                smplrt = QString::number(smplrt_int);
            } else {
                smplrt = "";
            }
            if (audioFormat != "") {
                audioFormat = audioFormat + "  " + smplrt + " kHz";
                audioCheckstate = "1";
            }
            QTableWidgetItem *newItem_audio = new QTableWidgetItem(audioFormat);
            QTableWidgetItem *newItem_lang = new QTableWidgetItem(audioLang);
            QTableWidgetItem *newItem_title = new QTableWidgetItem(audioTitle);
            QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(audioCheckstate);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_AUDIO_1, newItem_audio);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_AUDIOLANG_1, newItem_lang);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_AUDIOTITLE_1, newItem_title);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_AUDIOCHECK_1, newItem_checkstate);
        }

        QString subtitleFormat("");
        QString subtitleLang("");
        QString subtitleTitle("");
        QString subtitleCheckstate;
        for (int j = 0; j < AMOUNT_SUBTITLES; j++) {
            subtitleFormat = QString::fromStdWString(MI.Get(Stream_Text, size_t(j), L"Format"));
            subtitleLang = QString::fromStdWString(MI.Get(Stream_Text, size_t(j), L"Language"));
            subtitleTitle = QString::fromStdWString(MI.Get(Stream_Text, size_t(j), L"Title"));
            subtitleCheckstate = "0";

            if (subtitleFormat != "") {
                subtitleCheckstate = "1";
            }
            QTableWidgetItem *newItem_subtitle = new QTableWidgetItem(subtitleFormat);
            QTableWidgetItem *newItem_lang = new QTableWidgetItem(subtitleLang);
            QTableWidgetItem *newItem_title = new QTableWidgetItem(subtitleTitle);
            QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(subtitleCheckstate);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_SUBTITLE_1, newItem_subtitle);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_SUBLANG_1, newItem_lang);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_TITLESUB_1, newItem_title);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_SUBCHECK_1, newItem_checkstate);
        }
        MI.Close();
        showOpeningFiles(50);
        QApplication::processEvents();
        ui->tableWidget->selectRow(ui->tableWidget->rowCount() - 1);
        QApplication::processEvents();
        showOpeningFiles(100);
        QApplication::processEvents();
#if defined (Q_OS_UNIX)
        usleep(50000);
#elif defined (Q_OS_WIN64)
        Sleep(50);
#endif
        i++;
    }
    showOpeningFiles(false);
}

void Widget::on_tableWidget_itemSelectionChanged()
{
    ui->labelAnimation->hide();
    ui->label_Progress->hide();
    ui->label_Remaining->hide();
    ui->label_RemTime->hide();
    ui->progressBar->hide();
    ui->labelSplitPreview->clear();
    ui->horizontalSlider->blockSignals(true);
    ui->horizontalSlider->setValue(0);
    ui->horizontalSlider->blockSignals(false);
    ui->lineEditCurTime->clear();
    ui->lineEditStartTime->clear();
    ui->lineEditEndTime->clear();

    // **************************** Disable audio widgets ***********************************//
    QList <QCheckBox *> checkBoxAudio = ui->frameTab_2->findChildren<QCheckBox *>();
    QList <QLineEdit *> lineEditAudio = ui->frameTab_2->findChildren<QLineEdit *>();
    QList <QLabel*> labelsAudio = ui->frameTab_2->findChildren<QLabel*>();
    foreach (QCheckBox *checkBox, checkBoxAudio) {
        checkBox->setVisible(false);
    }
    foreach (QLineEdit *lineEdit, lineEditAudio) {
        lineEdit->setVisible(false);
    }
    foreach (QLabel *label, labelsAudio) {
        label->setVisible(false);
    }
    audioThumb->setVisible(true);

    // **************************** Disable subtitle widgets ***********************************//
    QList <QCheckBox *> checkBoxSubtitle = ui->frameTab_3->findChildren<QCheckBox *>();
    QList <QLineEdit *> lineEditSubtitle = ui->frameTab_3->findChildren<QLineEdit *>();
    QList <QLabel*> labelsSubtitle = ui->frameTab_3->findChildren<QLabel*>();
    foreach (QCheckBox *checkBox, checkBoxSubtitle) {
        checkBox->setVisible(false);
    }
    foreach (QLineEdit *lineEdit, lineEditSubtitle) {
        lineEdit->setVisible(false);
    }
    foreach (QLabel *label, labelsSubtitle) {
        label->setVisible(false);
    }
    subtitleThumb->setVisible(true);

    _row = ui->tableWidget->currentRow();
    if (_row != -1) {
        raiseThumb->hide();
        get_current_data();

    } else {
        // ********************************* Reset widgets ***************************************//
        raiseThumb->show();
        preview_pixmap = QPixmap();
        ui->labelThumb->clear();
        ui->textBrowser_1->clear();
        ui->textBrowser_2->clear();
        ui->labelThumb->setText(tr("Preview"));
        ui->label_source->setText("");
        ui->label_output->setText("");
        ui->horizontalSlider->setMaximum(0);

        // **************************** Reset metadata widgets ***********************************//
        QList<QLineEdit *> linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit *>();
        foreach (QLineEdit *lineEdit, linesEditMetadata) {
            lineEdit->clear();
            lineEdit->setEnabled(false);
        }

        // **************************** Reset audio widgets ***********************************//
        foreach (QLineEdit *lineEdit, lineEditAudio) {
            lineEdit->clear();
        }
        foreach (QCheckBox *checkBox, checkBoxAudio) {
            checkBox->setChecked(false);
        }

        // **************************** Reset subtitle widgets ***********************************//
        foreach (QLineEdit *lineEdit, lineEditSubtitle) {
            lineEdit->clear();
        }
        foreach (QCheckBox *checkBox, checkBoxSubtitle) {
            checkBox->setChecked(false);
        }

        // **************************** Reset video variables ***********************************//
        _dur = 0.0;
        _input_file = "";
        _stream_size = "";
        _width = "";
        _height = "";
        _fmt = "";
        _fps = "";
        _fr_count = 0;
        _startTime = 0.0;
        _endTime = 0.0;

        _hdr[CUR_COLOR_RANGE] = "";    // color range
        _hdr[CUR_COLOR_PRIMARY] = "";  // color primary
        _hdr[CUR_COLOR_MATRIX] = "";   // color matrix
        _hdr[CUR_TRANSFER] = "";       // transfer
        _hdr[CUR_MAX_LUM] = "";        // max lum
        _hdr[CUR_MIN_LUM] = "";        // min lum
        _hdr[CUR_MAX_CLL] = "";        // max cll
        _hdr[CUR_MAX_FALL] = "";       // max fall
        _hdr[CUR_MASTER_DISPLAY] = ""; // master display
        _hdr[CUR_CHROMA_COORD] = "";   // chr coord
        _hdr[CUR_WHITE_COORD] = "";    // white coord

        // **************************** Reset metadata variables ***********************************//
        _videoMetadata[VIDEO_TITLE] = "";
        _videoMetadata[VIDEO_AUTHOR] = "";
        _videoMetadata[VIDEO_YEAR] = "";
        _videoMetadata[VIDEO_PERFORMER] = "";
        _videoMetadata[VIDEO_DESCRIPTION] = "";
        _videoMetadata[VIDEO_MOVIENAME] = "";

        // **************************** Reset audio variables ***********************************//
        for (int i = 0; i < AMOUNT_AUDIO_STREAMS; i++) {
            _audioStreamCheckState[i] = 0;
            _audioLang[i] = "";
            _audioTitle[i] = "";
        }

        // **************************** Reset subtitle variables ***********************************//
        for (int i = 0; i < AMOUNT_SUBTITLES; i++) {
            _subtitleCheckState[i] = 0;
            _subtitleLang[i] = "";
            _subtitleTitle[i] = "";
        }
    }
}

void Widget::resizeTableRows(int rows_height)
{
    QHeaderView *verticalHeader = ui->tableWidget->verticalHeader();
    verticalHeader->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader->setDefaultSectionSize(rows_height);
    if (rows_height == 25) {
        ui->tableWidget->setIconSize(QSize(16, 16));
    } else {
        int rows_width = static_cast<int>(round(1.777f*rows_height));
        ui->tableWidget->setIconSize(QSize(rows_width, rows_height));
    }
    int numRows = ui->tableWidget->rowCount();
    if (numRows > 0) {
        int row = ui->tableWidget->currentRow();
        ui->tableWidget->clearSelection();
        if (numRows > 1) {
            for (int i = 0; i < numRows; i++) {
                ui->tableWidget->selectRow(i);
            }
        }
        ui->tableWidget->selectRow(row);
    }
}

void Widget::resetView()
{
    const QString defaultSettings(":/resources/data/default_settings.ini");
    QSettings *settings = new QSettings(defaultSettings, QSettings::IniFormat, this);
    if (settings->value("Version").toInt() == SETTINGS_VERSION) {
        // Restore Main Window
        settings->beginGroup("MainWindow");
        window->restoreState(settings->value("MainWindow/state").toByteArray());
        settings->endGroup();
    }
    delete settings;

    QList<int> dockSizesX = {};
    QList<int> dockSizesY = {};
    float coeffX[DOCKS_COUNT] = {0.25f, 0.04f, 0.48f, 0.48f, 0.25f, 0.25f, 0.25f, 0.25f};
    float coeffY[DOCKS_COUNT] = {0.9f, 0.1f, 0.1f, 0.1f, 0.9f, 0.9f, 0.9f, 0.9f};
    for (int ind = 0; ind < DOCKS_COUNT; ind++) {
        int dockWidth = static_cast<int>(coeffX[ind] * this->width());
        int dockHeight = static_cast<int>(coeffY[ind] * this->height());
        dockSizesX.append(dockWidth);
        dockSizesY.append(dockHeight);
    }
    setDocksParameters(dockSizesX, dockSizesY);
}

void Widget::provideContextMenu(const QPoint &position)     /*** Call table items menu  ***/
{
    QTableWidgetItem *item = ui->tableWidget->itemAt(0, position.y());
    if (item != nullptr) {
        itemMenu->exec(ui->tableWidget->mapToGlobal(QPoint(position.x(), position.y() + 35)));
    }
}

void Widget::dragEnterEvent(QDragEnterEvent* event)     /*** Drag enter event ***/
{
    event->acceptProposedAction();
}

void Widget::dragMoveEvent(QDragMoveEvent* event)     /*** Drag move event ***/
{
    event->acceptProposedAction();
}

void Widget::dragLeaveEvent(QDragLeaveEvent* event)     /*** Drag leave event ***/
{
    event->accept();
}

void Widget::dropEvent(QDropEvent* event)     /*** Drag & Drop ***/
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls())
    {
        QStringList formats;
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();
        for (int i = 0; i < urlList.size() && i < 32; ++i)
        {
            pathList.append(urlList.at(i).toLocalFile());
            formats.append(QMimeDatabase().mimeTypeForFile(pathList.at(i)).name());
        }
        if (!formats.filter("audio/").empty() || !formats.filter("video/").empty())
        {
            openFiles(pathList);
            event->acceptProposedAction();
            return;
        }
    }
    event->ignore();
}

QString Widget::timeConverter(const float &time)     /*** Time converter to hh:mm:ss ***/
{    
    const int h = static_cast<int>(trunc(time / 3600));
    const int m = static_cast<int>(trunc((time - float(h * 3600)) / 60));
    const int s = static_cast<int>(round(time - float(h * 3600) - float(m * 60)));
    const QString hrs = QString::number(h).rightJustified(2, '0');
    const QString min = QString::number(m).rightJustified(2, '0');
    const QString sec = QString::number(s).rightJustified(2, '0');
    return QString("%1:%2:%3").arg(hrs, min, sec);
}

void Widget::on_comboBoxMode_currentIndexChanged(int index)
{
    _batch_mode = (index == 0) ? false : true;
}

void Widget::on_horizontalSlider_resize_valueChanged(int value)
{
    _rowSize = value;
    resizeTableRows(value);
}

/************************************************
** Preview Window
************************************************/

QString Widget::setThumbnail(QString curFilename,
                             const double &time,
                             const int &quality,
                             const int &destination)
{
    QString qualityParam("-vf scale=480:-1 -compression_level 10 -pix_fmt rgb24");
    if (quality == PreviewRes::RES_LOW) qualityParam = QString("-vf scale=144:-1,format=pal8,dctdnoiz=4.5");
    const QString time_qstr = QString::number(time, 'f', 3);
    const QString tmb_name = curFilename.replace(".", "_").replace(" ", "_") + time_qstr;
    QString tmb_file = _thumb_path + QString("/") + tmb_name + QString(".png");
    QFile tmb(tmb_file);
    if (!tmb.exists()) {
        QStringList cmd;
        cmd << "-hide_banner" << "-ss" << time_qstr << "-i" << _input_file
            << qualityParam.split(" ") << "-vframes" << "1" << "-y" << tmb_file;
        processThumbCreation->start("ffmpeg", cmd);
        processThumbCreation->waitForFinished();
    }
    if (!tmb.exists()) tmb_file = ":/resources/images/no_preview.png";
    preview_pixmap = QPixmap(tmb_file);
    QPixmap pix_scaled;
    if (destination == PreviewDest::PREVIEW) {
        pix_scaled = preview_pixmap.scaled(ui->frame_preview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labelThumb->setPixmap(pix_scaled);
    } else
    if (destination == PreviewDest::SPLITTER) {
        pix_scaled = preview_pixmap.scaled(ui->labelSplitPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labelSplitPreview->setPixmap(pix_scaled);
    }
    return tmb_file;
}

void Widget::repeatHandler_Type_2()
{
    //std::cout << "Call by timer... " << std::endl;
    if (_row != -1) setThumbnail(_curFilename, _curTime, PreviewRes::RES_HIGH, PreviewDest::SPLITTER);
}

/************************************************
** Source Window
************************************************/

void Widget::on_buttonHotInputFile_clicked()
{
    on_actionAdd_clicked();
}

/************************************************
** Output Window
************************************************/

void Widget::get_output_filename()  /*** Get output data ***/
{
    ui->textBrowser_2->clear();
    ui->textBrowser_2->setText(_cur_param[CurParamIndex::OUTPUT_PARAM]);
    QString file_without_ext("");
    QString suffix("");
    QString prefix("");
    int _CODEC = _cur_param[CurParamIndex::CODEC].toInt();
    int _CONTAINER = _cur_param[CurParamIndex::CONTAINER].toInt();
    extension = updateFieldContainer(_CODEC, _CONTAINER).toLower();

    if (_suffixType == 0) {
        QString row_qstr = QString::number(_row);
        std::ostringstream sstr;
        sstr << std::setw(4) << std::setfill('0') << row_qstr.toStdString();
        std::string counter = sstr.str();
        suffix = _suffixName + QString::fromStdString(counter);
    } else {
        QTime ct = QTime::currentTime();
        QDate cd = QDate::currentDate();
        QString ct_qstr = ct.toString();
        QString cd_qstr = cd.toString("MM.dd.yyyy");
        suffix = QString("_") + ct_qstr.remove(":") + QString("_") + cd_qstr.remove(".");
    }

    if (_prefxType == 0) {
        std::wstring file_name_wstr = _curFilename.toStdWString();
        std::wstring::size_type separator = file_name_wstr.rfind('.');
        if (separator != std::wstring::npos) {
            file_without_ext = QString::fromStdWString(file_name_wstr.substr(0, separator));
        } else {
            file_without_ext = _curFilename;
        }
        prefix = file_without_ext;
    } else {
        prefix = _prefixName;
    }

    QString _output_file_name = prefix + suffix + QString(".") + extension;
    ui->label_output->setText(_output_file_name);
    if (_output_folder == "") {
        _output_file = _curPath + QString("/") + _output_file_name;
    } else {
        _output_file = _output_folder + QString("/") + _output_file_name;
    }
    if (_temp_folder == "") {
        _temp_file = _curPath + QString("/temp.mkv");
    } else {
        _temp_file = _temp_folder + QString("/temp.mkv");
    }
}

void Widget::on_buttonHotOutputFile_clicked()
{
    const QString output_folder_name = callFileDialog(tr("Select output folder"));
    if (output_folder_name.isEmpty()) return;
    _output_folder = output_folder_name;
    if (_row != -1) get_output_filename();
}

QString Widget::callFileDialog(const QString title)  /*** Call file dialog ***/
{
    QFileDialog selectFolderWindow(nullptr);
    selectFolderWindow.setWindowTitle(title);
    selectFolderWindow.setMinimumWidth(600);
    selectFolderWindow.setWindowFlags(Qt::Dialog | Qt::SubWindow);
    if (_desktopEnv == "gnome") selectFolderWindow.setOption(QFileDialog::DontUseNativeDialog, true);
    selectFolderWindow.setFileMode(QFileDialog::DirectoryOnly);
    selectFolderWindow.setAcceptMode(QFileDialog::AcceptOpen);
    QString directory = QDir::homePath();
    if (_output_folder != "") directory = _output_folder;
    selectFolderWindow.setDirectory(directory);
    if (selectFolderWindow.exec() == QFileDialog::Accepted) {
        return selectFolderWindow.selectedFiles().at(0);
    }
    return QString("");
}

/************************************************
** Metadata Window
************************************************/

void Widget::on_lineEditTitleVideo_editingFinished()
{
    if (_row != -1) {
        if (!ui->lineEditTitleVideo->isModified()) {
            return;
        }
        ui->lineEditTitleVideo->setModified(false);
        QString videoTitle = ui->lineEditTitleVideo->text();
        QTableWidgetItem *newItem_videoTitle = new QTableWidgetItem(videoTitle);
        ui->tableWidget->setItem(_row, ColumnIndex::T_VIDEOTITLE, newItem_videoTitle);
        _videoMetadata[VIDEO_TITLE] = videoTitle;
    }
}

void Widget::on_lineEditAuthorVideo_editingFinished()
{
    if (_row != -1) {
        if (!ui->lineEditAuthorVideo->isModified()) {
            return;
        }
        ui->lineEditAuthorVideo->setModified(false);
        QString videoAuthor = ui->lineEditAuthorVideo->text();
        QTableWidgetItem *newItem_videoAuthor = new QTableWidgetItem(videoAuthor);
        ui->tableWidget->setItem(_row, ColumnIndex::T_VIDEOAUTHOR, newItem_videoAuthor);
        _videoMetadata[VIDEO_AUTHOR] = videoAuthor;
    }
}

void Widget::on_lineEditYearVideo_editingFinished()
{
    if (_row != -1) {
        if (!ui->lineEditYearVideo->isModified()) {
            return;
        }
        ui->lineEditYearVideo->setModified(false);
        QString videoYear = ui->lineEditYearVideo->text();
        QTableWidgetItem *newItem_videoYear = new QTableWidgetItem(videoYear);
        ui->tableWidget->setItem(_row, ColumnIndex::T_VIDEOYEAR, newItem_videoYear);
        _videoMetadata[VIDEO_YEAR] = videoYear;
    }
}

void Widget::on_lineEditPerfVideo_editingFinished()
{
    if (_row != -1) {
        if (!ui->lineEditPerfVideo->isModified()) {
            return;
        }
        ui->lineEditPerfVideo->setModified(false);
        QString videoPerf = ui->lineEditPerfVideo->text();
        QTableWidgetItem *newItem_videoPerf = new QTableWidgetItem(videoPerf);
        ui->tableWidget->setItem(_row, ColumnIndex::T_VIDEOPERF, newItem_videoPerf);
        _videoMetadata[VIDEO_PERFORMER] = videoPerf;
    }
}

void Widget::on_lineEditMovieNameVideo_editingFinished()
{
    if (_row != -1) {
        if (!ui->lineEditMovieNameVideo->isModified()) {
            return;
        }
        ui->lineEditMovieNameVideo->setModified(false);
        QString videoMovieName = ui->lineEditMovieNameVideo->text();
        QTableWidgetItem *newItem_videoMovieName = new QTableWidgetItem(videoMovieName);
        ui->tableWidget->setItem(_row, ColumnIndex::T_VIDEOMOVIENAME, newItem_videoMovieName);
        _videoMetadata[VIDEO_MOVIENAME] = videoMovieName;
    }
}

void Widget::on_lineEditDescriptionVideo_editingFinished()
{
    if (_row != -1) {
        if (!ui->lineEditDescriptionVideo->isModified()) {
            return;
        }
        ui->lineEditDescriptionVideo->setModified(false);
        QString videoDescription = ui->lineEditDescriptionVideo->text();
        QTableWidgetItem *newItem_videoDescription = new QTableWidgetItem(videoDescription);
        ui->tableWidget->setItem(_row, ColumnIndex::T_VIDEODESCR, newItem_videoDescription);
        _videoMetadata[VIDEO_DESCRIPTION] = videoDescription;
    }
}

void Widget::on_actionClearMetadata_clicked()
{
    if (_row != -1) {
        QList<QLineEdit*> linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit*>();
        foreach (QLineEdit *lineEdit, linesEditMetadata) {
            lineEdit->clear();
            lineEdit->insert("");
            lineEdit->setFocus();
            lineEdit->setModified(true);
        }
        ui->frame_middle->setFocus();
    }
}

void Widget::on_actionUndoMetadata_clicked()
{
    if (_row != -1) {
        QList<QLineEdit*> linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit*>();
        foreach (QLineEdit *lineEdit, linesEditMetadata) {
            lineEdit->undo();
            if (lineEdit->text() != "") {
                lineEdit->setFocus();
                lineEdit->setCursorPosition(0);
                lineEdit->setModified(true);
            }
        }
        ui->frame_middle->setFocus();
    }
}

/************************************************
** Streams Window
************************************************/

void Widget::on_actionClearAudioTitles_clicked()
{
    if (_row != -1) {
        for (int stream = 0; stream < AMOUNT_AUDIO_STREAMS; stream++) {
            QLineEdit *lineEditTitleAudio = ui->frameTab_2->findChild<QLineEdit *>("lineEditTitleAudio_"
                + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
            lineEditTitleAudio->clear();
            lineEditTitleAudio->insert("");
            lineEditTitleAudio->setFocus();
            lineEditTitleAudio->setModified(true);
        }
        ui->frame_middle->setFocus();
    }
}

void Widget::on_actionClearSubtitleTitles_clicked()
{
    if (_row != -1) {
        for (int stream = 0; stream < AMOUNT_SUBTITLES; stream++) {
            QLineEdit *lineEditTitleSubtitle = ui->frameTab_3->findChild<QLineEdit *>("lineEditTitleSubtitle_"
                + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
            lineEditTitleSubtitle->clear();
            lineEditTitleSubtitle->insert("");
            lineEditTitleSubtitle->setFocus();
            lineEditTitleSubtitle->setModified(true);
        }
        ui->frame_middle->setFocus();
    }
}

void Widget::on_actionUndoTitles_clicked()
{
    if (_row != -1) {
        for (int stream = 0; stream < AMOUNT_AUDIO_STREAMS; stream++) {
            QLineEdit *lineEditTitleAudio = ui->frameTab_2->findChild<QLineEdit *>("lineEditTitleAudio_"
                + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
            lineEditTitleAudio->undo();
            if (lineEditTitleAudio->text() != "") {
                lineEditTitleAudio->setFocus();
                lineEditTitleAudio->setCursorPosition(0);
                lineEditTitleAudio->setModified(true);
            }
        }
        for (int stream = 0; stream < AMOUNT_SUBTITLES; stream++) {
            QLineEdit *lineEditTitleSubtitle = ui->frameTab_3->findChild<QLineEdit *>("lineEditTitleSubtitle_"
                + QString::number(stream + 1), Qt::FindDirectChildrenOnly);
            lineEditTitleSubtitle->undo();
            if (lineEditTitleSubtitle->text() != "") {
                lineEditTitleSubtitle->setFocus();
                lineEditTitleSubtitle->setCursorPosition(0);
                lineEditTitleSubtitle->setModified(true);
            }
        }
        ui->frame_middle->setFocus();
    }
}

/************************************************
** Split Window
************************************************/

void Widget::on_horizontalSlider_valueChanged(int value)
{
    if (_row != -1) {
        double fps_double = _fps.toDouble();
        if (fps_double != 0.0) {
            _curTime = round(1000.0 * (static_cast<double>(value) / fps_double)) / 1000.0;
        } else {
            _curTime = 0.0;
        }
        ui->lineEditCurTime->setText(timeConverter(_curTime));
        timerCallSetThumbnail->stop();
        timerCallSetThumbnail->start();
    }
}

void Widget::on_buttonFramePrevious_clicked()
{
    int value = ui->horizontalSlider->value();
    if (value > 0) {
        ui->horizontalSlider->setValue(value - 1);
    }
}

void Widget::on_buttonFrameNext_clicked()
{
    int value = ui->horizontalSlider->value();
    if (value < _fr_count) {
        ui->horizontalSlider->setValue(value + 1);
    }
}

void Widget::on_buttonSetStartTime_clicked()
{
    if (_row != -1) {
        _startTime = _curTime;
        if (_startTime > _endTime && _endTime != 0.0) {
            _startTime = _endTime;
        }
        ui->lineEditStartTime->setText(timeConverter(_startTime));
        QTableWidgetItem *newItem_startTime = new QTableWidgetItem(QString::number(_startTime, 'f', 3));
        ui->tableWidget->setItem(_row, ColumnIndex::T_STARTTIME, newItem_startTime);
    }
}

void Widget::on_buttonSetEndTime_clicked()
{
    if (_row != -1) {
        _endTime = _curTime;
        if (_endTime < _startTime) {
            _endTime =_startTime;
        }
        ui->lineEditEndTime->setText(timeConverter(_endTime));
        QTableWidgetItem *newItem_endTime = new QTableWidgetItem(QString::number(_endTime, 'f', 3));
        ui->tableWidget->setItem(_row, ColumnIndex::T_ENDTIME, newItem_endTime);
    }
}

void Widget::on_actionResetLabels_clicked()
{
    if (_row != -1) {
        ui->labelSplitPreview->clear();
        ui->horizontalSlider->blockSignals(true);
        ui->horizontalSlider->setValue(0);
        ui->horizontalSlider->blockSignals(false);

        _curTime = 0.0;
        ui->lineEditCurTime->setText(timeConverter(_curTime));

        _startTime = 0.0;
        ui->lineEditStartTime->setText(timeConverter(_startTime));
        QTableWidgetItem *newItem_startTime = new QTableWidgetItem(QString::number(_startTime, 'f', 3));
        ui->tableWidget->setItem(_row, ColumnIndex::T_STARTTIME, newItem_startTime);

        _endTime = 0.0;
        ui->lineEditEndTime->setText(timeConverter(_endTime));
        QTableWidgetItem *newItem_endTime = new QTableWidgetItem(QString::number(_endTime, 'f', 3));
        ui->tableWidget->setItem(_row, ColumnIndex::T_ENDTIME, newItem_endTime);
    }
}

QString Widget::timeConverter(double &time)     /*** Time converter to hh:mm:ss.msc ***/
{
    const int h = static_cast<int>(trunc(time / 3600));
    const int m = static_cast<int>(trunc((time - double(h * 3600)) / 60));
    const int s = static_cast<int>(trunc(time - double(h * 3600) - double(m * 60)));
    const int ms = static_cast<int>(round(1000 * (time - double(h * 3600) - double(m * 60) - double(s))));
    const QString hrs = QString::number(h).rightJustified(2, '0');
    const QString min = QString::number(m).rightJustified(2, '0');
    const QString sec = QString::number(s).rightJustified(2, '0');
    const QString msec = QString::number(ms).rightJustified(3, '0');
    return QString("%1:%2:%3.%4").arg(hrs, min, sec, msec);
}

/************************************************
** Preset Window
************************************************/

void Widget::set_defaults() /*** Set default presets ***/
{
    std::cout<< "Set defaults..." << std::endl;
    QFile _prs_file(":/resources/data/default_presets.ini");
    if (_prs_file.open(QIODevice::ReadOnly)) {
        QDataStream in(&_prs_file);
        in.setVersion(QDataStream::Qt_4_0);
        int ver;
        in >> ver;
        if (ver == PRESETS_VERSION) {
            in >> _cur_param >> _pos_top >> _pos_cld >> _preset_table;
        }
        _prs_file.close();
    }
}

void Widget::on_buttonApplyPreset_clicked()  /*** Apply preset ***/
{
    int index = ui->treeWidget->currentIndex().row();
    if (index < 0) {
        _message = tr("Select preset first!\n");
        call_task_complete(_message, false);
        return;
    }
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem != nullptr) {
        // Item is child...
        for (int k = 0; k < PARAMETERS_COUNT; k++) {
            _cur_param[k] = item->text(k+7);
        }

    } else {
        // Item is parent...
        _message = tr("Select preset first!\n");
        call_task_complete(_message, false);
        return;
    }
    _pos_top = ui->treeWidget->indexOfTopLevelItem(parentItem);
    _pos_cld = parentItem->indexOfChild(item);
    std::cout << "Pos_top: " << _pos_top << std::endl;  // Current section pos
    std::cout << "Pos_cld: " << _pos_cld << std::endl;  // Current preset pos
    if (_row != -1) {
        get_output_filename();
    }
}

void Widget::on_actionRemove_preset_clicked()  /*** Remove preset ***/
{
    int index = ui->treeWidget->currentIndex().row();
    if (index < 0) {
        return;
    }
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem != nullptr) {
        // Item is child...
        _message = tr("Delete?");
        bool confirm = call_dialog(_message);
        if (confirm == true)
        {
            int index_top = ui->treeWidget->indexOfTopLevelItem(parentItem);
            int index_child = parentItem->indexOfChild(item);
            updateCurPresetPos(index_top, index_child);

            QTreeWidgetItem *takenItem = parentItem->takeChild(index_child);
            Q_ASSERT(takenItem == item);
            delete takenItem;
            updatePresetTable();
        }

    } else {
        // Item is parent...
        int count_child = item->childCount();
        if (count_child == 0) {
            int index_top = ui->treeWidget->indexOfTopLevelItem(item);
            int index_child = -1;
            updateCurPresetPos(index_top, index_child);

            QTreeWidgetItem *takenItem = ui->treeWidget->takeTopLevelItem(index_top);
            Q_ASSERT(takenItem==item);
            delete takenItem;
            updatePresetTable();

        } else {
            _message = tr("Delete presets first!\n");
            call_task_complete(_message, false);
        }
    }
}

void Widget::on_actionEdit_preset_clicked()  /*** Edit preset ***/
{
    int index = ui->treeWidget->currentIndex().row();
    if (index < 0) {
        _message = tr("Select preset first!\n");
        call_task_complete(_message, false);
        return;
    }
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem != nullptr) {
        // Item is child...
        for (int k = 0; k < PARAMETERS_COUNT; k++) {
            _new_param[k] = item->text(k+7);
        };
        Preset presetWindow(this);
        presetWindow.setParameters(&_presetWindowGeometry, &_new_param);
        presetWindow.setModal(true);
        if (presetWindow.exec() == QDialog::Accepted) {
            for (int k = 0; k < PARAMETERS_COUNT; k++) {
                item->setText(k+7, _new_param[k]);
            }
            updateInfoFields(_new_param[1],
                             _new_param[2],
                             _new_param[3],
                             _new_param[4],
                             _new_param[11],
                             _new_param[12],
                             _new_param[21],
                             item, true);
            int index_top = ui->treeWidget->indexOfTopLevelItem(parentItem);
            int index_child = parentItem->indexOfChild(item);
            if (_pos_top == index_top && _pos_cld == index_child) {
                for (int k = 0; k < PARAMETERS_COUNT; k++) {
                    _cur_param[k] = item->text(k+7);
                }
                if (_row != -1) {
                    get_output_filename();
                }
            }
            updatePresetTable();
        }
    } else {
        // Item is parent...
        _message = tr("Select preset first!\n");
        call_task_complete(_message, false);
    }
}

void Widget::add_section()  /*** Add section ***/
{
    QFont parentFont;
    parentFont.setBold(true);
    QTreeWidgetItem *root = new QTreeWidgetItem();
    root->setText(0, "New section");
    root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    root->setFont(0, parentFont);
    setPresetIcon(root, true);
    ui->treeWidget->addTopLevelItem(root);
    ui->treeWidget->setCurrentItem(root);
    root->setFirstColumnSpanned(true);
    updatePresetTable();
}

void Widget::add_preset()  /*** Add preset ***/
{
    int index = ui->treeWidget->currentIndex().row();
    if (index < 0) {
        _message = tr("First add a section!\n");
        call_task_complete(_message, false);
        return;
    }

    QVector<QString> cur_param;
    QFile _prs_file(":/resources/data/default_presets.ini");
    if (_prs_file.open(QIODevice::ReadOnly)) {
        QDataStream in(&_prs_file);
        in.setVersion(QDataStream::Qt_4_0);
        int ver;
        in >> ver;
        if (ver == PRESETS_VERSION) {
            in >> cur_param;
            QTreeWidgetItem *item = ui->treeWidget->currentItem();
            QTreeWidgetItem *parentItem = item->parent();
            QTreeWidgetItem *child = new QTreeWidgetItem();
            for (int k = 0; k < PARAMETERS_COUNT; k++) {
                child->setText(k + 7, cur_param[k]);
            }
            updateInfoFields(cur_param[1], cur_param[2], cur_param[3], cur_param[4],
                             cur_param[11], cur_param[12], cur_param[21], child, true);
            setItemStyle(child);
            if (parentItem != nullptr) {
                // Item is child...
                parentItem->addChild(child);

                int index_top = ui->treeWidget->indexOfTopLevelItem(parentItem);
                int index_child = parentItem->indexOfChild(child);
                updateCurPresetPos(index_top, index_child);
            } else {
                // Item is parent...
                item->addChild(child);
                ui->treeWidget->expandItem(item);

                int index_top = ui->treeWidget->indexOfTopLevelItem(item);
                int index_child = item->indexOfChild(child);
                updateCurPresetPos(index_top, index_child);
            }
            updatePresetTable();
        }
        _prs_file.close();
    }
}

void Widget::renameSectionPreset()
{
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem != nullptr) {
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        ui->treeWidget->editItem(item, 0);
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    } else {
        ui->treeWidget->editItem(item, 0);
    }
}

void Widget::setItemStyle(QTreeWidgetItem *item)
{
    QFont font = qApp->font();
    font.setItalic(true);
    QColor foregroundChildColor;
    switch (_theme)
    {
        case 0:
        case 1:
        case 2:
            foregroundChildColor.setRgb(qRgb(50, 100, 157));
            break;
        case 3:
            foregroundChildColor.setRgb(qRgb(30, 50, 150));
            break;
    }
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
    item->setForeground(0, foregroundChildColor);
    item->setFont(0, font);
    for (int column = 1; column < 7; column++) {
        item->setTextAlignment(column, Qt::AlignCenter);
        item->setForeground(column, foregroundChildColor);
        item->setFont(column, font);
    }
}

void Widget::updateCurPresetPos(int &index_top, int &index_child)
{
    std::cout << "Pos top: " << _pos_top << " Pos child: " << _pos_cld
              << " >>>>>> Index top: " << index_top << " Index child: " << index_child << std::endl;
    //if (index_child == _pos_cld) {
    _pos_top = -1;
    _pos_cld = -1;
    _cur_param[CurParamIndex::OUTPUT_PARAM] = tr("Preset not selected");
    if (_row != -1) {
        get_output_filename();
    }
    //}
}

void Widget::updateInfoFields(QString &codec_qstr,
                              QString &mode_qstr,
                              QString &container_qstr,
                              QString &bqr_qstr,
                              QString &pass_qstr,
                              QString &preset_qstr,
                              QString &acodec_qstr,
                              QTreeWidgetItem *item,
                              bool defaultNameFlag)
{
    int codec = codec_qstr.toInt();
    int mode = mode_qstr.toInt();
    int preset = preset_qstr.toInt();
    int pass = pass_qstr.toInt();
    int acodec = acodec_qstr.toInt();
    int container = container_qstr.toInt();
    if (defaultNameFlag) {
        item->setText(0, updateFieldCodec(codec));
        QString newPresetName = item->text(0);
        item->setText(30 + 7, newPresetName);
    }
    item->setText(1, updateFieldMode(codec, mode));
    item->setText(2, bqr_qstr);
    item->setText(3, updateFieldPreset(codec, preset));
    item->setText(4, updateFieldPass(codec, pass));
    item->setText(5, updateFieldAcodec(codec, acodec));
    item->setText(6, updateFieldContainer(codec, container));
}

void Widget::updatePresetTable()
{
    int TOP_LEVEL_ITEMS_COUNT = ui->treeWidget->topLevelItemCount();
    int CHILD_COUNT = 0;
    for (int i = 0; i < TOP_LEVEL_ITEMS_COUNT; i++) {
        CHILD_COUNT += ui->treeWidget->topLevelItem(i)->childCount();
    }
    int ROWS_COUNT = TOP_LEVEL_ITEMS_COUNT + CHILD_COUNT;  // Count of all rows...
    /****************************************************/
    for (int i = 0; i < PARAMETERS_COUNT+1; i++) {
      _preset_table[i].resize(ROWS_COUNT);
    }
    /****************************************************/
    int row = 0;
    for (int top = 0; top < TOP_LEVEL_ITEMS_COUNT; top++) {
        _preset_table[0][row] = ui->treeWidget->topLevelItem(top)->text(0);
        _preset_table[PARAMETERS_COUNT][row] = "TopLewelItem";

        CHILD_COUNT = ui->treeWidget->topLevelItem(top)->childCount();
        for (int child = 0; child < CHILD_COUNT; child++) {
            row++;
            for (int column = 0; column < PARAMETERS_COUNT; column++) {
                _preset_table[column][row] = ui->treeWidget->topLevelItem(top)->child(child)->text(column+7);
            }
            _preset_table[PARAMETERS_COUNT][row] = "ChildItem";
        }
        row++;
    }
    /****************************************************/
    std::cout << _preset_table[0].size() << " x " << _preset_table.size() << std::endl; // Table size
}

QString Widget::updateFieldCodec(int &codec)
{
    QString arr_codec[NUMBER_PRESETS] = {
        tr("H.265/HEVC 4:2:0 12 bit"),
        tr("H.265/HEVC 4:2:0 10 bit"),
        tr("H.265/HEVC 4:2:0 8 bit"),
        tr("H.264/AVC 4:2:0 8 bit"),
        tr("VP9 4:2:0 10 bit"),
        tr("VP9 4:2:0 8 bit"),
        tr("Intel QSV H.265/HEVC 4:2:0 10 bit"),
        tr("Intel QSV H.265/HEVC 4:2:0 8 bit"),
        tr("Intel QSV H.264/AVC 4:2:0 8 bit"),
        tr("Intel QSV VP9 4:2:0 10 bit"),
        tr("Intel QSV VP9 4:2:0 8 bit"),
        tr("Intel QSV MPEG-2 4:2:0 8 bit"),
        tr("NVENC H.265/HEVC 4:2:0 10 bit"),
        tr("NVENC H.265/HEVC 4:2:0 8 bit"),
        tr("NVENC H.264/AVC 4:2:0 8 bit"),
        tr("ProRes Proxy"),
        "ProRes LT",
        tr("ProRes Standard"),
        "ProRes HQ",
        "ProRes 4444",
        "ProRes 4444XQ",
        "DNxHR LB",
        "DNxHR SQ",
        "DNxHR HQ",
        "DNxHR HQX",
        "DNxHR 444",
        "XDCAM HD422",
        "XAVC 4:2:2",
        tr("From source")
    };
    return arr_codec[codec];
}

QString Widget::updateFieldMode(int &codec, int &mode)
{
    QString arr_mode[NUMBER_PRESETS][5] = {
        {"CBR",      "ABR", "VBR", "CRF", "CQP"},
        {"CBR",      "ABR", "VBR", "CRF", "CQP"},
        {"CBR",      "ABR", "VBR", "CRF", "CQP"},
        {"CBR",      "ABR", "VBR", "CRF", "CQP"},
        {"ABR",      "CRF", "",    "",    ""},
        {"ABR",      "CRF", "",    "",    ""},
        {"VBR",      "",    "",    "",    ""},
        {"VBR",      "",    "",    "",    ""},
        {"VBR",      "",    "",    "",    ""},
        {"ABR",      "CRF", "",    "",    ""},
        {"ABR",      "CRF", "",    "",    ""},
        {"VBR",      "",    "",    "",    ""},
        {"VBR",      "",    "",    "",    ""},
        {"VBR",      "",    "",    "",    ""},
        {"VBR",      "",    "",    "",    ""},
        {tr("Auto"), "",    "",    "",    ""},
        {tr("Auto"), "",    "",    "",    ""},
        {tr("Auto"), "",    "",    "",    ""},
        {tr("Auto"), "",    "",    "",    ""},
        {tr("Auto"), "",    "",    "",    ""},
        {tr("Auto"), "",    "",    "",    ""},
        {tr("Auto"), "",    "",    "",    ""},
        {tr("Auto"), "",    "",    "",    ""},
        {tr("Auto"), "",    "",    "",    ""},
        {tr("Auto"), "",    "",    "",    ""},
        {tr("Auto"), "",    "",    "",    ""},
        {"VBR",      "",    "",    "",    ""},
        {"CBR",      "",    "",    "",    ""},
        {tr("Auto"), "",    "",    "",    ""}
    };
    return arr_mode[codec][mode];
}

QString Widget::updateFieldPreset(int &codec, int &preset)
{
    QString arr_preset[NUMBER_PRESETS][10] = {
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Veryfast"),  tr("Faster"),    tr("Fast"),     tr("Medium"), tr("Slow"), tr("Slower"), tr("Veryslow"), "",           ""},
        {tr("None"), tr("Veryfast"),  tr("Faster"),    tr("Fast"),     tr("Medium"), tr("Slow"), tr("Slower"), tr("Veryslow"), "",           ""},
        {tr("None"), tr("Veryfast"),  tr("Faster"),    tr("Fast"),     tr("Medium"), tr("Slow"), tr("Slower"), tr("Veryslow"), "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Veryfast"),  tr("Faster"),    tr("Fast"),     tr("Medium"), tr("Slow"), tr("Slower"), tr("Veryslow"), "",           ""},
        {tr("None"), tr("Slow"),      "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Slow"),      "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Slow"),      "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""},
        {tr("None"), tr("Ultrafast"), tr("Superfast"), tr("Veryfast"), tr("Faster"), tr("Fast"), tr("Medium"), tr("Slow"),     tr("Slower"), tr("Veryslow")},
        {tr("None"), "",              "",              "",             "",           "",         "",           "",             "",           ""}
    };
    return arr_preset[codec][preset];
}

QString Widget::updateFieldPass(int &codec, int &pass)
{
    QString arr_pass[NUMBER_PRESETS][2] = {
        {tr("1 Pass"), tr("2 Pass")},
        {tr("1 Pass"), tr("2 Pass")},
        {tr("1 Pass"), tr("2 Pass")},
        {tr("1 Pass"), tr("2 Pass")},
        {tr("1 Pass"), tr("2 Pass")},
        {tr("1 Pass"), tr("2 Pass")},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("2 Pass"), ""},
        {tr("2 Pass"), ""},
        {tr("2 Pass"), ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""},
        {tr("Auto"),   ""}
    };
    return arr_pass[codec][pass];
}

QString Widget::updateFieldAcodec(int &codec, int &acodec)
{
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
    return arr_acodec[codec][acodec];
}

QString Widget::updateFieldContainer(int &codec, int &container)
{
    QString arr_container[NUMBER_PRESETS][5] = {
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"WebM", "MKV", "",    "",     ""},
        {"WebM", "MKV", "",    "",     ""},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"WebM", "MKV", "",    "",     ""},
        {"WebM", "MKV", "",    "",     ""},
        {"MKV",  "MPG", "AVI", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MOV",  "",    "",    "",     ""},
        {"MXF",  "",    "",    "",     ""},
        {"MXF",  "",    "",    "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"}
    };
    return arr_container[codec][container];
}

void Widget::setPresetIcon(QTreeWidgetItem *item, bool collapsed)
{
    QIcon sectionIcon;
    QString path;
    switch (_theme)
    {
        case Theme::GRAY:
        case Theme::DARK:
        case Theme::WAVE:
            if (collapsed) {
                path = QString::fromUtf8(":/resources/icons/16x16/cil-folder.png");
            } else {
                path = QString::fromUtf8(":/resources/icons/16x16/cil-folder-open.png");}
            break;
        case Theme::DEFAULT:
            if (collapsed) {
                path = QString::fromUtf8(":/resources/icons/16x16/cil-folder_light.png");
            } else {
                path = QString::fromUtf8(":/resources/icons/16x16/cil-folder-open_light.png");}
            break;
    }
    sectionIcon.addFile(path, QSize(), QIcon::Normal, QIcon::Off);
    item->setIcon(0, sectionIcon);
}

void Widget::on_treeWidget_itemCollapsed(QTreeWidgetItem *item)
{
    if (item != nullptr) {
        setPresetIcon(item, true);
    }
}

void Widget::on_treeWidget_itemExpanded(QTreeWidgetItem *item)
{
    if (item != nullptr) {
        setPresetIcon(item, false);
    }
}

void Widget::on_treeWidget_itemChanged(QTreeWidgetItem *item, int column)
{
    if (item->isSelected() && column == 0) {
        QTreeWidgetItem *parentItem = item->parent();
        if (parentItem != nullptr) {
            QString newPresetName = item->text(0);
            item->setText(30 + 7, newPresetName);
        }
        updatePresetTable();
    }
}

void Widget::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem != nullptr) {
        on_buttonApplyPreset_clicked();
        std::cout << "Double clicked column: " << column << std::endl;
    }
}

void Widget::providePresetContextMenu(const QPoint &position)     /*** Call tree items menu  ***/
{
    QTreeWidgetItem *item = ui->treeWidget->itemAt(position);
    if (item != nullptr) {
        QTreeWidgetItem *parentItem = item->parent();
        if (parentItem != nullptr) {
            presetMenu->exec(ui->treeWidget->mapToGlobal(QPoint(position.x(), position.y() + 35)));
        } else {
            sectionMenu->exec(ui->treeWidget->mapToGlobal(QPoint(position.x(), position.y() + 35)));
        }
    }
}

/************************************************
** Dialogs Windows
************************************************/

bool Widget::call_dialog(const QString &_message)  /*** Call dialog ***/
{
    Dialog dialog(this);
    dialog.setMessage(_message);
    dialog.setModal(true);
    if (dialog.exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

void Widget::call_task_complete(const QString &_message, const bool &_timer_mode)  /*** Call task complete ***/
{
    if (this->isHidden()) {
        if (_hideInTrayFlag && !_timer_mode) {
            trayIcon->showMessage(_message, tr("Task"), QSystemTrayIcon::Information, 151000);
        }
        else if (_timer_mode) {
            this->show();
            Taskcomplete taskcomplete(this);
            taskcomplete.setMessage(_message, _timer_mode);
            taskcomplete.setModal(true);
            taskcomplete.exec();
        }
    } else {
        Taskcomplete taskcomplete(this);
        taskcomplete.setMessage(_message, _timer_mode);
        taskcomplete.setModal(true);
        taskcomplete.exec();
    }
}
