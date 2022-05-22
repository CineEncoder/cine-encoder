/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: mainwindow.cpp
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "notification.h"
#include "settings.h"
#include "preset.h"
#include "message.h"
#include "progress.h"
#include "tables.h"
#include "helper.h"
#include <QDesktopWidget>
#include <QPaintEvent>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QMimeDatabase>
#include <QMimeData>
#include <QTableWidgetItem>
#include <QListView>
#include <QUrl>
#include <QList>
#include <QMenu>
#include <QDate>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QDockWidget>
#include <QFile>
#include <QSizePolicy>
#include <QMap>
#include <QTranslator>
#include <iostream>
#include <iomanip>
#include <sstream>


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

#define WINDOW_SIZE QSize(1024, 650)


MainWindow::MainWindow(QWidget *parent):
    BaseWindow(parent),
    ui(new Ui::Widget),
    _windowActivated(false),
    _expandWindowsState(false)
{
    QWidget *ui_widget = new QWidget(this);
    setCentralWidget(ui_widget);
    ui->setupUi(ui_widget);
    setTitleBar(ui->frame_top);
#ifdef Q_OS_UNIX
    setMaskWidget(ui_widget);
#endif

    // **************************** Set front label ***********************************//
    QHBoxLayout *raiseLayout = new QHBoxLayout(ui->tableWidget);
    ui->tableWidget->setLayout(raiseLayout);
    raiseThumb = new QLabel(ui->tableWidget);
    raiseLayout->addWidget(raiseThumb);
    raiseThumb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    raiseThumb->setObjectName(QString::fromUtf8("TableWidgetLabel"));
    raiseThumb->setAlignment(Qt::AlignCenter);
    raiseThumb->setText(tr("No media"));

    audioThumb = new QLabel(ui->frameTab_2);
    ui->gridLayoutAudio->addWidget(audioThumb);
    audioThumb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    audioThumb->setObjectName(QString::fromUtf8("AudioLabel"));
    audioThumb->setAlignment(Qt::AlignCenter);
    audioThumb->setText(tr("No audio"));

    subtitleThumb = new QLabel(ui->frameTab_3);
    ui->gridLayoutSubtitle->addWidget(subtitleThumb);
    subtitleThumb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    subtitleThumb->setObjectName(QString::fromUtf8("SubtitleLabel"));
    subtitleThumb->setAlignment(Qt::AlignCenter);
    subtitleThumb->setText(tr("No subtitle"));

    // **************************** Create docks ***********************************//
    QGridLayout *layout = new QGridLayout(ui->frame_middle);
    ui->frame_middle->setLayout(layout);
    window = new QMainWindow(ui->frame_middle);
    layout->addWidget(window);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setVerticalSpacing(0);
    layout->setHorizontalSpacing(0);
    window->setObjectName("CentralWindow");
    window->setWindowFlags(Qt::Widget);
    window->setDockNestingEnabled(true);

    centralWidget = new QWidget(window);
    centralWidget->setObjectName("centralwidget");
    window->setCentralWidget(centralWidget);

    /*window->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    window->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    window->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    window->setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);*/

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
    raiseThumb->installEventFilter(this);
    ui->labelThumb->installEventFilter(this);
    ui->frame_middle->setFocusPolicy(Qt::StrongFocus);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    BaseWindow::showEvent(event);
    if (!_windowActivated) {
        _windowActivated = true;
        setParameters();
    }
}

void MainWindow::closeEvent(QCloseEvent *event) // Show prompt when close app
{
    event->ignore();
    if (showDialogMessage(tr("Quit program?"))) {

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

        SETTINGS(_settings);
        // Save Version
        _settings.setValue("Version", SETTINGS_VERSION);

        // Save Main Widget
        _settings.beginGroup("MainWidget");
        _settings.setValue("MainWidget/geometry", this->saveGeometry());
        _settings.endGroup();

        // Save Main Window
        _settings.beginGroup("MainWindow");
        _settings.setValue("MainWindow/state", window->saveState());
        _settings.setValue("MainWindow/geometry", window->saveGeometry());
        _settings.beginWriteArray("MainWindow/docks_geometry");
            for (int ind = 0; ind < DOCKS_COUNT; ind++) {
                _settings.setArrayIndex(ind);
                _settings.setValue("MainWindow/docks_geometry/dock_size", docks[ind]->size());
            }
            _settings.endArray();
        _settings.endGroup();

        // Save Tables
        _settings.beginGroup("Tables");
        _settings.setValue("Tables/table_widget_state", ui->tableWidget->horizontalHeader()->saveState());
        _settings.setValue("Tables/tree_widget_state", ui->treeWidget->header()->saveState());
        _settings.endGroup();

        // Save Settings
        _settings.beginGroup("Settings");
        _settings.setValue("Settings/prefix_type", _prefxType);
        _settings.setValue("Settings/suffix_type", _suffixType);
        _settings.setValue("Settings/prefix_name", _prefixName);
        _settings.setValue("Settings/suffix_name", _suffixName);
        _settings.setValue("Settings/timer_interval", _timer_interval);
        _settings.setValue("Settings/theme", _theme);
        _settings.setValue("Settings/protection", _protection);
        _settings.setValue("Settings/show_hdr_mode", _showHDR_mode);
        _settings.setValue("Settings/temp_folder", _temp_folder);
        _settings.setValue("Settings/output_folder", _output_folder);
        _settings.setValue("Settings/open_dir", _openDir);
        _settings.setValue("Settings/batch_mode", _batch_mode);
        _settings.setValue("Settings/tray", _hideInTrayFlag);
        _settings.setValue("Settings/language", _language);
        _settings.setValue("Settings/font", _font);
        _settings.setValue("Settings/font_size", _fontSize);
        _settings.setValue("Settings/row_size", _rowSize);
        _settings.endGroup();

        if (_hideInTrayFlag) {
            trayIcon->hide();
        }
        event->accept();
    }
}

void MainWindow::paintEvent(QPaintEvent *event) // Disable QTab draw base
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

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick: {
        _expandWindowsState ? showMaximized() : showNormal();
        break;
    }
    default:
        break;
    }
}

void MainWindow::setTrayIconActions()
{
    minimizeAction = new QAction(tr("Hide"), this);
    restoreAction = new QAction(tr("Show"), this);
    quitAction = new QAction(tr("Exit"), this);

    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect(restoreAction, &QAction::triggered, this, [this]() {
        _expandWindowsState ? showMaximized() : showNormal();
    });
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));

    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addAction(quitAction);
}

void MainWindow::showTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(QPixmap(":/resources/icons/svg/cine-encoder.svg")));
    trayIcon->setContextMenu(trayIconMenu);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));
}

void MainWindow::createConnections()
{
    connect(ui->closeWindow, &QPushButton::clicked, this, &MainWindow::onCloseWindow);
    connect(ui->hideWindow, &QPushButton::clicked, this, &MainWindow::onHideWindow);
    connect(ui->expandWindow, &QPushButton::clicked, this, &MainWindow::onExpandWindow);
    connect(ui->actionAdd, &QPushButton::clicked, this, &MainWindow::onActionAdd);
    connect(ui->actionRemove, &QPushButton::clicked, this, &MainWindow::onActionRemove);
    connect(ui->buttonSortUp, &QPushButton::clicked, this, &MainWindow::onSortUp);
    connect(ui->buttonSortDown, &QPushButton::clicked, this, &MainWindow::onSortDown);
    connect(ui->actionStop, &QPushButton::clicked, this, &MainWindow::onActionStop);
    connect(ui->actionEncode, &QPushButton::clicked, this, &MainWindow::onActionEncode);
    connect(ui->actionSettings, &QPushButton::clicked, this, &MainWindow::onActionSettings);

    connect(ui->tableWidget, &QTableWidget::itemSelectionChanged, this, &MainWindow::onTableWidget_itemSelectionChanged);

    QLineEdit *lineEditVideoMetadata[6] = {
        ui->lineEditTitleVideo, ui->lineEditMovieNameVideo, ui->lineEditYearVideo,
        ui->lineEditAuthorVideo, ui->lineEditPerfVideo, ui->lineEditDescriptionVideo
    };
    for (int i = VIDEO_TITLE; i <= VIDEO_DESCRIPTION; i++) {
        Q_ASSERT(i < 6);
        connect(lineEditVideoMetadata[i], &QLineEdit::editingFinished, this, [=](){
            if (_row != -1) {
                if (!lineEditVideoMetadata[i]->isModified()) {
                    return;
                }
                lineEditVideoMetadata[i]->setModified(false);
                QString text = lineEditVideoMetadata[i]->text();
                QTableWidgetItem *__item = new QTableWidgetItem(text);
                ui->tableWidget->setItem(_row, ColumnIndex::T_VIDEOTITLE + i, __item);
                _videoMetadata[VIDEO_TITLE + i] = text;
            }
        });
    }

    connect(ui->actionClearMetadata, &QPushButton::clicked, this, &MainWindow::onActionClearMetadata);
    connect(ui->actionUndoMetadata, &QPushButton::clicked, this, &MainWindow::onActionUndoMetadata);
    connect(ui->actionClearAudioTitles, &QPushButton::clicked, this, &MainWindow::onActionClearAudioTitles);
    connect(ui->actionClearSubtitleTitles, &QPushButton::clicked, this, &MainWindow::onActionClearSubtitleTitles);
    connect(ui->actionUndoTitles, &QPushButton::clicked, this, &MainWindow::onActionUndoTitles);

    connect(ui->horizontalSlider, &QSlider::valueChanged, this, &MainWindow::onSplitSlider_valueChanged);
    connect(ui->buttonFramePrevious, &QPushButton::clicked, this, &MainWindow::onButtonFramePrevious);
    connect(ui->buttonFrameNext, &QPushButton::clicked, this, &MainWindow::onButtonFrameNext);
    connect(ui->buttonSetStartTime, &QPushButton::clicked, this, &MainWindow::onButtonSetStartTime);
    connect(ui->buttonSetEndTime, &QPushButton::clicked, this, &MainWindow::onButtonSetEndTime);

    connect(ui->actionRemove_preset, &QPushButton::clicked, this, &MainWindow::onActionRemovePreset);
    connect(ui->actionEdit_preset, &QPushButton::clicked, this, &MainWindow::onActionEditPreset);
    connect(ui->buttonApplyPreset, &QPushButton::clicked, this, &MainWindow::onApplyPreset);
    connect(ui->treeWidget, &QTreeWidget::itemCollapsed, this, &MainWindow::onTreeWidget_itemCollapsed);
    connect(ui->treeWidget, &QTreeWidget::itemExpanded, this, &MainWindow::onTreeWidget_itemExpanded);
    connect(ui->treeWidget, &QTreeWidget::itemChanged, this, &MainWindow::onTreeWidget_itemChanged);
    connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, &MainWindow::onTreeWidget_itemDoubleClicked);

    connect(ui->horizontalSlider_resize, &QSlider::valueChanged, this, &MainWindow::onIconResizeSlider_valueChanged);
    connect(ui->buttonHotInputFile, &QPushButton::clicked, this, &MainWindow::onButtonHotInputFile);
    connect(ui->buttonHotOutputFile, &QPushButton::clicked, this, &MainWindow::onButtonHotOutputFile);
    connect(ui->buttonCloseTaskWindow, &QPushButton::clicked, this, &MainWindow::onButtonCloseTaskWindow);

    connect(ui->comboBoxMode, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxMode_currentIndexChanged(int)));
    connect(ui->actionResetLabels, &QPushButton::clicked, this, &MainWindow::onActionResetLabels);

    encoder = new Encoder(this);
    connect(encoder, &Encoder::onEncodingMode, this, &MainWindow::onEncodingMode);
    connect(encoder, &Encoder::onEncodingStarted, this, &MainWindow::onEncodingStarted);
    connect(encoder, &Encoder::onEncodingInitError, this, &MainWindow::onEncodingInitError);
    connect(encoder, &Encoder::onEncodingProgress, this, &MainWindow::onEncodingProgress);
    connect(encoder, &Encoder::onEncodingLog, this, &MainWindow::onEncodingLog);
    connect(encoder, &Encoder::onEncodingCompleted, this, &MainWindow::onEncodingCompleted);
    connect(encoder, &Encoder::onEncodingAborted, this, &MainWindow::onEncodingAborted);
    connect(encoder, &Encoder::onEncodingError, this, &MainWindow::onEncodingError);

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
    connect(add_files, &QAction::triggered, this, &MainWindow::onActionAdd);
    connect(remove_files, &QAction::triggered, this, &MainWindow::onActionRemove);
    connect(close_prog, &QAction::triggered, this, &MainWindow::onCloseWindow);

    encode_files = new QAction(tr("Encode/Pause"), this);
    stop_encode = new QAction(tr("Stop"), this);
    connect(encode_files, &QAction::triggered, this, &MainWindow::onActionEncode);
    connect(stop_encode, &QAction::triggered, this, &MainWindow::onActionStop);

    edit_metadata = new QAction(tr("Edit metadata"), this);
    select_audio = new QAction(tr("Select audio streams"), this);
    select_subtitles = new QAction(tr("Select subtitles"), this);
    split_video = new QAction(tr("Split video"), this);
    connect(edit_metadata, &QAction::triggered, this, &MainWindow::showMetadataEditor);
    connect(select_audio, &QAction::triggered, this, &MainWindow::showAudioStreamsSelection);
    connect(select_subtitles, &QAction::triggered, this, &MainWindow::showSubtitlesSelection);
    connect(split_video, &QAction::triggered, this, &MainWindow::showVideoSplitter);

    settings = new QAction(tr("Settings"), this);
    connect(settings, &QAction::triggered, this, &MainWindow::onActionSettings);

    reset_view = new QAction(tr("Reset state"), this);
    connect(reset_view, &QAction::triggered, this, &MainWindow::resetView);

    about = new QAction(tr("About"), this);
    donate = new QAction(tr("Donate"), this);
    connect(about, &QAction::triggered, this, &MainWindow::onActionAbout);
    connect(donate, &QAction::triggered, this, &MainWindow::onActionDonate);

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
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &MainWindow::provideContextMenu);

    // ***************************** Tree menu actions ***********************************//

    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QAction* add_section = new QAction(tr("Add section"), this);
    QAction* add_preset = new QAction(tr("Add preset"), this);
    QAction* rename_section_preset = new QAction(tr("Rename"), this);
    QAction* remove_section_preset = new QAction(tr("Remove"), this);
    connect(add_section, &QAction::triggered, this, &MainWindow::add_section);
    connect(add_preset, &QAction::triggered, this, &MainWindow::add_preset);
    connect(rename_section_preset, &QAction::triggered, this, &MainWindow::renameSectionPreset);
    connect(remove_section_preset, &QAction::triggered, this, &MainWindow::onActionRemovePreset);

    QAction* apply_preset = new QAction(tr("Apply"), this);
    QAction* edit_preset = new QAction(tr("Edit"), this);
    connect(apply_preset, &QAction::triggered, this, &MainWindow::onApplyPreset);
    connect(edit_preset, &QAction::triggered, this, &MainWindow::onActionEditPreset);

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
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &MainWindow::providePresetContextMenu);

    // ***************************** Preset menu actions ***********************************//

    addsection = new QAction(tr("Add section"), this);
    addpreset = new QAction(tr("Add new preset"), this);
    addsection->setIcon(QIcon(":/resources/icons/16x16/cil-folder.png"));
    addpreset->setIcon(QIcon(":/resources/icons/16x16/cil-file.png"));
    connect(addsection, &QAction::triggered, this, &MainWindow::add_section);
    connect(addpreset, &QAction::triggered, this, &MainWindow::add_preset);

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

void MainWindow::setParameters()    /*** Set parameters ***/
{
    // ***************************** Set parameters ***********************************//
    createConnections();
    trayIcon = new QSystemTrayIcon(this);
    _new_param.resize(PARAMETERS_COUNT);
    _cur_param.resize(PARAMETERS_COUNT);
    _preset_table.resize(PARAMETERS_COUNT+1);
    for (int i = 0; i < PARAMETERS_COUNT+1; i++)
      _preset_table[i].resize(5);

    // ****************************** Initialize variables ************************************//
    _openDir = QDir::homePath();
    _settings_path = QDir::homePath() + QString("/CineEncoder");
    _thumb_path = _settings_path + QString("/thumbnails");
    _preset_file = _settings_path + QString("/presets.ini");
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
    animation->setFileName(":/resources/icons/gif/cil-spinner-circle.gif");
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
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->verticalHeader()->setVisible(true);
    ui->tableWidget->setAlternatingRowColors(true);
    //ui->tableWidget->verticalHeader()->setFixedWidth(0);
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
    SETTINGS(_settings);
    if (_settings.value("Version").toInt() == SETTINGS_VERSION) {
        // Restore Main Widget
        _settings.beginGroup("MainWidget");
        restoreGeometry(_settings.value("MainWidget/geometry").toByteArray());
        _settings.endGroup();

        // Restore Main Window
        _settings.beginGroup("MainWindow");
        window->restoreState(_settings.value("MainWindow/state").toByteArray());
        int arraySize = _settings.beginReadArray("MainWindow/docks_geometry");
            for (int i = 0; i < arraySize && i < DOCKS_COUNT; i++) {
                _settings.setArrayIndex(i);
                QSize size = _settings.value("MainWindow/docks_geometry/dock_size").toSize();
                dockSizesX.append(size.width());
                dockSizesY.append(size.height());
            }
            _settings.endArray();
        _settings.endGroup();

        // Restore Tables
        _settings.beginGroup("Tables");
        ui->tableWidget->horizontalHeader()->restoreState(_settings.value("Tables/table_widget_state").toByteArray());
        ui->treeWidget->header()->restoreState(_settings.value("Tables/tree_widget_state").toByteArray());
        _settings.endGroup();

        // Restore Settings
        _settings.beginGroup("Settings");
        _prefxType = _settings.value("Settings/prefix_type").toInt();
        _suffixType = _settings.value("Settings/suffix_type").toInt();
        _prefixName = _settings.value("Settings/prefix_name").toString();
        _suffixName = _settings.value("Settings/suffix_name").toString();
        _timer_interval = _settings.value("Settings/timer_interval").toInt();
        _theme = _settings.value("Settings/theme").toInt();
        _protection = _settings.value("Settings/protection").toBool();
        _showHDR_mode = _settings.value("Settings/show_hdr_mode").toBool();
        _temp_folder = _settings.value("Settings/temp_folder").toString();
        _output_folder = _settings.value("Settings/output_folder").toString();
        _openDir = _settings.value("Settings/open_dir").toString();
        _batch_mode = _settings.value("Settings/batch_mode").toBool();
        _hideInTrayFlag = _settings.value("Settings/tray").toBool();
        _language = _settings.value("Settings/language").toString();
        _font = _settings.value("Settings/font").toString();
        _fontSize = _settings.value("Settings/font_size").toInt();
        _rowSize = _settings.value("Settings/row_size").toInt();
        _settings.endGroup();

    } else {
        const QSize delta = QApplication::desktop()->size() - WINDOW_SIZE;
        const QPointF topLeft(float(delta.width())/2, float(delta.height())/2);
        const QRect rect(topLeft.toPoint(), WINDOW_SIZE);
        setGeometry(rect);
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
            int dockWidth = static_cast<int>(coeffX[ind] * WINDOW_SIZE.width());
            int dockHeight = static_cast<int>(coeffY[ind] * WINDOW_SIZE.height());
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
        QMap<int, QString> langIndex;
        langIndex[QLocale::Chinese] = "zh";
        langIndex[QLocale::German] = "de";
        langIndex[QLocale::Russian] = "ru";
        if (langIndex.contains(locale.language())) {
            _language = langIndex.value(locale.language());
        } else {
            _language = "en";
        }
    }

    if (isMaximized())
        _expandWindowsState = true;

    if (_batch_mode) {
        ui->comboBoxMode->blockSignals(true);
        ui->comboBoxMode->setCurrentIndex(1);
        ui->comboBoxMode->blockSignals(false);
    }
    setTrayIconActions();
    showTrayIcon();
    if (_hideInTrayFlag)
        trayIcon->show();
    setTheme(_theme);
    std::cout << "Desktop env.: " << short(Helper::getEnv()) << std::endl;
}

void MainWindow::setDocksParameters(QList<int> dockSizesX, QList<int> dockSizesY)
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

void MainWindow::onCloseWindow()    /*** Close window signal ***/
{
    this->close();
}

void MainWindow::setExpandIcon()
{
    ui->expandWindow->setProperty("expanded", isMaximized() ? true : false);
    ui->expandWindow->style()->polish(ui->expandWindow);
}

void MainWindow::onHideWindow()    /*** Hide window ***/
{
    _hideInTrayFlag ? hide() : showMinimized();
}

void MainWindow::onActionAbout()   /*** About ***/
{
    Notification about(this, MessConf::CloseOnly, tr("ABOUT"));
    about.exec();
}

void MainWindow::onActionDonate()   /*** Donate ***/
{
    Notification donate(this, MessConf::AllBtns, tr("DONATE"));
    donate.exec();
}

void MainWindow::onActionSettings()
{
    Settings settings(this);
    settings.setParameters(&_output_folder,
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
                           &_fontSize,
                           &_font);
    //settings.setModal(true);
    if (settings.exec() == Dialog::Accept) {
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
        showInfoMessage(_message);
    }
}

void MainWindow::showMetadataEditor()
{
    if (!docks[DockIndex::METADATA_DOCK]->isVisible()) {
        docks[DockIndex::METADATA_DOCK]->setVisible(true);
    }
    docks[DockIndex::METADATA_DOCK]->setFloating(true);
}

void MainWindow::showAudioStreamsSelection()
{
    if (!docks[DockIndex::STREAMS_DOCK]->isVisible()) {
        docks[DockIndex::STREAMS_DOCK]->setVisible(true);
    }
    docks[DockIndex::STREAMS_DOCK]->setFloating(true);
    ui->tabWidgetRight->setCurrentIndex(0);
}

void MainWindow::showSubtitlesSelection()
{
    if (!docks[DockIndex::STREAMS_DOCK]->isVisible()) {
        docks[DockIndex::STREAMS_DOCK]->setVisible(true);
    }
    docks[DockIndex::STREAMS_DOCK]->setFloating(true);
    ui->tabWidgetRight->setCurrentIndex(1);
}

void MainWindow::showVideoSplitter()
{
    if (!docks[DockIndex::SPLIT_DOCK]->isVisible()) {
        docks[DockIndex::SPLIT_DOCK]->setVisible(true);
    }
    docks[DockIndex::SPLIT_DOCK]->setFloating(true);
}

void MainWindow::get_current_data() /*** Get current data ***/
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
    ui->lineEditStartTime->setText(Helper::timeConverter(_startTime));
    ui->lineEditEndTime->setText(Helper::timeConverter(_endTime));

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

void MainWindow::setTheme(int &ind_theme)   /*** Set theme ***/
{
    const QString themePath = QString(":/resources/css/style_%1.css")
            .arg(QString::number(ind_theme));
    QFile file(themePath);
    if (file.open(QFile::ReadOnly)) {
        const QString list = QString::fromUtf8(file.readAll());
        setStyleSheet(Helper::getParsedCss(list));
        file.close();
    }
    for (int i = 11; i <= 19; i++) {
        _showHDR_mode ? ui->tableWidget->showColumn(i) :
                        ui->tableWidget->hideColumn(i);
    }
    setExpandIcon();
}

void MainWindow::setStatus(QString status)
{
    QTableWidgetItem *newItem_status = new QTableWidgetItem(status);
    newItem_status->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(_row, ColumnIndex::STATUS, newItem_status);
}

void MainWindow::restore_initial_state()    /*** Restore initial state ***/
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
    ui->actionEncode->setProperty("status", _status_encode_btn);
    ui->actionEncode->style()->polish(ui->actionEncode);
    //ui->actionEncode->setIcon(QIcon(QPixmap(":/resources/icons/svg/play.svg")));
    ui->actionEncode->setToolTip(tr("Encode"));
}

void MainWindow::changeEvent(QEvent *event)
{
    BaseWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        _expandWindowsState = (isMaximized()) ? true: false;
        setExpandIcon();
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            ui->frame_middle->setFocus();
            return true;
        }
    } else

    if (watched == raiseThumb) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton) {
                onActionAdd();
                return true;
            }
        }
    } else

    if (watched == ui->labelThumb) {
        if (event->type() == QEvent::Resize) {
            if (!preview_pixmap.isNull()) {
                QPixmap pix_scaled;
                pix_scaled = preview_pixmap.scaled(ui->frame_preview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                ui->labelThumb->setPixmap(pix_scaled);
            }
        }
    }
    return BaseWindow::eventFilter(watched, event);
}

/************************************************
** Encoder
************************************************/

void MainWindow::initEncoding()
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

void MainWindow::onEncodingMode(const QString &mode)
{
    ui->label_Progress->setText(mode);
}

void MainWindow::onEncodingStarted()
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

void MainWindow::onEncodingInitError(const QString &message)
{
    restore_initial_state();
    showInfoMessage(message);
    /*_status_encode_btn = "start";
    ui->actionEncode->setIcon(QIcon(QPixmap(":/resources/icons/16x16/play.svg"));
    ui->actionEncode->setToolTip(tr("Encode"));
    showInfoMessage(_message, false);*/
}

void MainWindow::onEncodingProgress(const int &percent, const float &rem_time)
{
    ui->progressBar->setValue(percent);
    ui->label_RemTime->setText(Helper::timeConverter(rem_time));
}

void MainWindow::onEncodingLog(const QString &log)
{
    ui->textBrowser_log->append(log);
}

void MainWindow::onEncodingCompleted()
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
            _message = tr("Task completed!\n\n Elapsed time: ") + Helper::timeConverter(elps_t);
            showInfoMessage(_message);
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
        _message = tr("Task completed!\n\n Elapsed time: ") + Helper::timeConverter(elps_t);
        showInfoMessage(_message);
    }
    QDir().remove(QDir::homePath() + QString("/ffmpeg2pass-0.log"));
    QDir().remove(QDir::homePath() + QString("/ffmpeg2pass-0.log.mbtree"));
    QDir().remove(QDir::homePath() + QString("/x265_2pass.log"));
    QDir().remove(QDir::homePath() + QString("/x265_2pass.log.cutree"));
}

void MainWindow::onEncodingAborted()
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
    showInfoMessage(_message);
}

void MainWindow::onEncodingError(const QString &error_message)
{
    std::cout << "Error_1 ..." << std::endl;  //  Debug info //
    if (_protection)
        timer->stop();
    setStatus(tr("Error!"));
    restore_initial_state();
    _message = (error_message != "") ? tr("An error occurred: ") + error_message :
                                       tr("Unexpected error occurred!");
    showInfoMessage(_message);
}

void MainWindow::pause()    // Pause encoding
{
    if (_protection)
        timer->stop();
    if (encoder->getEncodingState() != QProcess::NotRunning) {
        setStatus(tr("Pause"));
        animation->stop();
        encoder->pauseEncoding();
    }
}

void MainWindow::resume()   // Resume encoding
{
    if (_protection)
        timer->start();
    if (encoder->getEncodingState() != QProcess::NotRunning) {
        setStatus(tr("Encoding"));
        animation->start();
        encoder->resumeEncoding();
    }
}

void MainWindow::repeatHandler_Type_1()  /*** Repeat handler ***/
{
    std::cout<< "Call by timer..." << std::endl;
    onActionEncode();
    showInfoMessage(tr("Pause"), true);
    onActionEncode();
}

/************************************************
** Task Window
************************************************/

void MainWindow::onActionAdd() /*** Add files ***/
{
    QStringList result;
    Helper::openFileDialog(FileDialogType::OPENVFILES,
                           tr("Open Files"),
                           _openDir,
                           result);
    if (!result.isEmpty())
        openFiles(result);
}

void MainWindow::onActionRemove()  /*** Remove file from table ***/
{
    _row = ui->tableWidget->currentRow();
    if (_row != -1) {
        ui->tableWidget->removeRow(_row);
    }
}

void MainWindow::onButtonCloseTaskWindow()
{
    onCloseWindow();
}

void MainWindow::onSortDown()    /*** Sort table ***/
{
    ui->tableWidget->sortByColumn(ColumnIndex::FILENAME, Qt::DescendingOrder);
}

void MainWindow::onSortUp()    /*** Sort table ***/
{
    ui->tableWidget->sortByColumn(ColumnIndex::FILENAME, Qt::AscendingOrder);
}

void MainWindow::onActionEncode()  /*** Encode button ***/
{
    switch (_status_encode_btn) {
    case EncodingStatus::START: {
        std::cout << "Status encode btn: start" << std::endl;  // Debug info //
        int cnt = ui->tableWidget->rowCount();
        if (cnt == 0) {
            _message = tr("Select input file first!");
            showInfoMessage(_message);
            return;
        }
        if (_pos_cld == -1) {
            _message = tr("Select preset first!");
            showInfoMessage(_message);
            return;
        }
        _status_encode_btn = EncodingStatus::PAUSE;
        //ui->actionEncode->setIcon(QIcon(QPixmap(":/resources/icons/svg/pause.svg")));
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
        //ui->actionEncode->setIcon(QIcon(QPixmap(":/resources/icons/svg/forward.svg")));
        break;
    }
    case EncodingStatus::RESUME: {
        std::cout << "Status encode btn: resume" << std::endl;  // Debug info //
        resume();
        _status_encode_btn = EncodingStatus::PAUSE;
        //ui->actionEncode->setIcon(QIcon(QPixmap(":/resources/icons/svg/pause.svg")));
        break;
    }
    default:
        return;
    }
    QString status[] = {tr("Pause"), tr("Resume"), tr("Pause")};
    ui->actionEncode->setToolTip(status[_status_encode_btn]);
    ui->actionEncode->setProperty("status", _status_encode_btn);
    ui->actionEncode->style()->polish(ui->actionEncode);
}

void MainWindow::onActionStop()    /*** Stop ***/
{
    std::cout << "Call Stop ..." << std::endl;  //  Debug info //
    if (encoder->getEncodingState() != QProcess::NotRunning) {
        _message = tr("Stop encoding?");
        bool confirm = showDialogMessage(_message);
        if (confirm) {
            encoder->stopEncoding();
        }
    }
}

void MainWindow::openFiles(const QStringList &openFileNames)    /*** Open files ***/
{
    Progress prg(this, tr("OPENING FILES"));
    prg.setModal(true);
    prg.show();
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
        prg.setText(inputFile);
        prg.setPercent(0);
        QApplication::processEvents();
        if (i == 1)
            _openDir = inputFolder;
        MI.Open(file.toStdWString());
        QString fmt_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"Format"));
        QString width_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"Width"));
        QString height_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"Height"));
        QString size = width_qstr + "x" + height_qstr;
        QString duration_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"Duration"));
        double duration_double = 0.001 * duration_qstr.toDouble();
        float duration_float = static_cast<float>(duration_double);
        QString durationTime = Helper::timeConverter(duration_float);
        QString fps_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"FrameRate"));
        const QString status("");
        const QString bitrate_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"BitRate"));
        int bitrate_int = static_cast<int>(0.001 * bitrate_qstr.toDouble());        
        QString matrix_coefficients = QString::fromStdWString(MI.Get(Stream_Video, 0, L"matrix_coefficients"));
        const QString mtrx_coeff = matrix_coefficients.replace(" ", "").replace(".", "").replace("on-", "").replace("onstant", "");
        QString mastering_display_luminance_qstr = QString::fromStdWString(MI.Get(Stream_Video, 0, L"MasteringDisplay_Luminance"));
        QString mastering_display_luminance_rep = mastering_display_luminance_qstr.replace("min: ", "").replace("max: ", "").replace(" cd/m2", "");
        QString min_luminance = mastering_display_luminance_rep.split(", ")[0];
        QString max_luminance = mastering_display_luminance_rep.replace(min_luminance, "").replace(", ", "");
        QString mastering_display_color_primaries = QString::fromStdWString(MI.Get(Stream_Video, 0, L"MasteringDisplay_ColorPrimaries"));       
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

        const QString arr_items[] = {
            inputFile, fmt_qstr, size, durationTime, fps_qstr,
            QString::fromStdWString(MI.Get(Stream_Video, 0, L"DisplayAspectRatio")),
            status, QString::number(bitrate_int),
            QString::fromStdWString(MI.Get(Stream_Video, 0, L"ChromaSubsampling")),
            QString::fromStdWString(MI.Get(Stream_Video, 0, L"BitDepth")),
            QString::fromStdWString(MI.Get(Stream_Video, 0, L"ColorSpace")),
            QString::fromStdWString(MI.Get(Stream_Video, 0, L"colour_range")),
            QString::fromStdWString(MI.Get(Stream_Video, 0, L"colour_primaries")).replace(".", ""),
            mtrx_coeff,
            QString::fromStdWString(MI.Get(Stream_Video, 0, L"transfer_characteristics")).replace(".", ""),
            max_luminance, min_luminance,
            QString::fromStdWString(MI.Get(Stream_Video, 0, L"MaxCLL")).replace(" cd/m2", ""),
            QString::fromStdWString(MI.Get(Stream_Video, 0, L"MaxFALL")).replace(" cd/m2", ""),
            mastering_display_color_primaries, inputFolder, QString::number(duration_double, 'f', 3),
            chroma_coord, white_coord,
            QString::fromStdWString(MI.Get(Stream_Video, 0, L"StreamSize")),
            width_qstr, height_qstr,
            QString::fromStdWString(MI.Get(Stream_Video, 0, L"Title")),
            QString::fromStdWString(MI.Get(Stream_General, 0, L"Movie")),
            QString::fromStdWString(MI.Get(Stream_General, 0, L"YEAR")),
            QString::fromStdWString(MI.Get(Stream_General, 0, L"AUTHOR")),
            QString::fromStdWString(MI.Get(Stream_General, 0, L"Performer")),
            QString::fromStdWString(MI.Get(Stream_General, 0, L"Description"))
        };

        for (int column = ColumnIndex::FILENAME; column <= ColumnIndex::T_VIDEODESCR; column++) {
            QTableWidgetItem *item = new QTableWidgetItem(arr_items[column]);
            if (column >= ColumnIndex::FORMAT && column <= ColumnIndex::MASTERDISPLAY) {
                item->setTextAlignment(Qt::AlignCenter);
            }
            ui->tableWidget->setItem(numRows, column, item);
        }

        QTableWidgetItem *item_startTime = new QTableWidgetItem("0");
        QTableWidgetItem *item_endTime = new QTableWidgetItem("0");
        ui->tableWidget->setItem(numRows, ColumnIndex::T_STARTTIME, item_startTime);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_ENDTIME, item_endTime);

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
        prg.setPercent(50);
        QApplication::processEvents();
        ui->tableWidget->selectRow(ui->tableWidget->rowCount() - 1);
        QApplication::processEvents();
        prg.setPercent(100);
        QApplication::processEvents();
#if defined (Q_OS_UNIX)
        usleep(50000);
#elif defined (Q_OS_WIN64)
        Sleep(50);
#endif
        i++;
    }
}

void MainWindow::onTableWidget_itemSelectionChanged()
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
        for (int i = 0; i < 11; i++) {
            _hdr[i] = "";
        }

        // **************************** Reset metadata variables ***********************************//
        for (int i = 0; i < 6; i++) {
            _videoMetadata[i] = "";
        }

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

void MainWindow::resizeTableRows(int rows_height)
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

void MainWindow::resetView()
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

void MainWindow::provideContextMenu(const QPoint &position)     /*** Call table items menu  ***/
{
    QTableWidgetItem *item = ui->tableWidget->itemAt(0, position.y());
    if (item != nullptr) {
        itemMenu->exec(ui->tableWidget->mapToGlobal(QPoint(position.x(), position.y() + 35)));
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)     /*** Drag enter event ***/
{
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event)     /*** Drag move event ***/
{
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent* event)     /*** Drag leave event ***/
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent* event)     /*** Drag & Drop ***/
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

void MainWindow::onComboBoxMode_currentIndexChanged(int index)
{
    _batch_mode = (index == 0) ? false : true;
}

void MainWindow::onIconResizeSlider_valueChanged(int value)
{
    _rowSize = value;
    resizeTableRows(value);
}

/************************************************
** Preview Window
************************************************/

QString MainWindow::setThumbnail(QString curFilename,
                             const double &time,
                             const int &quality,
                             const int &destination)
{
    QString qualityParam("-vf scale=480:-1 -compression_level 10 -pix_fmt rgb24");
    if (quality == PreviewRes::RES_LOW)
        qualityParam = QString("-vf scale=144:-1,format=pal8,dctdnoiz=4.5");
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
    if (!tmb.exists())
        tmb_file = ":/resources/images/no_preview.png";
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

void MainWindow::repeatHandler_Type_2()
{
    //std::cout << "Call by timer... " << std::endl;
    if (_row != -1)
        setThumbnail(_curFilename, _curTime, PreviewRes::RES_HIGH, PreviewDest::SPLITTER);
}

/************************************************
** Source Window
************************************************/

void MainWindow::onButtonHotInputFile()
{
    onActionAdd();
}

/************************************************
** Output Window
************************************************/

void MainWindow::get_output_filename()  /*** Get output data ***/
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

void MainWindow::onButtonHotOutputFile()
{
    QStringList result;
    Helper::openFileDialog(FileDialogType::SELECTFOLDER,
                           tr("Select output folder"),
                           _output_folder,
                           result);
    if (!result.isEmpty()) {
        _output_folder = result.at(0);
        if (_row != -1)
            get_output_filename();
    }
}

/************************************************
** Metadata Window
************************************************/

void MainWindow::onActionClearMetadata()
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

void MainWindow::onActionUndoMetadata()
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

void MainWindow::onActionClearAudioTitles()
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

void MainWindow::onActionClearSubtitleTitles()
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

void MainWindow::onActionUndoTitles()
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

void MainWindow::onSplitSlider_valueChanged(int value)
{
    if (_row != -1) {
        double fps_double = _fps.toDouble();
        if (fps_double != 0.0) {
            _curTime = round(1000.0 * (static_cast<double>(value) / fps_double)) / 1000.0;
        } else {
            _curTime = 0.0;
        }
        ui->lineEditCurTime->setText(Helper::timeConverter(_curTime));
        timerCallSetThumbnail->stop();
        timerCallSetThumbnail->start();
    }
}

void MainWindow::onButtonFramePrevious()
{
    int value = ui->horizontalSlider->value();
    if (value > 0) {
        ui->horizontalSlider->setValue(value - 1);
    }
}

void MainWindow::onButtonFrameNext()
{
    int value = ui->horizontalSlider->value();
    if (value < _fr_count) {
        ui->horizontalSlider->setValue(value + 1);
    }
}

void MainWindow::onButtonSetStartTime()
{
    if (_row != -1) {
        _startTime = _curTime;
        if (_startTime > _endTime && _endTime != 0.0) {
            _startTime = _endTime;
        }
        ui->lineEditStartTime->setText(Helper::timeConverter(_startTime));
        QTableWidgetItem *newItem_startTime = new QTableWidgetItem(QString::number(_startTime, 'f', 3));
        ui->tableWidget->setItem(_row, ColumnIndex::T_STARTTIME, newItem_startTime);
    }
}

void MainWindow::onButtonSetEndTime()
{
    if (_row != -1) {
        _endTime = _curTime;
        if (_endTime < _startTime) {
            _endTime =_startTime;
        }
        ui->lineEditEndTime->setText(Helper::timeConverter(_endTime));
        QTableWidgetItem *newItem_endTime = new QTableWidgetItem(QString::number(_endTime, 'f', 3));
        ui->tableWidget->setItem(_row, ColumnIndex::T_ENDTIME, newItem_endTime);
    }
}

void MainWindow::onActionResetLabels()
{
    if (_row != -1) {
        ui->labelSplitPreview->clear();
        ui->horizontalSlider->blockSignals(true);
        ui->horizontalSlider->setValue(0);
        ui->horizontalSlider->blockSignals(false);

        _curTime = 0.0;
        ui->lineEditCurTime->setText(Helper::timeConverter(_curTime));

        _startTime = 0.0;
        ui->lineEditStartTime->setText(Helper::timeConverter(_startTime));
        QTableWidgetItem *newItem_startTime = new QTableWidgetItem(QString::number(_startTime, 'f', 3));
        ui->tableWidget->setItem(_row, ColumnIndex::T_STARTTIME, newItem_startTime);

        _endTime = 0.0;
        ui->lineEditEndTime->setText(Helper::timeConverter(_endTime));
        QTableWidgetItem *newItem_endTime = new QTableWidgetItem(QString::number(_endTime, 'f', 3));
        ui->tableWidget->setItem(_row, ColumnIndex::T_ENDTIME, newItem_endTime);
    }
}

/************************************************
** Preset Window
************************************************/

void MainWindow::set_defaults() /*** Set default presets ***/
{
    std::cout<< "Set defaults..." << std::endl;
    QFile _prs_file(":/resources/data/default_presets.ini");
    if (_prs_file.open(QIODevice::ReadOnly)) {
        QDataStream in(&_prs_file);
        in.setVersion(QDataStream::Qt_4_0);
        int ver;
        in >> ver;
        if (ver == PRESETS_VERSION) { // Replace to open the old version
            in >> _cur_param >> _pos_top >> _pos_cld >> _preset_table;
        }
        _prs_file.close();
    }
}

void MainWindow::onApplyPreset()  /*** Apply preset ***/
{
    int index = ui->treeWidget->currentIndex().row();
    if (index < 0) {
        _message = tr("Select preset first!\n");
        showInfoMessage(_message);
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
        showInfoMessage(_message);
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

void MainWindow::onActionRemovePreset()  /*** Remove preset ***/
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
        bool confirm = showDialogMessage(_message);
        if (confirm == true) {
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
            showInfoMessage(_message);
        }
    }
}

void MainWindow::onActionEditPreset()  /*** Edit preset ***/
{
    int index = ui->treeWidget->currentIndex().row();
    if (index < 0) {
        _message = tr("Select preset first!\n");
        showInfoMessage(_message);
        return;
    }
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem != nullptr) {
        // Item is child...
        for (int k = 0; k < PARAMETERS_COUNT; k++) {
            _new_param[k] = item->text(k+7);
        }
        Preset presetWindow(this, &_new_param);
        //presetWindow.setModal(true);
        if (presetWindow.exec() == Dialog::Accept) {
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
        showInfoMessage(_message);
    }
}

void MainWindow::add_section()  /*** Add section ***/
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

void MainWindow::add_preset()  /*** Add preset ***/
{
    int index = ui->treeWidget->currentIndex().row();
    if (index < 0) {
        _message = tr("First add a section!\n");
        showInfoMessage(_message);
        return;
    }

    QVector<QString> cur_param = {
        "Emergency, Res: Source, Fps: Source, YUV, 4:2:2, 10 bit, HDR: Enabled, Audio: PCM 16 bit, MOV",
        "18", "0", "0", "Auto", "Auto", "Auto", "0", "0", "0", "0", "0", "0", "0", "", "", "", "", "0",
        "From source", "From source", "0", "0", "Auto", "0", "0", "0", "0", "0", "0", "Emergency", "0",
        "0", "0"
    };
    QFile _prs_file(":/resources/data/default_presets.ini");
    if (_prs_file.open(QIODevice::ReadOnly)) {
        QDataStream in(&_prs_file);
        in.setVersion(QDataStream::Qt_4_0);
        int ver;
        in >> ver;
        if (ver == PRESETS_VERSION) {
            cur_param.clear();
            in >> cur_param;
        } else {std::cout << "Added energercy params..." << std::endl;}
        _prs_file.close();
    }
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

void MainWindow::renameSectionPreset()
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

void MainWindow::setItemStyle(QTreeWidgetItem *item)
{
    QFont font = qApp->font();
    font.setItalic(true);
    QColor foregroundChildColor;
    switch (_theme) {
    case Theme::GRAY:
    case Theme::DARK:
    case Theme::WAVE:
        foregroundChildColor.setRgb(qRgb(150, 190, 220));
        break;
    case Theme::DEFAULT:
        foregroundChildColor.setRgb(qRgb(50, 50, 80));
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

void MainWindow::updateCurPresetPos(int &index_top, int &index_child)
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

void MainWindow::updateInfoFields(QString &codec_qstr,
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

void MainWindow::updatePresetTable()
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

QString MainWindow::updateFieldCodec(int &codec)
{
    Tables t;
    return t.arr_codec[codec][0];
}

QString MainWindow::updateFieldMode(int &codec, int &mode)
{
    Tables t;
    return t.getCurrentMode(codec, mode);
}

QString MainWindow::updateFieldPreset(int &codec, int &preset)
{
    Tables t;
    return t.arr_preset[codec][preset];
}

QString MainWindow::updateFieldPass(int &codec, int &pass)
{
    Tables t;
    return t.getCurrentPass(codec, pass);
}

QString MainWindow::updateFieldAcodec(int &codec, int &acodec)
{
    Tables t;
    return t.arr_acodec[codec][acodec];
}

QString MainWindow::updateFieldContainer(int &codec, int &container)
{
    Tables t;
    return t.arr_container[codec][container];
}

void MainWindow::setPresetIcon(QTreeWidgetItem *item, bool collapsed)
{
    QString file;
    switch (_theme) {
    case Theme::GRAY:
    case Theme::DARK:
    case Theme::WAVE:
        file = (collapsed) ? QString::fromUtf8("cil-folder.png") :
                             QString::fromUtf8("cil-folder-open.png");
        break;
    case Theme::DEFAULT:
        file = (collapsed) ? QString::fromUtf8("cil-folder_light.png") :
                             QString::fromUtf8("cil-folder-open_light.png");
        break;
    }
    QIcon sectionIcon;
    sectionIcon.addFile(QString::fromUtf8(":/resources/icons/16x16/") + file,
                        QSize(), QIcon::Normal, QIcon::Off);
    item->setIcon(0, sectionIcon);
}

void MainWindow::onTreeWidget_itemCollapsed(QTreeWidgetItem *item)
{
    if (item != nullptr) {
        setPresetIcon(item, true);
    }
}

void MainWindow::onTreeWidget_itemExpanded(QTreeWidgetItem *item)
{
    if (item != nullptr) {
        setPresetIcon(item, false);
    }
}

void MainWindow::onTreeWidget_itemChanged(QTreeWidgetItem *item, int column)
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

void MainWindow::onTreeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem != nullptr) {
        onApplyPreset();
        std::cout << "Double clicked column: " << column << std::endl;
    }
}

void MainWindow::providePresetContextMenu(const QPoint &position)     /*** Call tree items menu  ***/
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
** Message Windows
************************************************/

bool MainWindow::showDialogMessage(const QString &_message)
{
    Message msg(this, MessType::DIALOG, _message);
    if (msg.exec() == Dialog::Accept)
        return true;
    return false;
}

void MainWindow::showInfoMessage(const QString &_message, const bool _timer_mode)
{
    auto showMessage = [this, _message, _timer_mode](){
        Message msg(this, MessType::INFO, _message, _timer_mode);
        msg.exec();
    };
    if (isHidden()) {
        if (_hideInTrayFlag && !_timer_mode) {
            trayIcon->showMessage(_message, tr("Task"), QSystemTrayIcon::Information, 151000);
        } else
        if (_timer_mode) {
            show();
            showMessage();
        }
    } else {
        showMessage();
    }
}
