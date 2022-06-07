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
#include <math.h>
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
#define ROWHEIGHT 25
#define DEFAULTTIMER 30
#define DEFAULTPATH QDir::homePath()
#define PRESETFILE SETTINGSPATH + QString("/presets.ini")
#define THUMBNAILPATH SETTINGSPATH + QString("/thumbnails")
#define GETINFO(a, b, c) QString::fromStdWString(MI.Get(a, b, c))
#define VINFO(a, b) QString::fromStdWString(MI.Get(Stream_Video, a, b))
#define AINFO(a, b) QString::fromStdWString(MI.Get(Stream_Audio, a, b))
#define Q_LOOP(i, start, end) for(int i = start; i < end; i++)
#define GETTEXT(row, col) ui->tableWidget->item(row, col)->text()
#define SLT(method) &MainWindow::method

typedef void(MainWindow::*FnVoidVoid)(void);
typedef void(MainWindow::*FnVoidInt)(int);

QLabel* createLabel(QWidget *parent, const char *name, const QString &text)
{
    QLabel *label = new QLabel(parent);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setObjectName(QString::fromUtf8(name));
    label->setAlignment(Qt::AlignCenter);
    label->setText(text);
    return label;
}

void undoLineEdit(QLineEdit *line) {
    line->undo();
    if (line->text() != "") {
        line->setFocus();
        line->setCursorPosition(0);
        line->setModified(true);
    }
};

void clearLineEdit(QLineEdit *line) {
    line->clear();
    line->insert("");
    line->setFocus();
    line->setModified(true);
}


MainWindow::MainWindow(QWidget *parent):
    BaseWindow(parent),
    m_hideInTrayFlag(false),
    m_showHdrFlag(false),
    m_protectFlag(false),
    m_fontSize(FONTSIZE),
    m_prefxType(0),
    m_suffixType(0),
    m_pos_top(-1),
    m_pos_cld(-1),
    m_timerInterval(DEFAULTTIMER),
    _language("en"),
    _output_folder(""),
    _temp_folder(""),
    _prefixName(DEFAULTPREFIX),
    _suffixName(DEFAULTSUFFIX),
    _font(""),
    ui(new Ui::Widget),
    m_pProcessThumbCreation(nullptr),
    _openDir(DEFAULTPATH),
    _theme(Theme::DEFAULT),
    _batch_mode(false),
    _status_encode_btn(EncodingStatus::START),
    _row(-1),
    _curTime(0),
    _curFilename(""),
    _curPath(""),
    _temp_file(""),
    _input_file(""),
    _output_file(""),
    _expandWindowsState(false),
    m_rowHeight(ROWHEIGHT)
{
    ui->setupUi(centralWidget());
    setTitleBar(ui->frame_top);
    //*************** Set labels *****************//
    QHBoxLayout *pTableLayout = new QHBoxLayout(ui->tableWidget);
    ui->tableWidget->setLayout(pTableLayout);
    m_pTableLabel = createLabel(ui->tableWidget, "TableWidgetLabel", tr("No media"));
    pTableLayout->addWidget(m_pTableLabel);
    m_pAudioLabel = createLabel(ui->frameAudio, "AudioLabel", tr("No audio"));
    ui->gridLayoutAudio->addWidget(m_pAudioLabel);
    m_pSubtitleLabel = createLabel(ui->frameSubtitle, "SubtitleLabel", tr("No subtitle"));
    ui->gridLayoutSubtitle->addWidget(m_pSubtitleLabel);

    //************** Create docks ******************//
    QGridLayout *pMiddleLayout = new QGridLayout(ui->frameMiddle);
    ui->frameMiddle->setLayout(pMiddleLayout);
    m_pDocksContainer = new QMainWindow(ui->frameMiddle);
    pMiddleLayout->addWidget(m_pDocksContainer);
    pMiddleLayout->setContentsMargins(2, 2, 2, 2);
    pMiddleLayout->setSpacing(0);
    m_pDocksContainer->setObjectName("DocksContainer");
    m_pDocksContainer->setWindowFlags(Qt::Widget);
    m_pDocksContainer->setDockNestingEnabled(true);

    m_pCentralDock = new QWidget(m_pDocksContainer);
    m_pCentralDock->setObjectName("CentralDock");
    m_pDocksContainer->setCentralWidget(m_pCentralDock);

    m_pDocksContainer->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    m_pDocksContainer->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    m_pDocksContainer->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    m_pDocksContainer->setCorner(Qt::BottomRightCorner, Qt::BottomDockWidgetArea);

    QGridLayout *pCentralDockLayout = new QGridLayout(m_pCentralDock);
    m_pCentralDock->setLayout(pCentralDockLayout);
    pCentralDockLayout->addWidget(ui->frameTask);
    pCentralDockLayout->setContentsMargins(0, 0, 0, 0);

    QString dockNames[] = {
        tr("Presets"), tr("Preview"), tr("Source"), tr("Output"),
        tr("Streams"), tr("Log"), tr("Metadata"), tr("Split")
    };
    Qt::DockWidgetArea dockArea[] = {
        Qt::LeftDockWidgetArea, Qt::BottomDockWidgetArea,
        Qt::BottomDockWidgetArea, Qt::BottomDockWidgetArea,
        Qt::RightDockWidgetArea, Qt::RightDockWidgetArea,
        Qt::RightDockWidgetArea, Qt::RightDockWidgetArea
    };
    QString objNames[] = {
        "dockPresets", "dockPreview", "dockSource", "dockOutput",
        "dockStreams", "dockLog", "dockMetadata", "dockSplit"
    };
    QFrame* dockFrames[] = {
        ui->framePreset, ui->framePreview, ui->frameSource, ui->frameOutput,
        ui->frameStreams, ui->frameLog, ui->frameMetadata, ui->frameSplit
    };
    Q_LOOP(i, 0, DOCKS_COUNT) {
        m_pDocks[i] = new QDockWidget(dockNames[i], m_pDocksContainer);
        m_pDocks[i]->setObjectName(objNames[i]);
        m_pDocks[i]->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::TopDockWidgetArea |
                                     Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
        m_pDocks[i]->setFeatures(QDockWidget::AllDockWidgetFeatures);
        m_pDocks[i]->setWidget(dockFrames[i]);
        m_pDocksContainer->addDockWidget(dockArea[i], m_pDocks[i]);
    }

    //*********** Set Event Filters ****************//
    m_pTableLabel->installEventFilter(this);
    ui->labelPreview->installEventFilter(this);
    ui->frameMiddle->setFocusPolicy(Qt::StrongFocus);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)
{
    BaseWindow::showEvent(event);
    static bool windowActivated = false;
    if (!windowActivated) {
        windowActivated = true;
        setParameters();
    }
}

void MainWindow::closeEvent(QCloseEvent *event) // Show prompt when close app
{
    event->ignore();
    if (showDialogMessage(tr("Quit program?"))) {
        if (m_pEncoder->getEncodingState() != QProcess::NotRunning)
            m_pEncoder->killEncoding();
        if (m_pProcessThumbCreation) {
            if (m_pProcessThumbCreation->state() != QProcess::NotRunning)
                m_pProcessThumbCreation->kill();
        }
        QFile prs_file(PRESETFILE);
        if (prs_file.open(QIODevice::WriteOnly)) {
            QDataStream out(&prs_file);
            out.setVersion(QDataStream::Qt_4_0);
            out << PRESETS_VERSION;
            out << _cur_param << m_pos_top << m_pos_cld << _preset_table;
            prs_file.close();
        }

        SETTINGS(stn);
        // Save Version
        stn.setValue("Version", SETTINGS_VERSION);
        // Save Window
        stn.beginGroup("Window");
        stn.setValue("Window/geometry", this->saveGeometry());
        stn.endGroup();
        // Save DocksContainer
        stn.beginGroup("DocksContainer");
        stn.setValue("DocksContainer/state", m_pDocksContainer->saveState());
        stn.setValue("DocksContainer/geometry", m_pDocksContainer->saveGeometry());
        stn.beginWriteArray("DocksContainer/docks_geometry");
            Q_LOOP(i, 0, DOCKS_COUNT) {
                stn.setArrayIndex(i);
                stn.setValue("DocksContainer/docks_geometry/dock_size", m_pDocks[i]->size());
            }
            stn.endArray();
        stn.endGroup();
        // Save Tables
        stn.beginGroup("Tables");
        stn.setValue("Tables/table_widget_state", ui->tableWidget->horizontalHeader()->saveState());
        stn.setValue("Tables/tree_widget_state", ui->treeWidget->header()->saveState());
        stn.endGroup();
        // Save Settings
        stn.beginGroup("Settings");
        stn.setValue("Settings/prefix_type", m_prefxType);
        stn.setValue("Settings/suffix_type", m_suffixType);
        stn.setValue("Settings/prefix_name", _prefixName);
        stn.setValue("Settings/suffix_name", _suffixName);
        stn.setValue("Settings/timer_interval", m_timerInterval);
        stn.setValue("Settings/theme", _theme);
        stn.setValue("Settings/protection", m_protectFlag);
        stn.setValue("Settings/show_hdr_mode", m_showHdrFlag);
        stn.setValue("Settings/temp_folder", _temp_folder);
        stn.setValue("Settings/output_folder", _output_folder);
        stn.setValue("Settings/open_dir", _openDir);
        stn.setValue("Settings/batch_mode", _batch_mode);
        stn.setValue("Settings/tray", m_hideInTrayFlag);
        stn.setValue("Settings/language", _language);
        stn.setValue("Settings/font", _font);
        stn.setValue("Settings/font_size", m_fontSize);
        stn.setValue("Settings/row_size", m_rowHeight);
        stn.endGroup();

        if (trayIcon)
            trayIcon->deleteLater();
        event->accept();
    }
}

void MainWindow::paintEvent(QPaintEvent *event) // Disable QTab draw base
{
    if (event->type() == QEvent::Paint) {
        auto tabBars = m_pDocksContainer->findChildren<QTabBar*>();
        foreach (QTabBar *tabBar, tabBars) {
            if (tabBar->drawBase())
                 tabBar->setDrawBase(false);
        }
    }
}

void MainWindow::setTrayIcon()
{
    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(QIcon(QPixmap(":/resources/icons/svg/cine-encoder.svg")));

    QMenu *trayMenu = new QMenu(this);
    const int ACT_COUNT = 3;
    QString actNames[ACT_COUNT] = {tr("Hide"), tr("Show"), tr("Exit")};
    FnVoidVoid actMethods[ACT_COUNT] = {
        SLT(hide), SLT(onRestoreWindowState), SLT(onCloseWindow)
    };
    Q_LOOP(i, 0, ACT_COUNT) {
        QAction *act = new QAction(actNames[i], trayMenu);
        connect(act, &QAction::triggered, this, actMethods[i]);
        trayMenu->addAction(act);
    }
    trayIcon->setContextMenu(trayMenu);
    connect(trayIcon, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason rsn){
        switch (rsn) {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            onRestoreWindowState();
            break;
        default:
            break;
        }
    });
}

void MainWindow::createConnections()
{
    // Buttons
    const int BTN_COUNT = 26;
    QPushButton *btns[BTN_COUNT] = {
        ui->closeWindow,  ui->hideWindow,    ui->expandWindow,
        ui->addFiles,     ui->removeFile,    ui->sortUp,
        ui->sortDown,     ui->stop,          ui->start,
        ui->settings,     ui->clearMetadata, ui->undoMetadata,
        ui->clearATitles, ui->clearSTitles,  ui->undoTitles,
        ui->framePrev,    ui->frameNext,     ui->setStartTime,
        ui->setEndTime,   ui->removePreset,  ui->editPreset,
        ui->applyPreset,  ui->addFilesHot,   ui->setOutFolder,
        ui->closeTWindow, ui->resetLabels
    };
    FnVoidVoid btn_methods[BTN_COUNT] = {
        SLT(onCloseWindow),  SLT(onHideWindow),    SLT(onExpandWindow),
        SLT(onAddFiles),     SLT(onRemoveFile),    SLT(onSortUp),
        SLT(onSortDown),     SLT(onStop),          SLT(onStart),
        SLT(onSettings),     SLT(onClearMetadata), SLT(onUndoMetadata),
        SLT(onClearATitles), SLT(onClearSTitles),  SLT(onUndoTitles),
        SLT(onFramePrev),    SLT(onFrameNext),     SLT(onSetStartTime),
        SLT(onSetEndTime),   SLT(onRemovePreset),  SLT(onEditPreset),
        SLT(onApplyPreset),  SLT(onAddFiles),      SLT(onSetOutFolder),
        SLT(onCloseWindow),  SLT(onResetLabels)
    };
    Q_LOOP(i, 0, BTN_COUNT)
        connect(btns[i], &QPushButton::clicked, this, btn_methods[i]);

    // Table
    connect(ui->tableWidget, &QTableWidget::itemSelectionChanged,
            this, SLT(onTableSelectionChanged));

    // Line edit
    QLineEdit *videoMetadata[6] = {
        ui->lineVTitle,  ui->lineVName,  ui->lineVYear,
        ui->lineVAuthor, ui->lineVPerf,  ui->lineVDescript
    };
    Q_LOOP(i, VIDEO_TITLE, VIDEO_DESCRIPTION + 1) {
        Q_ASSERT(i < 6);
        connect(videoMetadata[i], &QLineEdit::editingFinished, this, [=](){
            if (_row != -1) {
                if (videoMetadata[i]->isModified()) {
                    videoMetadata[i]->setModified(false);
                    const QString text = videoMetadata[i]->text();
                    QTableWidgetItem *__item = new QTableWidgetItem(text);
                    ui->tableWidget->setItem(_row, ColumnIndex::T_VIDEOTITLE + i, __item);
                    _videoMetadata[VIDEO_TITLE + i] = text;
                }
            }
        });
    }

    connect(ui->comboBoxMode, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboModeChanged(int)));
    connect(ui->sliderTimeline, &QSlider::valueChanged, this, SLT(onSliderTimelineChanged));
    connect(ui->sliderResize, &QSlider::valueChanged, this, SLT(onSliderResizeChanged));

    connect(ui->treeWidget, &QTreeWidget::itemCollapsed, this, SLT(onTreeCollapsed));
    connect(ui->treeWidget, &QTreeWidget::itemExpanded, this, SLT(onTreeExpanded));
    connect(ui->treeWidget, &QTreeWidget::itemChanged, this, SLT(onTreeChanged));
    connect(ui->treeWidget, &QTreeWidget::itemDoubleClicked, this, SLT(onTreeDblClicked));

    m_pEncoder = new Encoder(this);
    connect(m_pEncoder, &Encoder::onEncodingMode, this, SLT(onEncodingMode));
    connect(m_pEncoder, &Encoder::onEncodingStarted, this, SLT(onEncodingStarted));
    connect(m_pEncoder, &Encoder::onEncodingInitError, this, SLT(onEncodingInitError));
    connect(m_pEncoder, &Encoder::onEncodingProgress, this, SLT(onEncodingProgress));
    connect(m_pEncoder, &Encoder::onEncodingLog, this, SLT(onEncodingLog));
    connect(m_pEncoder, &Encoder::onEncodingCompleted, this, SLT(onEncodingCompleted));
    connect(m_pEncoder, &Encoder::onEncodingAborted, this, SLT(onEncodingAborted));
    connect(m_pEncoder, &Encoder::onEncodingError, this, SLT(onEncodingError));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(repeatHandler_Type_1()));
    timerCallSetThumbnail = new QTimer(this);
    timerCallSetThumbnail->setSingleShot(true);
    timerCallSetThumbnail->setInterval(800);
    connect(timerCallSetThumbnail, SIGNAL(timeout()), this, SLOT(repeatHandler_Type_2()));

    //************ Top menu actions ****************//
    QMenu *menuFiles = new QMenu(ui->menuFileButton);
    actAddFiles = new QAction(tr("Add files"), menuFiles);
    actRemoveFile = new QAction(tr("Remove from the list"), menuFiles);
    actCloseWindow = new QAction(tr("Close"), menuFiles);
    connect(actAddFiles, &QAction::triggered, this, SLT(onAddFiles));
    connect(actRemoveFile, &QAction::triggered, this, SLT(onRemoveFile));
    connect(actCloseWindow, &QAction::triggered, this, SLT(onCloseWindow));
    menuFiles->addAction(actAddFiles);
    menuFiles->addAction(actRemoveFile);
    menuFiles->addSeparator();
    menuFiles->addAction(actCloseWindow);
    ui->menuFileButton->setMenu(menuFiles);

    QMenu *menuEdit = new QMenu(ui->menuEditButton);
    actStart = new QAction(tr("Encode/Pause"), menuEdit);
    actStop = new QAction(tr("Stop"), menuEdit);
    connect(actStart, &QAction::triggered, this, SLT(onStart));
    connect(actStop, &QAction::triggered, this, SLT(onStop));
    menuEdit->addAction(actStart);
    menuEdit->addAction(actStop);
    ui->menuEditButton->setMenu(menuEdit);

    QMenu *menuTools = new QMenu(ui->menuToolsButton);
    actEditMetadata = new QAction(tr("Edit metadata"), menuTools);
    actSelectAudio = new QAction(tr("Select audio streams"), menuTools);
    actSelectSubtitles = new QAction(tr("Select subtitles"), menuTools);
    actSplitVideo = new QAction(tr("Split video"), menuTools);
    connect(actEditMetadata, &QAction::triggered, this, SLT(showMetadataEditor));
    connect(actSelectAudio, &QAction::triggered, this, SLT(showAudioStreams));
    connect(actSelectSubtitles, &QAction::triggered, this, SLT(showSubtitles));
    connect(actSplitVideo, &QAction::triggered, this, SLT(showVideoSplitter));
    menuTools->addAction(actEditMetadata);
    menuTools->addSeparator();
    menuTools->addAction(actSelectAudio);
    menuTools->addAction(actSelectSubtitles);
    menuTools->addSeparator();
    menuTools->addAction(actSplitVideo);
    ui->menuToolsButton->setMenu(menuTools);

    QMenu *menuPreferences = new QMenu(ui->menuPreferencesButton);
    actSettings = new QAction(tr("Settings"), menuPreferences);
    connect(actSettings, &QAction::triggered, this, SLT(onSettings));
    menuPreferences->addAction(actSettings);
    ui->menuPreferencesButton->setMenu(menuPreferences);

    QMenu *menuView = new QMenu(ui->menuViewButton);
    actResetView = new QAction(tr("Reset state"), menuView);
    connect(actResetView, &QAction::triggered, this, SLT(resetView));
    Q_LOOP(i, 0, DOCKS_COUNT)
        menuView->addAction(m_pDocks[i]->toggleViewAction());
    menuView->addAction(actResetView);
    ui->menuViewButton->setMenu(menuView);
    m_pDocks[DockIndex::LOG_DOCK]->toggleViewAction()->setChecked(false);
    m_pDocks[DockIndex::SPLIT_DOCK]->toggleViewAction()->setChecked(false);
    m_pDocks[DockIndex::LOG_DOCK]->setVisible(false);
    m_pDocks[DockIndex::SPLIT_DOCK]->setVisible(false);

    QMenu *menuAbout = new QMenu(ui->menuAboutButton);
    actAbout = new QAction(tr("About"), menuAbout);
    actDonate = new QAction(tr("Donate"), menuAbout);
    connect(actAbout, &QAction::triggered, this, SLT(onActionAbout));
    connect(actDonate, &QAction::triggered, this, SLT(onActionDonate));
    menuAbout->addAction(actAbout);
    menuAbout->addSeparator();
    menuAbout->addAction(actDonate);
    ui->menuAboutButton->setMenu(menuAbout);

    //********** Table menu actions ****************//
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    itemMenu = new QMenu(this);
    itemMenu->addAction(actRemoveFile);
    itemMenu->addSeparator();
    itemMenu->addAction(actStart);
    itemMenu->addSeparator();
    itemMenu->addAction(actEditMetadata);
    itemMenu->addSeparator();
    itemMenu->addAction(actSelectAudio);
    itemMenu->addAction(actSelectSubtitles);
    itemMenu->addSeparator();
    itemMenu->addAction(actSplitVideo);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, SLT(provideContextMenu));

    //*********** Tree menu actions ****************//
    ui->treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QAction *actAddSection = new QAction(tr("Add section"), this);
    QAction *actAddPreset = new QAction(tr("Add preset"), this);
    QAction *actRenamePreset = new QAction(tr("Rename"), this);
    QAction *actRemovePreset = new QAction(tr("Remove"), this);
    QAction *actApplyPreset = new QAction(tr("Apply"), this);
    QAction *actEditPreset = new QAction(tr("Edit"), this);
    connect(actAddSection, &QAction::triggered, this, SLT(onAddSection));
    connect(actAddPreset, &QAction::triggered, this, SLT(onAddPreset));
    connect(actRenamePreset, &QAction::triggered, this, SLT(onRenamePreset));
    connect(actRemovePreset, &QAction::triggered, this, SLT(onRemovePreset));
    connect(actApplyPreset, &QAction::triggered, this, SLT(onApplyPreset));
    connect(actEditPreset, &QAction::triggered, this, SLT(onEditPreset));

    sectionMenu = new QMenu(this);
    sectionMenu->addAction(actAddSection);
    sectionMenu->addAction(actAddPreset);
    sectionMenu->addAction(actRenamePreset);
    sectionMenu->addAction(actRemovePreset);

    presetMenu = new QMenu(this);
    presetMenu->addAction(actAddSection);
    presetMenu->addAction(actAddPreset);
    presetMenu->addAction(actRenamePreset);
    presetMenu->addAction(actRemovePreset);
    presetMenu->addSeparator();
    presetMenu->addAction(actApplyPreset);
    presetMenu->addAction(actEditPreset);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, SLT(providePresetContextMenu));

    //********** Preset menu actions ***************//
    QMenu *addPresetMenu = new QMenu(ui->addPreset);
    QAction *_actAddSection = new QAction(tr("Add section"), this);
    QAction *_actAddPreset = new QAction(tr("Add new preset"), this);
    _actAddSection->setIcon(QIcon(":/resources/icons/16x16/cil-folder.png"));
    _actAddPreset->setIcon(QIcon(":/resources/icons/16x16/cil-file.png"));
    connect(_actAddSection, &QAction::triggered, this, SLT(onAddSection));
    connect(_actAddPreset, &QAction::triggered, this, SLT(onAddPreset));
    addPresetMenu->addAction(_actAddSection);
    addPresetMenu->addSeparator();
    addPresetMenu->addAction(_actAddPreset);
    ui->addPreset->setMenu(addPresetMenu);

    //**********************************************//
    auto connectAction = [this](QLineEdit* line, bool isVisible)->void {
        auto actionList = line->findChildren<QAction*>();
        if (!actionList.isEmpty()) {
            connect(actionList.first(), &QAction::triggered, this, [this, line]() {
                line->clear();
                line->insert("");
                line->setModified(true);
                ui->frameMiddle->setFocus();
            });
        }
        if (!isVisible) {
            line->setEnabled(true);
            line->setVisible(false);
        }
    };

    //******* Metadata Elements Actions ************//
    auto linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit *>();
    foreach (QLineEdit *lineEdit, linesEditMetadata)
        connectAction(lineEdit, true);

    //******** Audio Elements Actions **************//
    auto labelsAudio = ui->frameAudio->findChildren<QLabel*>();
    foreach (QLabel *label, labelsAudio)
        label->setVisible(false);
    m_pAudioLabel->setVisible(true);

    Q_LOOP(i, 0, AMOUNT_AUDIO_STREAMS) {
        // Check Boxes
        QCheckBox *checkBoxAudio = ui->frameAudio->findChild<QCheckBox *>("checkBoxAudio_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        connect(checkBoxAudio, &QCheckBox::clicked, this, [this, checkBoxAudio, i](){
            if (_row != -1) {
                const QString state = (checkBoxAudio->checkState() == 2) ? "1" : "0";
                QTableWidgetItem *__item = new QTableWidgetItem(state);
                ui->tableWidget->setItem(_row, ColumnIndex::T_AUDIOCHECK_1 + i, __item);
                _audioStreamCheckState[i] = state.toInt();
            }
        });
        checkBoxAudio->setEnabled(true);
        checkBoxAudio->setVisible(false);

        // Line Edit Lang
        QLineEdit *lineEditLangAudio = ui->frameAudio->findChild<QLineEdit *>("lineEditLangAudio_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        connect(lineEditLangAudio, &QLineEdit::editingFinished, this, [this, lineEditLangAudio, i](){
            if (_row != -1) {
                if (lineEditLangAudio->isModified()) {
                    lineEditLangAudio->setModified(false);
                    const QString langAudio = lineEditLangAudio->text();
                    QTableWidgetItem *__item = new QTableWidgetItem(langAudio);
                    ui->tableWidget->setItem(_row, ColumnIndex::T_AUDIOLANG_1 + i, __item);
                    _audioLang[i] = langAudio;
                }
            }
        });
        connectAction(lineEditLangAudio, false);

        // Line Edit Title
        QLineEdit *lineEditTitleAudio = ui->frameAudio->findChild<QLineEdit *>("lineEditTitleAudio_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        connect(lineEditTitleAudio, &QLineEdit::editingFinished, this, [this, lineEditTitleAudio, i](){
            if (_row != -1) {
                if (lineEditTitleAudio->isModified()) {
                    lineEditTitleAudio->setModified(false);
                    const QString titleAudio = lineEditTitleAudio->text();
                    QTableWidgetItem *__item = new QTableWidgetItem(titleAudio);
                    ui->tableWidget->setItem(_row, ColumnIndex::T_AUDIOTITLE_1 + i, __item);
                    _audioTitle[i] = titleAudio;
                }
            }
        });
        connectAction(lineEditTitleAudio, false);
    }

    //******* Subtitle Elements Actions ************//
    auto labelsSubtitle = ui->frameSubtitle->findChildren<QLabel*>();
    foreach (QLabel *label, labelsSubtitle)
        label->setVisible(false);
    m_pSubtitleLabel->setVisible(true);

    Q_LOOP(i, 0, AMOUNT_SUBTITLES) {
        // Check Boxes
        QCheckBox *checkBoxSubtitle = ui->frameSubtitle->findChild<QCheckBox *>("checkBoxSubtitle_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        connect(checkBoxSubtitle, &QCheckBox::clicked, this, [this, checkBoxSubtitle, i](){
            if (_row != -1) {
                const QString state = (checkBoxSubtitle->checkState() == 2) ? "1" : "0";
                QTableWidgetItem *__item = new QTableWidgetItem(state);
                ui->tableWidget->setItem(_row, ColumnIndex::T_SUBCHECK_1 + i, __item);
                _subtitleCheckState[i] = state.toInt();
            }
        });
        checkBoxSubtitle->setEnabled(true);
        checkBoxSubtitle->setVisible(false);

        // Line Edit Lang
        QLineEdit *lineEditLangSubtitle = ui->frameSubtitle->findChild<QLineEdit *>("lineEditLangSubtitle_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        connect(lineEditLangSubtitle, &QLineEdit::editingFinished, this, [this, lineEditLangSubtitle, i](){
            if (_row != -1) {
                if (lineEditLangSubtitle->isModified()) {
                    lineEditLangSubtitle->setModified(false);
                    const QString langSubtitle = lineEditLangSubtitle->text();
                    QTableWidgetItem *__item = new QTableWidgetItem(langSubtitle);
                    ui->tableWidget->setItem(_row, ColumnIndex::T_SUBLANG_1 + i, __item);
                    _subtitleLang[i] = langSubtitle;
                }
            }
        });
        connectAction(lineEditLangSubtitle, false);

        // Line Edit Title
        QLineEdit *lineEditTitleSubtitle = ui->frameSubtitle->findChild<QLineEdit *>("lineEditTitleSubtitle_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        connect(lineEditTitleSubtitle, &QLineEdit::editingFinished, this, [this, lineEditTitleSubtitle, i](){
            if (_row != -1) {
                if (lineEditTitleSubtitle->isModified()) {
                    lineEditTitleSubtitle->setModified(false);
                    const QString titleSubtitle = lineEditTitleSubtitle->text();
                    QTableWidgetItem *__item = new QTableWidgetItem(titleSubtitle);
                    ui->tableWidget->setItem(_row, ColumnIndex::T_TITLESUB_1 + i, __item);
                    _subtitleTitle[i] = titleSubtitle;
                }
            }
        });
        connectAction(lineEditTitleSubtitle, false);
    }
}

void MainWindow::setParameters()    // Set parameters
{
    createConnections();
    _new_param.resize(PARAMETERS_COUNT);
    _cur_param.resize(PARAMETERS_COUNT);
    _preset_table.resize(PARAMETERS_COUNT+1);
    Q_LOOP(i, 0, PARAMETERS_COUNT+1)
      _preset_table[i].resize(5);

    auto comboBoxes = findChildren<QComboBox*>();
    foreach (auto combo, comboBoxes) {
        QListView *_view = new QListView(combo);
        _view->setTextElideMode(Qt::ElideMiddle);
        combo->setView(_view);
    }

    ui->comboBoxPreset->setVisible(false);
    ui->comboBoxView->setVisible(false);
    animation = new QMovie(ui->labelAnimation);
    animation->setFileName(":/resources/icons/gif/cil-spinner-circle.gif");
    animation->setScaledSize(QSize(18, 18));

    ui->labelAnimation->setMovie(animation);
    ui->labelAnimation->hide();
    ui->label_Progress->hide();
    ui->label_Remaining->hide();
    ui->label_RemTime->hide();
    ui->progressBar->hide();

    //************ Create folders ******************//
    if (!QDir(SETTINGSPATH).exists()) {
        QDir().mkdir(SETTINGSPATH);
        Dump("Setting path not existed and was created ...");
    }
    if (QDir(THUMBNAILPATH).exists()) {
        unsigned int count_thumb = QDir(THUMBNAILPATH).count();
        if (count_thumb > 300) {
            QDir(THUMBNAILPATH).removeRecursively();
            Dump("Thumbnails removed... ");
        }
    }
    if (!QDir(THUMBNAILPATH).exists()) {
        QDir().mkdir(THUMBNAILPATH);
        Dump("Thumbnail path not existed and was created ...");
    }

    //************** Read presets ******************//
    QFile _prs_file(PRESETFILE);
    if (_prs_file.open(QIODevice::ReadOnly)) {
        QDataStream in(&_prs_file);
        in.setVersion(QDataStream::Qt_4_0);
        int ver;
        in >> ver;
        if (ver == PRESETS_VERSION) {
            in >> _cur_param >> m_pos_top >> m_pos_cld >> _preset_table;
            _prs_file.close();
        }
        else {
            _prs_file.close();
            set_defaults();
        }
    } else {
        set_defaults();
    }

    //*********** Table parameters ****************//
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setShowGrid(false);
    QFont fnt = ui->tableWidget->horizontalHeader()->font();
    fnt.setBold(true);
    fnt.setItalic(true);
    ui->tableWidget->horizontalHeader()->setFont(fnt);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->verticalHeader()->setVisible(true);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setColumnWidth(ColumnIndex::FILENAME, 250);
    ui->tableWidget->setColumnWidth(ColumnIndex::FORMAT, 80);
    ui->tableWidget->setColumnWidth(ColumnIndex::RESOLUTION, 85);
    ui->tableWidget->setColumnWidth(ColumnIndex::DURATION, 70);
    ui->tableWidget->setColumnWidth(ColumnIndex::FPS, 70);
    ui->tableWidget->setColumnWidth(ColumnIndex::AR, 60);
    ui->tableWidget->setColumnWidth(ColumnIndex::STATUS, 80);
    ui->tableWidget->setIconSize(QSize(16, 16));

    for (int i = ColumnIndex::COLORRANGE; i <= ColumnIndex::MAXFALL; i++)
        ui->tableWidget->setColumnWidth(i, 82);

    for (int i = ColumnIndex::BITRATE; i <= ColumnIndex::COLORSPACE; i++)
        ui->tableWidget->hideColumn(i);

    for (int i = ColumnIndex::T_DUR; i <= ColumnIndex::T_ENDTIME; i++)
        ui->tableWidget->hideColumn(i);

    //************* Read settings ******************//
    const QString sysLang = Helper::getSysLanguage();
    QList<int> dockSizesX = {};
    QList<int> dockSizesY = {};
    SETTINGS(stn);
    if (stn.value("Version").toInt() == SETTINGS_VERSION) {
        // Restore Window
        stn.beginGroup("Window");
        restoreGeometry(stn.value("Window/geometry").toByteArray());
        stn.endGroup();
        // Restore DocksContainer
        stn.beginGroup("DocksContainer");
        m_pDocksContainer->restoreState(stn.value("DocksContainer/state").toByteArray());
        int arraySize = stn.beginReadArray("DocksContainer/docks_geometry");
            for (int i = 0; i < arraySize && i < DOCKS_COUNT; i++) {
                stn.setArrayIndex(i);
                QSize size = stn.value("DocksContainer/docks_geometry/dock_size").toSize();
                dockSizesX.append(size.width());
                dockSizesY.append(size.height());
            }
            stn.endArray();
        stn.endGroup();
        // Restore Tables
        stn.beginGroup("Tables");
        ui->tableWidget->horizontalHeader()->restoreState(stn.value("Tables/table_widget_state").toByteArray());
        ui->treeWidget->header()->restoreState(stn.value("Tables/tree_widget_state").toByteArray());
        stn.endGroup();
        // Restore Settings
        stn.beginGroup("Settings");
        m_prefxType = stn.value("Settings/prefix_type").toInt();
        m_suffixType = stn.value("Settings/suffix_type").toInt();
        _prefixName = stn.value("Settings/prefix_name").toString();
        _suffixName = stn.value("Settings/suffix_name").toString();
        m_timerInterval = stn.value("Settings/timer_interval").toInt();
        _theme = stn.value("Settings/theme").toInt();
        m_protectFlag = stn.value("Settings/protection").toBool();
        m_showHdrFlag = stn.value("Settings/show_hdr_mode").toBool();
        _temp_folder = stn.value("Settings/temp_folder").toString();
        _output_folder = stn.value("Settings/output_folder").toString();
        _openDir = stn.value("Settings/open_dir").toString();
        _batch_mode = stn.value("Settings/batch_mode").toBool();
        m_hideInTrayFlag = stn.value("Settings/tray").toBool();
        _language = stn.value("Settings/language", sysLang).toString();
        _font = stn.value("Settings/font").toString();
        m_fontSize = stn.value("Settings/font_size", FONTSIZE).toInt();
        m_rowHeight = stn.value("Settings/row_size").toInt();
        stn.endGroup();

    } else {
        const QSize delta = QApplication::desktop()->size() - WINDOW_SIZE;
        const QPointF topLeft(float(delta.width())/2, float(delta.height())/2);
        const QRect rect(topLeft.toPoint(), WINDOW_SIZE);
        setGeometry(rect);

        ui->treeWidget->setColumnWidth(0, 230);
        ui->treeWidget->setColumnWidth(1, 55);
        ui->treeWidget->setColumnWidth(2, 55);
        ui->treeWidget->setColumnWidth(3, 55);
        ui->treeWidget->setColumnWidth(4, 55);
        ui->treeWidget->setColumnWidth(5, 55);
        ui->treeWidget->setColumnWidth(6, 55);
    }

    //*********** Preset parameters ****************//
    ui->treeWidget->clear();
    ui->treeWidget->header()->setFont(fnt);
    ui->treeWidget->setHeaderHidden(false);
    ui->treeWidget->setAlternatingRowColors(true);
    int NUM_ROWS = _preset_table[0].size();
    int NUM_COLUMNS = _preset_table.size();
    QString type;
    QFont parentFont;
    parentFont.setBold(true);
    parentFont.setItalic(true);
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
            for (int j = 0; j < PARAMETERS_COUNT; j++)
                child->setText(j + 7, _preset_table[j][i]);
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
    if (m_pos_top != -1 && m_pos_cld != -1) {
        QTreeWidgetItem *__item = ui->treeWidget->topLevelItem(m_pos_top)->child(m_pos_cld);
        ui->treeWidget->setCurrentItem(__item);
    }
    Dump(NUM_ROWS << " x " << NUM_COLUMNS);
    Q_LOOP(i, 7, 41)
        ui->treeWidget->hideColumn(i);

    //*********** Other parameters *****************//
    if (dockSizesX.count() < DOCKS_COUNT || dockSizesY.count() < DOCKS_COUNT) {
        float coeffX[DOCKS_COUNT] = {0.25f, 0.04f, 0.48f, 0.48f, 0.25f, 0.25f, 0.25f, 0.25f};
        float coeffY[DOCKS_COUNT] = {0.9f, 0.1f, 0.1f, 0.1f, 0.9f, 0.9f, 0.9f, 0.9f};
        for (int ind = 0; ind < DOCKS_COUNT; ind++) {
            const int dockWidth = static_cast<int>(coeffX[ind] * WINDOW_SIZE.width());
            const int dockHeight = static_cast<int>(coeffY[ind] * WINDOW_SIZE.height());
            dockSizesX.append(dockWidth);
            dockSizesY.append(dockHeight);
        }
    }
    QTimer::singleShot(200, this, [this, dockSizesX, dockSizesY](){
        setDocksParameters(dockSizesX, dockSizesY);
    });

    if (m_timerInterval < 15)
        m_timerInterval = 30;
    timer->setInterval(m_timerInterval*1000);

    if (m_rowHeight != 0)
        ui->sliderResize->setValue(m_rowHeight);

    _expandWindowsState = isMaximized() ? true : false;

    if (_batch_mode) {
        ui->comboBoxMode->blockSignals(true);
        ui->comboBoxMode->setCurrentIndex(1);
        ui->comboBoxMode->blockSignals(false);
    }
    setTrayIcon();
    if (m_hideInTrayFlag)
        trayIcon->show();
    setTheme(_theme);
    Dump("Desktop env.: " << short(Helper::getEnv()));
}

void MainWindow::setDocksParameters(QList<int> dockSizesX, QList<int> dockSizesY)
{
    QList<QDockWidget*> docksVis;
    QList<int> dockVisSizesX;
    QList<int> dockVisSizesY;
    for (int i = 0; i < DOCKS_COUNT; i++) {
        if (m_pDocks[i]->isVisible() && !m_pDocks[i]->isFloating()){
            docksVis.append(m_pDocks[i]);
            dockVisSizesX.append(dockSizesX.at(i));
            dockVisSizesY.append(dockSizesY.at(i));
        }
    }
    m_pDocksContainer->resizeDocks(docksVis, dockVisSizesX, Qt::Horizontal);
    m_pDocksContainer->resizeDocks(docksVis, dockVisSizesY, Qt::Vertical);
    for (int i = 0; i < DOCKS_COUNT; i++) {
        if (m_pDocks[i]->isVisible() && m_pDocks[i]->isFloating()){
            m_pDocks[i]->setFloating(false); // Bypassing the error with detached docks in some Linux distributions
            m_pDocks[i]->setFloating(true);
        }
    }
}

void MainWindow::onCloseWindow()    // Close window
{
    this->close();
}

void MainWindow::setExpandIcon()
{
    ui->expandWindow->setProperty("expanded", isMaximized() ? true : false);
    ui->expandWindow->style()->polish(ui->expandWindow);
}

void MainWindow::onHideWindow()    // Hide window
{
    m_hideInTrayFlag ? hide() : showMinimized();
}

void MainWindow::onRestoreWindowState()
{
    _expandWindowsState ? showMaximized() : showNormal();
}

void MainWindow::onActionAbout()   // About
{
    Notification about(this, MessConf::CloseOnly, tr("ABOUT"));
    about.exec();
}

void MainWindow::onActionDonate()   // Donate
{
    Notification donate(this, MessConf::AllBtns, tr("DONATE"));
    donate.exec();
}

void MainWindow::onSettings()
{
    Settings settings(this);
    settings.setParameters(&_output_folder,
                           &_temp_folder,
                           &m_protectFlag,
                           &m_showHdrFlag,
                           &m_timerInterval,
                           &_theme,
                           &_prefixName,
                           &_suffixName,
                           &m_prefxType,
                           &m_suffixType,
                           &m_hideInTrayFlag,
                           &_language,
                           &m_fontSize,
                           &_font);
    if (settings.exec() == Dialog::Accept) {
        timer->setInterval(m_timerInterval*1000);
        setTheme(_theme);
        m_hideInTrayFlag ? trayIcon->show() : trayIcon->hide();
        if (_row != -1)
            get_output_filename();
        showInfoMessage(tr("You need to restart the program for the settings to take effect."));
    }
}

void MainWindow::setFloating(const int index, const QPoint &offset, const QSize &size)
{
    if (!m_pDocks[index]->isVisible())
        m_pDocks[index]->setVisible(true);
    m_pDocks[index]->setFloating(true);
    m_pDocks[index]->move(this->geometry().center() + offset);
    m_pDocks[index]->resize(size);
}

void MainWindow::showMetadataEditor()
{
    setFloating(DockIndex::METADATA_DOCK, QPoint(200,-200), QSize(300,500));
}

void MainWindow::showAudioStreams()
{
    setFloating(DockIndex::STREAMS_DOCK, QPoint(220,-180), QSize(300,500));
    ui->tabWidgetStreams->setCurrentIndex(0);
}

void MainWindow::showSubtitles()
{
    setFloating(DockIndex::STREAMS_DOCK, QPoint(220,-180), QSize(300,500));
    ui->tabWidgetStreams->setCurrentIndex(1);
}

void MainWindow::showVideoSplitter()
{
    setFloating(DockIndex::SPLIT_DOCK, QPoint(240,-160), QSize(400,410));
}

void MainWindow::get_current_data() // Get current data
{
    _dur = GETTEXT(_row, ColumnIndex::T_DUR).toDouble();
    _stream_size = GETTEXT(_row, ColumnIndex::T_STREAMSIZE);
    _width = GETTEXT(_row, ColumnIndex::T_WIDTH);
    _height = GETTEXT(_row, ColumnIndex::T_HEIGHT);
    _fmt = GETTEXT(_row, ColumnIndex::FORMAT);
    _fps = GETTEXT(_row, ColumnIndex::FPS);

    const double fps_double = _fps.toDouble();
    _fr_count = static_cast<int>(round(_dur * fps_double));
    _startTime =  GETTEXT(_row, ColumnIndex::T_STARTTIME).toDouble();
    _endTime = GETTEXT(_row, ColumnIndex::T_ENDTIME).toDouble();
    //Dump("DURATION SEC COUNT: " << _dur << "  FRAME RATE COUNT: " << _fr_count);

    QString curCodec = _fmt;
    QString curFps = _fps;
    QString curRes = GETTEXT(_row, ColumnIndex::RESOLUTION);
    QString curAr = GETTEXT(_row, ColumnIndex::AR);
    QString curBitrate = GETTEXT(_row, ColumnIndex::BITRATE);
    QString curColorSampling = GETTEXT(_row, ColumnIndex::SUBSAMPLING);
    QString curDepth = GETTEXT(_row, ColumnIndex::BITDEPTH);
    QString curSpace = GETTEXT(_row, ColumnIndex::COLORSPACE);

    _curPath = GETTEXT(_row, ColumnIndex::PATH);
    _curFilename = GETTEXT(_row, ColumnIndex::FILENAME);
    _input_file = _curPath + QString("/") + _curFilename;
    _hdr[CUR_COLOR_RANGE] = GETTEXT(_row, ColumnIndex::COLORRANGE);       // color range
    _hdr[CUR_COLOR_PRIMARY] = GETTEXT(_row, ColumnIndex::COLORPRIM);      // color primary
    _hdr[CUR_COLOR_MATRIX] = GETTEXT(_row, ColumnIndex::COLORMATRIX);     // color matrix
    _hdr[CUR_TRANSFER] = GETTEXT(_row, ColumnIndex::TRANSFER);            // transfer
    _hdr[CUR_MAX_LUM] = GETTEXT(_row, ColumnIndex::MAXLUM);               // max lum
    _hdr[CUR_MIN_LUM] = GETTEXT(_row, ColumnIndex::MINLUM);               // min lum
    _hdr[CUR_MAX_CLL] = GETTEXT(_row, ColumnIndex::MAXCLL);               // max cll
    _hdr[CUR_MAX_FALL] = GETTEXT(_row, ColumnIndex::MAXFALL);             // max fall
    _hdr[CUR_MASTER_DISPLAY] = GETTEXT(_row, ColumnIndex::MASTERDISPLAY); // master display
    _hdr[CUR_CHROMA_COORD] = GETTEXT(_row, ColumnIndex::T_CHROMACOORD);   // chr coord
    _hdr[CUR_WHITE_COORD] = GETTEXT(_row, ColumnIndex::T_WHITECOORD);     // white coord

    _videoMetadata[VIDEO_TITLE] = GETTEXT(_row, ColumnIndex::T_VIDEOTITLE);
    _videoMetadata[VIDEO_AUTHOR] = GETTEXT(_row, ColumnIndex::T_VIDEOAUTHOR);
    _videoMetadata[VIDEO_YEAR] = GETTEXT(_row, ColumnIndex::T_VIDEOYEAR);
    _videoMetadata[VIDEO_PERFORMER] = GETTEXT(_row, ColumnIndex::T_VIDEOPERF);
    _videoMetadata[VIDEO_DESCRIPTION] = GETTEXT(_row, ColumnIndex::T_VIDEODESCR);
    _videoMetadata[VIDEO_MOVIENAME] = GETTEXT(_row, ColumnIndex::T_VIDEOMOVIENAME);

    Q_LOOP(p, 0, AMOUNT_AUDIO_STREAMS) {
        _audioStreamCheckState[p] = GETTEXT(_row, p + ColumnIndex::T_AUDIOCHECK_1).toInt();
        _audioLang[p] = GETTEXT(_row, p + ColumnIndex::T_AUDIOLANG_1);
        _audioTitle[p] = GETTEXT(_row, p + ColumnIndex::T_AUDIOTITLE_1);
    }

    Q_LOOP(p, 0, AMOUNT_SUBTITLES) {
        _subtitleCheckState[p] = GETTEXT(_row, p + ColumnIndex::T_SUBCHECK_1).toInt();
        _subtitleLang[p] = GETTEXT(_row, p + ColumnIndex::T_SUBLANG_1);
        _subtitleTitle[p] = GETTEXT(_row, p + ColumnIndex::T_TITLESUB_1);
    }

    //*************** Set icons ********************//
    double halfTime = _dur/2;
    QString tmb_file = setThumbnail(_curFilename, halfTime, PreviewRes::RES_HIGH, PreviewDest::PREVIEW);
    QSize imageSize = QSize(85, 48);
    if (m_rowHeight == ROWHEIGHT) {
        const QString icons[4][5] = {
            {"cil-hdr",       "cil-camera-roll",       "cil-hd",       "cil-4k",       "cil-file"},
            {"cil-hdr",       "cil-camera-roll",       "cil-hd",       "cil-4k",       "cil-file"},
            {"cil-hdr",       "cil-camera-roll",       "cil-hd",       "cil-4k",       "cil-file"},
            {"cil-hdr_black", "cil-camera-roll_black", "cil-hd_black", "cil-4k_black", "cil-file_black"}
        };
        int type = 4;
        if (_hdr[CUR_MASTER_DISPLAY] != "")
            type = 0;
        else
        if (_height.toInt() >=1 && _height.toInt() < 720)
            type = 1;
        else
        if (_height.toInt() >= 720 && _height.toInt() < 2160)
            type = 2;
        else
        if (_height.toInt() >= 2160)
            type = 3;

        tmb_file = QString(":/resources/icons/16x16/%1.png").arg(icons[_theme][type]);
        imageSize = QSize(16, 16);
    }
    QPixmap pxmp(tmb_file);
    QPixmap scaled = pxmp.scaled(imageSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->tableWidget->item(_row, ColumnIndex::FILENAME)->setIcon(QIcon(scaled));

    //*********** Set description ******************//
    ui->label_source->setText(_curFilename);
    if (curCodec != "")
        curCodec += ", ";

    if (curRes != "")
        curRes += ", ";

    if (curFps != "")
        curFps += " fps, ";

    if (curAr != "")
        curAr += ", ";

    if (curSpace != "")
        curSpace += ", ";

    if (curColorSampling != "")
        curColorSampling += ", ";

    if (curDepth != "")
        curDepth += QString(" %1, ").arg(tr("bit"));

    if (curBitrate != "")
        curBitrate += QString(" %1; ").arg(tr("kbps"));

    QString sourceParam = curCodec + curRes + curFps + curAr + curSpace +
            curColorSampling + curDepth + curBitrate;

    int countAudioStreams = 0;
    while (countAudioStreams < AMOUNT_AUDIO_STREAMS) {
        const QString curAudioStream = GETTEXT(_row, countAudioStreams + ColumnIndex::T_AUDIO_1);
        if (curAudioStream == "")
            break;
        countAudioStreams++;
        sourceParam += QString("%1 #%2: %3; ").arg(tr("Audio"), QString::number(countAudioStreams),
                                                   curAudioStream);
    }

    int countSubtitles = 0;
    while (countSubtitles < AMOUNT_SUBTITLES) {
        const QString curSubtitle = GETTEXT(_row, countSubtitles + ColumnIndex::T_SUBTITLE_1);
        if (curSubtitle == "")
            break;
        countSubtitles++;
    }

    if (curCodec == "Undef, ")
        sourceParam = tr("Undefined");

    ui->textBrowser_1->clear();
    ui->textBrowser_1->setText(sourceParam);
    get_output_filename();

    //*********** Set time widgets *****************//
    ui->sliderTimeline->setMaximum(_fr_count);
    ui->lineEditStartTime->setText(Helper::timeConverter(_startTime));
    ui->lineEditEndTime->setText(Helper::timeConverter(_endTime));

    //********** Set video widgets *****************//
    auto linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit *>();
    foreach (QLineEdit *lineEdit, linesEditMetadata)
        lineEdit->setEnabled(true);

    ui->lineVTitle->setText(_videoMetadata[VIDEO_TITLE]);
    ui->lineVAuthor->setText(_videoMetadata[VIDEO_AUTHOR]);
    ui->lineVYear->setText(_videoMetadata[VIDEO_YEAR]);
    ui->lineVPerf->setText(_videoMetadata[VIDEO_PERFORMER]);
    ui->lineVDescript->setText(_videoMetadata[VIDEO_DESCRIPTION]);
    ui->lineVName->setText(_videoMetadata[VIDEO_MOVIENAME]);

    foreach (QLineEdit *lineEdit, linesEditMetadata)
        lineEdit->setCursorPosition(0);

    //********** Set audio widgets *****************//
    if (countAudioStreams > 0)
        m_pAudioLabel->setVisible(false);

    Q_LOOP(q, 0, AMOUNT_AUDIO_STREAMS) {
        QCheckBox *checkBoxAudio = ui->frameAudio->findChild<QCheckBox *>("checkBoxAudio_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        checkBoxAudio->setChecked(bool(_audioStreamCheckState[q]));
        QLineEdit *lineEditLangAudio = ui->frameAudio->findChild<QLineEdit *>("lineEditLangAudio_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        lineEditLangAudio->setText(_audioLang[q]);
        lineEditLangAudio->setCursorPosition(0);
        QLineEdit *lineEditTitleAudio = ui->frameAudio->findChild<QLineEdit *>("lineEditTitleAudio_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        lineEditTitleAudio->setText(_audioTitle[q]);
        lineEditTitleAudio->setCursorPosition(0);
        QLabel *labelAudio = ui->frameAudio->findChild<QLabel *>("labelTitleAudio_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        if (q < countAudioStreams) {
            checkBoxAudio->setVisible(true);
            lineEditLangAudio->setVisible(true);
            lineEditTitleAudio->setVisible(true);
            labelAudio->setVisible(true);
        }
    }

    //********* Set subtitle widgets ***************//
    if (countSubtitles > 0)
        m_pSubtitleLabel->setVisible(false);

    Q_LOOP(q, 0, AMOUNT_SUBTITLES) {
        QCheckBox *checkBoxSubtitle = ui->frameSubtitle->findChild<QCheckBox *>("checkBoxSubtitle_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        checkBoxSubtitle->setChecked(bool(_subtitleCheckState[q]));
        QLineEdit *lineEditLangSubtitle = ui->frameSubtitle->findChild<QLineEdit *>("lineEditLangSubtitle_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        lineEditLangSubtitle->setText(_subtitleLang[q]);
        lineEditLangSubtitle->setCursorPosition(0);
        QLineEdit *lineEditTitleSubtitle = ui->frameSubtitle->findChild<QLineEdit *>("lineEditTitleSubtitle_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        lineEditTitleSubtitle->setText(_subtitleTitle[q]);
        lineEditTitleSubtitle->setCursorPosition(0);
        QLabel *labelSubtitle = ui->frameSubtitle->findChild<QLabel *>("labelTitleSub_"
            + QString::number(q+1), Qt::FindDirectChildrenOnly);
        if (q < countSubtitles) {
            checkBoxSubtitle->setVisible(true);
            lineEditLangSubtitle->setVisible(true);
            lineEditTitleSubtitle->setVisible(true);
            labelSubtitle->setVisible(true);
        }
    }
}

void MainWindow::setTheme(const int ind_theme)   // Set theme
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
        m_showHdrFlag ? ui->tableWidget->showColumn(i) :
                        ui->tableWidget->hideColumn(i);
    }
    setExpandIcon();
}

void MainWindow::setStatus(const QString &status)
{
    QTableWidgetItem *__item = new QTableWidgetItem(status);
    __item->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(_row, ColumnIndex::STATUS, __item);
}

void MainWindow::restore_initial_state()    /*** Restore initial state ***/
{
    ui->treeWidget->setEnabled(true);
    animation->stop();
    actAddFiles->setEnabled(true);
    actRemoveFile->setEnabled(true);
    actSettings->setEnabled(true);
    ui->lineEditCurTime->setEnabled(true);
    ui->lineEditStartTime->setEnabled(true);
    ui->lineEditEndTime->setEnabled(true);
    ui->framePrev->setEnabled(true);
    ui->frameNext->setEnabled(true);
    ui->sliderTimeline->setEnabled(true);
    ui->setStartTime->setEnabled(true);
    ui->setEndTime->setEnabled(true);
    ui->tableWidget->setEnabled(true);
    ui->sortUp->setEnabled(true);
    ui->sortDown->setEnabled(true);
    ui->addFiles->setEnabled(true);
    ui->removeFile->setEnabled(true);

    ui->addPreset->setEnabled(true);
    ui->removePreset->setEnabled(true);
    ui->editPreset->setEnabled(true);
    ui->applyPreset->setEnabled(true);
    ui->comboBoxMode->setEnabled(true);
    ui->addFilesHot->setEnabled(true);
    ui->setOutFolder->setEnabled(true);
    ui->sliderResize->setEnabled(true);

    ui->clearMetadata->setEnabled(true);
    ui->undoMetadata->setEnabled(true);
    auto linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit*>();
    foreach (QLineEdit *lineEdit, linesEditMetadata)
        lineEdit->setEnabled(true);

    ui->undoTitles->setEnabled(true);
    ui->clearATitles->setEnabled(true);
    Q_LOOP(i, 0, AMOUNT_AUDIO_STREAMS) {
        QCheckBox *checkBoxAudio = ui->frameAudio->findChild<QCheckBox *>("checkBoxAudio_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        checkBoxAudio->setEnabled(true);
        QLineEdit *lineEditLangAudio = ui->frameAudio->findChild<QLineEdit *>("lineEditLangAudio_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        lineEditLangAudio->setEnabled(true);
        QLineEdit *lineEditTitleAudio = ui->frameAudio->findChild<QLineEdit *>("lineEditTitleAudio_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        lineEditTitleAudio->setEnabled(true);
    }

    ui->clearSTitles->setEnabled(true);
    Q_LOOP(i, 0, AMOUNT_SUBTITLES) {
        QCheckBox *checkBoxSubtitle = ui->frameSubtitle->findChild<QCheckBox *>("checkBoxSubtitle_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        checkBoxSubtitle->setEnabled(true);
        QLineEdit *lineEditLangSubtitle = ui->frameSubtitle->findChild<QLineEdit *>("lineEditLangSubtitle_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        lineEditLangSubtitle->setEnabled(true);
        QLineEdit *lineEditTitleSubtitle = ui->frameSubtitle->findChild<QLineEdit *>("lineEditTitleSubtitle_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        lineEditTitleSubtitle->setEnabled(true);
    }
    ui->resetLabels->setEnabled(true);
    ui->settings->setEnabled(true);
    _status_encode_btn = EncodingStatus::START;
    ui->start->setProperty("status", _status_encode_btn);
    ui->start->style()->polish(ui->start);
    ui->start->setToolTip(tr("Encode"));
}

void MainWindow::changeEvent(QEvent *event)
{
    BaseWindow::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange) {
        _expandWindowsState = isMaximized() ? true: false;
        setExpandIcon();
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Enter || keyEvent->key() == Qt::Key_Return) {
            ui->frameMiddle->setFocus();
            return true;
        }
    } else
    if (watched == m_pTableLabel) {
        if (event->type() == QEvent::MouseButtonPress) {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton) {
                onAddFiles();
                return true;
            }
        }
    } else
    if (watched == ui->labelPreview) {
        if (event->type() == QEvent::Resize) {
            if (!preview_pixmap.isNull()) {
                QPixmap pix_scaled;
                pix_scaled = preview_pixmap.scaled(ui->framePreview->size(),
                                                   Qt::KeepAspectRatio, Qt::SmoothTransformation);
                ui->labelPreview->setPixmap(pix_scaled);
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
    m_pEncoder->initEncoding(_temp_file,
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
    actAddFiles->setEnabled(false);
    actRemoveFile->setEnabled(false);
    actSettings->setEnabled(false);
    ui->lineEditCurTime->setEnabled(false);
    ui->lineEditStartTime->setEnabled(false);
    ui->lineEditEndTime->setEnabled(false);
    ui->framePrev->setEnabled(false);
    ui->frameNext->setEnabled(false);
    ui->sliderTimeline->setEnabled(false);
    ui->setStartTime->setEnabled(false);
    ui->setEndTime->setEnabled(false);
    ui->tableWidget->setEnabled(false);
    ui->sortUp->setEnabled(false);
    ui->sortDown->setEnabled(false);
    ui->addFiles->setEnabled(false);
    ui->removeFile->setEnabled(false);

    ui->addPreset->setEnabled(false);
    ui->removePreset->setEnabled(false);
    ui->editPreset->setEnabled(false);
    ui->applyPreset->setEnabled(false);
    ui->comboBoxMode->setEnabled(false);
    ui->addFilesHot->setEnabled(false);
    ui->setOutFolder->setEnabled(false);
    ui->sliderResize->setEnabled(false);

    ui->clearMetadata->setEnabled(false);
    ui->undoMetadata->setEnabled(false);
    auto linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit*>();
    foreach (QLineEdit *lineEdit, linesEditMetadata)
        lineEdit->setEnabled(false);

    ui->undoTitles->setEnabled(false);
    ui->clearATitles->setEnabled(false);
    Q_LOOP(i, 0, AMOUNT_AUDIO_STREAMS) {
        QCheckBox *checkBoxAudio = ui->frameAudio->findChild<QCheckBox *>("checkBoxAudio_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        checkBoxAudio->setEnabled(false);
        QLineEdit *lineEditLangAudio = ui->frameAudio->findChild<QLineEdit *>("lineEditLangAudio_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        lineEditLangAudio->setEnabled(false);
        QLineEdit *lineEditTitleAudio = ui->frameAudio->findChild<QLineEdit *>("lineEditTitleAudio_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        lineEditTitleAudio->setEnabled(false);
    }

    ui->clearSTitles->setEnabled(false);
    Q_LOOP(i, 0, AMOUNT_SUBTITLES) {
        QCheckBox *checkBoxSubtitle = ui->frameSubtitle->findChild<QCheckBox *>("checkBoxSubtitle_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        checkBoxSubtitle->setEnabled(false);
        QLineEdit *lineEditLangSubtitle = ui->frameSubtitle->findChild<QLineEdit *>("lineEditLangSubtitle_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        lineEditLangSubtitle->setEnabled(false);
        QLineEdit *lineEditTitleSubtitle = ui->frameSubtitle->findChild<QLineEdit *>("lineEditTitleSubtitle_"
            + QString::number(i + 1), Qt::FindDirectChildrenOnly);
        lineEditTitleSubtitle->setEnabled(false);
    }

    ui->resetLabels->setEnabled(false);
    ui->settings->setEnabled(false);
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
}

void MainWindow::onEncodingProgress(const int percent, const float rem_time)
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
    auto onTaskCompleted = [this]() {
        restore_initial_state();
        const time_t end_t = time(nullptr);
        const float elps_t = (end_t >= _strt_t) ? static_cast<float>(end_t - _strt_t) : 0.0f;
        if (m_protectFlag)
            timer->stop();
        showInfoMessage(tr("Task completed!\n\n Elapsed time: ") +
                        Helper::timeConverter(elps_t));
    };
    Dump("Completed ...");
    setStatus(tr("Done!"));
    animation->stop();
    ui->label_Progress->hide();
    ui->label_Remaining->hide();
    ui->label_RemTime->hide();
    ui->labelAnimation->hide();
    ui->progressBar->hide();
    if (_batch_mode) {
        const int row = ui->tableWidget->currentRow();
        const int numRows = ui->tableWidget->rowCount();
        if (numRows > (row + 1)) {
            ui->tableWidget->selectRow(row + 1);
            initEncoding();
        } else {
            onTaskCompleted();
        }
    } else {
        onTaskCompleted();
    }
    QList<QString> tempFiles = {
        "/ffmpeg2pass-0.log", "/ffmpeg2pass-0.log.mbtree",
        "/x265_2pass.log.temp", "/x265_2pass.log.cutree.temp",
        "/x265_2pass.log", "/x265_2pass.log.cutree"
    };
    foreach (auto &file, tempFiles)
        QDir().remove(QDir::homePath() + file);
}

void MainWindow::onEncodingAborted()
{
    Dump("Stop execute ...");
    if (m_protectFlag)
        timer->stop();
    setStatus(tr("Stop"));
    ui->label_Progress->hide();
    ui->label_Remaining->hide();
    ui->label_RemTime->hide();
    ui->labelAnimation->hide();
    ui->progressBar->hide();
    restore_initial_state();
    showInfoMessage(tr("The current encoding process has been canceled!\n"));
}

void MainWindow::onEncodingError(const QString &error_message)
{
    Dump("Error...");
    if (m_protectFlag)
        timer->stop();
    setStatus(tr("Error!"));
    restore_initial_state();
    const QString msg = (error_message != "") ? tr("An error occurred: ") + error_message :
                                                tr("Unexpected error occurred!");
    showInfoMessage(msg);
}

void MainWindow::pause()    // Pause encoding
{
    if (m_protectFlag)
        timer->stop();
    if (m_pEncoder->getEncodingState() != QProcess::NotRunning) {
        setStatus(tr("Pause"));
        animation->stop();
        m_pEncoder->pauseEncoding();
    }
}

void MainWindow::resume()   // Resume encoding
{
    if (m_protectFlag)
        timer->start();
    if (m_pEncoder->getEncodingState() != QProcess::NotRunning) {
        setStatus(tr("Encoding"));
        animation->start();
        m_pEncoder->resumeEncoding();
    }
}

void MainWindow::repeatHandler_Type_1()  // Repeat handler
{
    //Dump("Repeat handler...");
    onStart();
    showInfoMessage(tr("Pause"), true);
    onStart();
}

/************************************************
** Task Window
************************************************/

void MainWindow::onAddFiles() // Add files
{
    QStringList result;
    Helper::openFileDialog(Helper::FileDialogType::OPENVFILES,
                           tr("Open Files"),
                           _openDir,
                           result);
    if (!result.isEmpty())
        openFiles(result);
}

void MainWindow::onRemoveFile()  // Remove file from table
{
    _row = ui->tableWidget->currentRow();
    if (_row != -1)
        ui->tableWidget->removeRow(_row);
}

void MainWindow::onSortDown()    // Sort table
{
    ui->tableWidget->sortByColumn(ColumnIndex::FILENAME, Qt::DescendingOrder);
}

void MainWindow::onSortUp()    // Sort table
{
    ui->tableWidget->sortByColumn(ColumnIndex::FILENAME, Qt::AscendingOrder);
}

void MainWindow::onStart()  // Encode button
{
    switch (_status_encode_btn) {
    case EncodingStatus::START: {
        Dump("Status encode btn: start");
        if (ui->tableWidget->rowCount() == 0) {
            showInfoMessage(tr("Select input file first!"));
            return;
        }
        if (m_pos_cld == -1) {
            showInfoMessage(tr("Select preset first!"));
            return;
        }
        _status_encode_btn = EncodingStatus::PAUSE;
        _strt_t = time(nullptr);
        if (m_protectFlag)
            timer->start();
        initEncoding();
        break;
    }
    case EncodingStatus::PAUSE: {
        Dump("Status encode btn: pause");
        pause();
        _status_encode_btn = EncodingStatus::RESUME;
        break;
    }
    case EncodingStatus::RESUME: {
        Dump("Status encode btn: resume");
        resume();
        _status_encode_btn = EncodingStatus::PAUSE;
        break;
    }
    default:
        return;
    }
    QString status[] = {tr("Pause"), tr("Resume"), tr("Pause")};
    ui->start->setToolTip(status[_status_encode_btn]);
    ui->start->setProperty("status", _status_encode_btn);
    ui->start->style()->polish(ui->start);
}

void MainWindow::onStop()    // Stop
{
    Dump("Call Stop ...");
    if (m_pEncoder->getEncodingState() != QProcess::NotRunning) {
        bool confirm = showDialogMessage(tr("Stop encoding?"));
        if (confirm)
            m_pEncoder->stopEncoding();
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
        QString fmt_qstr = VINFO(0, L"Format");
        QString width_qstr = VINFO(0, L"Width");
        QString height_qstr = VINFO(0, L"Height");
        QString size = (width_qstr != "") ? width_qstr + "x" + height_qstr : "Undef";
        double duration_double = 0.001 * VINFO(0, L"Duration").toDouble();
        QString durationTime = Helper::timeConverter(static_cast<float>(duration_double));
        QString fps_qstr = VINFO(0, L"FrameRate");
        const QString status("");
        int bitrate_int = static_cast<int>(0.001 * VINFO(0, L"BitRate").toDouble());
        QString coeff = VINFO(0, L"matrix_coefficients");
        const QString mtrx_coeff = coeff.replace(" ", "").replace(".", "").replace("on-", "").replace("onstant", "");
        QString luminance_qstr = VINFO(0, L"MasteringDisplay_Luminance");
        QString luminance_rep = luminance_qstr.replace("min: ", "").replace("max: ", "").replace(" cd/m2", "");
        QString min_luminance = luminance_rep.split(", ")[0];
        QString max_luminance = luminance_rep.replace(min_luminance, "").replace(", ", "");
        QString color_prim = VINFO(0, L"MasteringDisplay_ColorPrimaries");
        int len = color_prim.length();
        QString white_coord = "";
        QString chroma_coord = "";
        if (len > 15) {
            QStringList mdcp = color_prim.split(",");
            QString r = mdcp[0].replace("R: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
            QString g = mdcp[1].replace(" G: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
            QString b = mdcp[2].replace(" B: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
            white_coord = mdcp[3].replace(" White point: ", "").replace("x=", "").replace(" ", ",").replace("y=", "").replace("000", "0");
            chroma_coord = r + "," + g + "," + b;
            color_prim = "Undefined";
        }

        if (fmt_qstr == "")
            fmt_qstr = "Undef";
        if (fps_qstr == "")
            fps_qstr = "Undef";
        if (durationTime == "00:00:00")
            durationTime = "Undef";

        const QString arr_items[] = {
            inputFile, fmt_qstr, size, durationTime, fps_qstr,
            VINFO(0, L"DisplayAspectRatio"),
            status, QString::number(bitrate_int),
            VINFO(0, L"ChromaSubsampling"),
            VINFO(0, L"BitDepth"),
            VINFO(0, L"ColorSpace"),
            VINFO(0, L"colour_range"),
            VINFO(0, L"colour_primaries").replace(".", ""),
            mtrx_coeff,
            VINFO(0, L"transfer_characteristics").replace(".", ""),
            max_luminance, min_luminance,
            VINFO(0, L"MaxCLL").replace(" cd/m2", ""),
            VINFO(0, L"MaxFALL").replace(" cd/m2", ""),
            color_prim, inputFolder, QString::number(duration_double, 'f', 3),
            chroma_coord, white_coord,
            VINFO(0, L"StreamSize"),
            width_qstr, height_qstr,
            VINFO(0, L"Title"),
            GETINFO(Stream_General, 0, L"Movie"),
            GETINFO(Stream_General, 0, L"YEAR"),
            GETINFO(Stream_General, 0, L"AUTHOR"),
            GETINFO(Stream_General, 0, L"Performer"),
            GETINFO(Stream_General, 0, L"Description")
        };

        for (int column = ColumnIndex::FILENAME; column <= ColumnIndex::T_VIDEODESCR; column++) {
            QTableWidgetItem *__item = new QTableWidgetItem(arr_items[column]);
            if (column >= ColumnIndex::FORMAT && column <= ColumnIndex::MASTERDISPLAY)
                __item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(numRows, column, __item);
        }

        QTableWidgetItem *__startTime = new QTableWidgetItem("0");
        QTableWidgetItem *__endTime = new QTableWidgetItem("0");
        ui->tableWidget->setItem(numRows, ColumnIndex::T_STARTTIME, __startTime);
        ui->tableWidget->setItem(numRows, ColumnIndex::T_ENDTIME, __endTime);

        for (int j = 0; j < AMOUNT_AUDIO_STREAMS; j++) {
            QString audioFormat = AINFO(size_t(j), L"Format");
            const QString audioLang = AINFO(size_t(j), L"Language");
            const QString audioTitle = AINFO(size_t(j), L"Title");
            const QString smplrt_qstr = AINFO(size_t(j), L"SamplingRate");
            const int smplrt_int = static_cast<int>(smplrt_qstr.toFloat() / 1000);
            const QString smplrt = (smplrt_int != 0) ? QString::number(smplrt_int) : "";
            QString audioCheckstate = "0";
            if (audioFormat != "") {
                audioFormat = audioFormat + "  " + smplrt + " kHz";
                audioCheckstate = "1";
            }
            QTableWidgetItem *__audio = new QTableWidgetItem(audioFormat);
            QTableWidgetItem *__lang = new QTableWidgetItem(audioLang);
            QTableWidgetItem *__title = new QTableWidgetItem(audioTitle);
            QTableWidgetItem *__checkstate = new QTableWidgetItem(audioCheckstate);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_AUDIO_1, __audio);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_AUDIOLANG_1, __lang);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_AUDIOTITLE_1, __title);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_AUDIOCHECK_1, __checkstate);
        }

        for (int j = 0; j < AMOUNT_SUBTITLES; j++) {
            const QString subtitleFormat = GETINFO(Stream_Text, size_t(j), L"Format");
            const QString subtitleLang = GETINFO(Stream_Text, size_t(j), L"Language");
            const QString subtitleTitle = GETINFO(Stream_Text, size_t(j), L"Title");
            const QString subtitleCheckstate = (subtitleFormat != "") ? "1" : "0";

            QTableWidgetItem *__subtitle = new QTableWidgetItem(subtitleFormat);
            QTableWidgetItem *__lang = new QTableWidgetItem(subtitleLang);
            QTableWidgetItem *__title = new QTableWidgetItem(subtitleTitle);
            QTableWidgetItem *__checkstate = new QTableWidgetItem(subtitleCheckstate);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_SUBTITLE_1, __subtitle);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_SUBLANG_1, __lang);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_TITLESUB_1, __title);
            ui->tableWidget->setItem(numRows, j + ColumnIndex::T_SUBCHECK_1, __checkstate);
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

void MainWindow::onTableSelectionChanged()
{
    ui->labelAnimation->hide();
    ui->label_Progress->hide();
    ui->label_Remaining->hide();
    ui->label_RemTime->hide();
    ui->progressBar->hide();
    ui->labelSplitPreview->clear();
    ui->sliderTimeline->blockSignals(true);
    ui->sliderTimeline->setValue(0);
    ui->sliderTimeline->blockSignals(false);
    ui->lineEditCurTime->clear();
    ui->lineEditStartTime->clear();
    ui->lineEditEndTime->clear();

    //*********** Disable audio widgets ***************//
    QList <QCheckBox *> checkBoxAudio = ui->frameAudio->findChildren<QCheckBox *>();
    QList <QLineEdit *> lineEditAudio = ui->frameAudio->findChildren<QLineEdit *>();
    QList <QLabel*> labelsAudio = ui->frameAudio->findChildren<QLabel*>();
    foreach (QCheckBox *checkBox, checkBoxAudio)
        checkBox->setVisible(false);
    foreach (QLineEdit *lineEdit, lineEditAudio)
        lineEdit->setVisible(false);
    foreach (QLabel *label, labelsAudio)
        label->setVisible(false);
    m_pAudioLabel->setVisible(true);

    //*********** Disable subtitle widgets *************//
    QList <QCheckBox *> checkBoxSubtitle = ui->frameSubtitle->findChildren<QCheckBox *>();
    QList <QLineEdit *> lineEditSubtitle = ui->frameSubtitle->findChildren<QLineEdit *>();
    QList <QLabel*> labelsSubtitle = ui->frameSubtitle->findChildren<QLabel*>();
    foreach (QCheckBox *checkBox, checkBoxSubtitle)
        checkBox->setVisible(false);
    foreach (QLineEdit *lineEdit, lineEditSubtitle)
        lineEdit->setVisible(false);
    foreach (QLabel *label, labelsSubtitle)
        label->setVisible(false);
    m_pSubtitleLabel->setVisible(true);

    _row = ui->tableWidget->currentRow();
    if (_row != -1) {
        m_pTableLabel->hide();
        get_current_data();

    } else {
        //************* Reset widgets ******************//
        m_pTableLabel->show();
        preview_pixmap = QPixmap();
        ui->labelPreview->clear();
        ui->textBrowser_1->clear();
        ui->textBrowser_2->clear();
        ui->labelPreview->setText(tr("Preview"));
        ui->label_source->setText("");
        ui->label_output->setText("");
        ui->sliderTimeline->setMaximum(0);

        //********* Reset metadata widgets *************//
        QList<QLineEdit *> linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit *>();
        foreach (QLineEdit *lineEdit, linesEditMetadata) {
            lineEdit->clear();
            lineEdit->setEnabled(false);
        }

        //********** Reset audio widgets ***************//
        foreach (QLineEdit *lineEdit, lineEditAudio)
            lineEdit->clear();
        foreach (QCheckBox *checkBox, checkBoxAudio)
            checkBox->setChecked(false);

        //********* Reset subtitle widgets *************//
        foreach (QLineEdit *lineEdit, lineEditSubtitle)
            lineEdit->clear();
        foreach (QCheckBox *checkBox, checkBoxSubtitle)
            checkBox->setChecked(false);

        //********** Reset video variables *************//
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
        for (int i = 0; i < 11; i++)
            _hdr[i] = "";

        //******* Reset metadata variables *************//
        for (int i = 0; i < 6; i++)
            _videoMetadata[i] = "";
        //******** Reset audio variables ***************//
        for (int i = 0; i < AMOUNT_AUDIO_STREAMS; i++) {
            _audioStreamCheckState[i] = 0;
            _audioLang[i] = "";
            _audioTitle[i] = "";
        }
        //******** Reset subtitle variables ************//
        for (int i = 0; i < AMOUNT_SUBTITLES; i++) {
            _subtitleCheckState[i] = 0;
            _subtitleLang[i] = "";
            _subtitleTitle[i] = "";
        }
    }
}

void MainWindow::resizeTableRows(int rows_height)
{
    QHeaderView *vertHeader = ui->tableWidget->verticalHeader();
    vertHeader->setSectionResizeMode(QHeaderView::Fixed);
    vertHeader->setDefaultSectionSize(rows_height);
    if (rows_height == ROWHEIGHT) {
        ui->tableWidget->setIconSize(QSize(16, 16));
    } else {
        const int rows_width = static_cast<int>(round(1.777f*rows_height));
        ui->tableWidget->setIconSize(QSize(rows_width, rows_height));
    }
    const int numRows = ui->tableWidget->rowCount();
    if (numRows > 0) {
        int row = ui->tableWidget->currentRow();
        ui->tableWidget->clearSelection();
        if (numRows > 1) {
            for (int i = 0; i < numRows; i++)
                ui->tableWidget->selectRow(i);
        }
        ui->tableWidget->selectRow(row);
    }
}

void MainWindow::resetView()
{
    const QString defaultSettings(":/resources/data/default_settings.ini");
    QSettings stn(defaultSettings, QSettings::IniFormat, this);
    if (stn.value("Version").toInt() == SETTINGS_VERSION) {
        // Restore DocksContainer
        stn.beginGroup("DocksContainer");
        m_pDocksContainer->restoreState(stn.value("DocksContainer/state").toByteArray());
        stn.endGroup();
    }

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

void MainWindow::provideContextMenu(const QPoint &pos)     // Call table items menu
{
    QTableWidgetItem *item = ui->tableWidget->itemAt(0, pos.y());
    if (item)
        itemMenu->exec(ui->tableWidget->mapToGlobal(pos + QPoint(0, 35)));
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event)     // Drag enter event
{
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event)     // Drag move event
{
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent* event)     // Drag leave event
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent* event)     // Drag & Drop
{
    const QMimeData *mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QStringList formats;
        QStringList pathList;
        QList<QUrl> urlList = mimeData->urls();
        for (int i = 0; i < urlList.size() && i < 32; ++i) {
            pathList.append(urlList.at(i).toLocalFile());
            formats.append(QMimeDatabase().mimeTypeForFile(pathList.at(i)).name());
        }
        if (!formats.filter("audio/").empty() || !formats.filter("video/").empty()) {
            openFiles(pathList);
            event->acceptProposedAction();
            return;
        }
    }
    event->ignore();
}

void MainWindow::onComboModeChanged(int index)
{
    _batch_mode = (index == 0) ? false : true;
}

void MainWindow::onSliderResizeChanged(int value)
{
    m_rowHeight = value;
    resizeTableRows(value);
}

/************************************************
** Preview Window
************************************************/

QString MainWindow::setThumbnail(QString curFilename,
                             const double time,
                             const int quality,
                             const int destination)
{
    QString qualityParam("-vf scale=480:-1 -compression_level 10 -pix_fmt rgb24");
    if (quality == PreviewRes::RES_LOW)
        qualityParam = QString("-vf scale=144:-1,format=pal8,dctdnoiz=4.5");
    const QString time_qstr = QString::number(time, 'f', 3);
    const QString tmb_name = curFilename.replace(".", "_").replace(" ", "_") + time_qstr;
    QString tmb_file = THUMBNAILPATH + QString("/%1.png").arg(tmb_name);
    QFile tmb(tmb_file);
    if (!tmb.exists()) {
        QStringList cmd;
        cmd << "-hide_banner" << "-ss" << time_qstr << "-i" << _input_file
            << qualityParam.split(" ") << "-vframes" << "1" << "-y" << tmb_file;
        m_pProcessThumbCreation = new QProcess(this);
        m_pProcessThumbCreation->start("ffmpeg", cmd);
        m_pProcessThumbCreation->waitForFinished();
        delete m_pProcessThumbCreation, m_pProcessThumbCreation = nullptr;
    }
    if (!tmb.exists())
        tmb_file = ":/resources/images/no_preview.png";
    preview_pixmap = QPixmap(tmb_file);
    QPixmap pix_scaled;
    if (destination == PreviewDest::PREVIEW) {
        pix_scaled = preview_pixmap.scaled(ui->framePreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labelPreview->setPixmap(pix_scaled);
    } else
    if (destination == PreviewDest::SPLITTER) {
        pix_scaled = preview_pixmap.scaled(ui->labelSplitPreview->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        ui->labelSplitPreview->setPixmap(pix_scaled);
    }
    return tmb_file;
}

void MainWindow::repeatHandler_Type_2()
{
    if (_row != -1)
        setThumbnail(_curFilename, _curTime, PreviewRes::RES_HIGH, PreviewDest::SPLITTER);
}

/************************************************
** Source Window
************************************************/

/************************************************
** Output Window
************************************************/

void MainWindow::get_output_filename()  // Get output data
{
    ui->textBrowser_2->clear();
    ui->textBrowser_2->setText(_cur_param[CurParamIndex::OUTPUT_PARAM]);    
    const int _CODEC = _cur_param[CurParamIndex::CODEC].toInt();
    const int _CONTAINER = _cur_param[CurParamIndex::CONTAINER].toInt();
    Tables t;
    extension = t.arr_container[_CODEC][_CONTAINER].toLower();
    QString suffix;
    if (m_suffixType == 0) {
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

    QString prefix = _prefixName;
    if (m_prefxType == 0) {
        std::wstring file_name_wstr = _curFilename.toStdWString();
        std::wstring::size_type separator = file_name_wstr.rfind('.');
        const QString file_without_ext = (separator != std::wstring::npos) ?
                    QString::fromStdWString(file_name_wstr.substr(0, separator)) : _curFilename;
        prefix = file_without_ext;
    }

    const QString _output_file_name = QString("%1%2.%3").arg(prefix, suffix, extension);
    ui->label_output->setText(_output_file_name);
    _output_file = (_output_folder == "") ? _curPath : _output_folder;
    _output_file += QString("/") + _output_file_name;

    _temp_file = (_temp_folder == "") ? _curPath : _temp_folder;
    _temp_file += QString("/temp.mkv");
}

void MainWindow::onSetOutFolder()
{
    QStringList result;
    Helper::openFileDialog(Helper::FileDialogType::SELECTFOLDER,
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

void MainWindow::onClearMetadata()
{
    if (_row != -1) {
        auto linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit*>();
        foreach (QLineEdit *lineEdit, linesEditMetadata)
            clearLineEdit(lineEdit);
        ui->frameMiddle->setFocus();
    }
}

void MainWindow::onUndoMetadata()
{
    if (_row != -1) {
        auto linesEditMetadata = ui->frameTab_1->findChildren<QLineEdit*>();
        foreach (QLineEdit *lineEdit, linesEditMetadata)
            undoLineEdit(lineEdit);
        ui->frameMiddle->setFocus();
    }
}

/************************************************
** Streams Window
************************************************/

void MainWindow::onClearATitles()
{
    if (_row != -1) {
        Q_LOOP(i, 0, AMOUNT_AUDIO_STREAMS) {
            QLineEdit *lineEdit = ui->frameAudio->findChild<QLineEdit*>("lineEditTitleAudio_"
                + QString::number(i + 1), Qt::FindDirectChildrenOnly);
            clearLineEdit(lineEdit);
        }
        ui->frameMiddle->setFocus();
    }
}

void MainWindow::onClearSTitles()
{
    if (_row != -1) {
        Q_LOOP(i, 0, AMOUNT_SUBTITLES) {
            QLineEdit *lineEdit = ui->frameSubtitle->findChild<QLineEdit*>("lineEditTitleSubtitle_"
                + QString::number(i + 1), Qt::FindDirectChildrenOnly);
            clearLineEdit(lineEdit);
        }
        ui->frameMiddle->setFocus();
    }
}

void MainWindow::onUndoTitles()
{
    if (_row != -1) {
        Q_LOOP(i, 0, AMOUNT_AUDIO_STREAMS) {
            QLineEdit *lineEdit = ui->frameAudio->findChild<QLineEdit *>("lineEditTitleAudio_"
                + QString::number(i + 1), Qt::FindDirectChildrenOnly);
            undoLineEdit(lineEdit);
        }
        Q_LOOP(i, 0, AMOUNT_SUBTITLES) {
            QLineEdit *lineEdit = ui->frameSubtitle->findChild<QLineEdit *>("lineEditTitleSubtitle_"
                + QString::number(i + 1), Qt::FindDirectChildrenOnly);
            undoLineEdit(lineEdit);
        }
        ui->frameMiddle->setFocus();
    }
}

/************************************************
** Split Window
************************************************/

void MainWindow::onSliderTimelineChanged(int value)
{
    if (_row != -1) {
        const double fps_double = _fps.toDouble();
        _curTime = (fps_double != 0.0) ? round(1000.0 * (static_cast<double>(value)
                                                         / fps_double)) / 1000.0 : 0.0;
        ui->lineEditCurTime->setText(Helper::timeConverter(_curTime));
        timerCallSetThumbnail->stop();
        timerCallSetThumbnail->start();
    }
}

void MainWindow::onFramePrev()
{
    const int value = ui->sliderTimeline->value();
    if (value > 0)
        ui->sliderTimeline->setValue(value - 1);
}

void MainWindow::onFrameNext()
{
    const int value = ui->sliderTimeline->value();
    if (value < _fr_count)
        ui->sliderTimeline->setValue(value + 1);
}

void MainWindow::onSetStartTime()
{
    if (_row != -1) {
        _startTime = (_startTime > _endTime && _endTime != 0.0) ? _endTime : _curTime;
        ui->lineEditStartTime->setText(Helper::timeConverter(_startTime));
        QTableWidgetItem *__item = new QTableWidgetItem(QString::number(_startTime, 'f', 3));
        ui->tableWidget->setItem(_row, ColumnIndex::T_STARTTIME, __item);
    }
}

void MainWindow::onSetEndTime()
{
    if (_row != -1) {
        _endTime = (_endTime < _startTime) ? _startTime : _curTime;
        ui->lineEditEndTime->setText(Helper::timeConverter(_endTime));
        QTableWidgetItem *__item = new QTableWidgetItem(QString::number(_endTime, 'f', 3));
        ui->tableWidget->setItem(_row, ColumnIndex::T_ENDTIME, __item);
    }
}

void MainWindow::onResetLabels()
{
    if (_row != -1) {
        ui->labelSplitPreview->clear();
        ui->sliderTimeline->blockSignals(true);
        ui->sliderTimeline->setValue(0);
        ui->sliderTimeline->blockSignals(false);

        _curTime = 0.0;
        ui->lineEditCurTime->setText(Helper::timeConverter(_curTime));

        _startTime = 0.0;
        ui->lineEditStartTime->setText(Helper::timeConverter(_startTime));
        QTableWidgetItem *__item_st = new QTableWidgetItem(QString::number(_startTime, 'f', 3));
        ui->tableWidget->setItem(_row, ColumnIndex::T_STARTTIME, __item_st);

        _endTime = 0.0;
        ui->lineEditEndTime->setText(Helper::timeConverter(_endTime));
        QTableWidgetItem *__item_end = new QTableWidgetItem(QString::number(_endTime, 'f', 3));
        ui->tableWidget->setItem(_row, ColumnIndex::T_ENDTIME, __item_end);
    }
}

/************************************************
** Preset Window
************************************************/

void MainWindow::set_defaults() // Set default presets
{
    Dump("Set defaults...");
    QFile _prs_file(":/resources/data/default_presets.ini");
    if (_prs_file.open(QIODevice::ReadOnly)) {
        QDataStream in(&_prs_file);
        in.setVersion(QDataStream::Qt_4_0);
        int ver;
        in >> ver;
        if (ver == PRESETS_VERSION) // Replace to open the old version
            in >> _cur_param >> m_pos_top >> m_pos_cld >> _preset_table;
        _prs_file.close();
    }
}

void MainWindow::onApplyPreset()  // Apply preset
{
    if (ui->treeWidget->currentIndex().row() < 0) {
        showInfoMessage(tr("Select preset first!\n"));
        return;
    }
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem) {
        // Item is child...
        for (int k = 0; k < PARAMETERS_COUNT; k++)
            _cur_param[k] = item->text(k+7);
    } else {
        // Item is parent...
        showInfoMessage(tr("Select preset first!\n"));
        return;
    }
    m_pos_top = ui->treeWidget->indexOfTopLevelItem(parentItem);
    m_pos_cld = parentItem->indexOfChild(item);
    if (_row != -1)
        get_output_filename();
}

void MainWindow::onRemovePreset()  // Remove preset
{
    if (ui->treeWidget->currentIndex().row() < 0)
        return;
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem) {
        // Item is child...
        bool confirm = showDialogMessage(tr("Delete?"));
        if (confirm) {
            const int index_top = ui->treeWidget->indexOfTopLevelItem(parentItem);
            const int index_child = parentItem->indexOfChild(item);
            updateCurPresetPos(index_top, index_child);

            QTreeWidgetItem *takenItem = parentItem->takeChild(index_child);
            Q_ASSERT(takenItem == item);
            delete takenItem;
            updatePresetTable();
        }

    } else {
        // Item is parent...
        if (item->childCount() == 0) {
            const int index_top = ui->treeWidget->indexOfTopLevelItem(item);
            const int index_child = -1;
            updateCurPresetPos(index_top, index_child);

            QTreeWidgetItem *takenItem = ui->treeWidget->takeTopLevelItem(index_top);
            Q_ASSERT(takenItem==item);
            delete takenItem;
            updatePresetTable();

        } else {
            showInfoMessage(tr("Delete presets first!\n"));
        }
    }
}

void MainWindow::onEditPreset()  // Edit preset
{
    if (ui->treeWidget->currentIndex().row() < 0) {
        showInfoMessage(tr("Select preset first!\n"));
        return;
    }
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem) {
        // Item is child...
        for (int k = 0; k < PARAMETERS_COUNT; k++)
            _new_param[k] = item->text(k+7);
        Preset presetWindow(this, &_new_param);
        if (presetWindow.exec() == Dialog::Accept) {
            for (int k = 0; k < PARAMETERS_COUNT; k++)
                item->setText(k+7, _new_param[k]);
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
            if (m_pos_top == index_top && m_pos_cld == index_child) {
                for (int k = 0; k < PARAMETERS_COUNT; k++)
                    _cur_param[k] = item->text(k+7);
                if (_row != -1)
                    get_output_filename();
            }
            updatePresetTable();
        }
    } else {
        // Item is parent...
        showInfoMessage(tr("Select preset first!\n"));
    }
}

void MainWindow::onAddSection()  // Add section
{
    QFont parentFont;
    parentFont.setBold(true);
    parentFont.setItalic(true);
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

void MainWindow::onAddPreset()  // Add preset
{
    if (ui->treeWidget->currentIndex().row() < 0) {
        showInfoMessage(tr("First add a section!\n"));
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
        } else {
            Dump("Added energercy params...");
        }
        _prs_file.close();
    }
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    QTreeWidgetItem *child = new QTreeWidgetItem();
    for (int k = 0; k < PARAMETERS_COUNT; k++)
        child->setText(k + 7, cur_param[k]);
    updateInfoFields(cur_param[1], cur_param[2], cur_param[3], cur_param[4],
                     cur_param[11], cur_param[12], cur_param[21], child, true);
    setItemStyle(child);
    if (parentItem) {
        // Item is child...
        parentItem->addChild(child);
        const int index_top = ui->treeWidget->indexOfTopLevelItem(parentItem);
        const int index_child = parentItem->indexOfChild(child);
        updateCurPresetPos(index_top, index_child);
    } else {
        // Item is parent...
        item->addChild(child);
        ui->treeWidget->expandItem(item);
        const int index_top = ui->treeWidget->indexOfTopLevelItem(item);
        const int index_child = item->indexOfChild(child);
        updateCurPresetPos(index_top, index_child);
    }
    updatePresetTable();
}

void MainWindow::onRenamePreset()
{
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    if (item->parent()) {
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
        foregroundChildColor.setRgb(qRgb(150, 150, 150));
        break;
    case Theme::DEFAULT:
        foregroundChildColor.setRgb(qRgb(20, 20, 30));
        break;
    }
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
    item->setForeground(0, foregroundChildColor);
    item->setFont(0, font);
    Q_LOOP(column, 1, 7) {
        item->setTextAlignment(column, Qt::AlignCenter);
        item->setForeground(column, foregroundChildColor);
        item->setFont(column, font);
    }
}

void MainWindow::updateCurPresetPos(const int index_top, const int index_child)
{
    Q_UNUSED(index_top)
    Q_UNUSED(index_child)
    m_pos_top = -1;
    m_pos_cld = -1;
    _cur_param[CurParamIndex::OUTPUT_PARAM] = tr("Preset not selected");
    if (_row != -1)
        get_output_filename();
}

void MainWindow::updateInfoFields(const QString &codec_qstr,
                              const QString &mode_qstr,
                              const QString &container_qstr,
                              const QString &bqr_qstr,
                              const QString &pass_qstr,
                              const QString &preset_qstr,
                              const QString &acodec_qstr,
                              QTreeWidgetItem *item,
                              const bool defaultNameFlag)
{
    Tables t;
    const int codec = codec_qstr.toInt();
    if (defaultNameFlag) {
        item->setText(0, t.arr_codec[codec][0]);
        const QString newPresetName = item->text(0);
        item->setText(30 + 7, newPresetName);
    }
    item->setText(1, t.getCurrentMode(codec, mode_qstr.toInt()));
    item->setText(2, bqr_qstr);
    item->setText(3, t.arr_preset[codec][preset_qstr.toInt()]);
    item->setText(4, t.getCurrentPass(codec, pass_qstr.toInt()));
    item->setText(5, t.arr_acodec[codec][acodec_qstr.toInt()]);
    item->setText(6, t.arr_container[codec][container_qstr.toInt()]);
}

void MainWindow::updatePresetTable()
{
    int CHILD_COUNT = 0;
    const int TOP_LEVEL_ITEMS_COUNT = ui->treeWidget->topLevelItemCount();
    for (int i = 0; i < TOP_LEVEL_ITEMS_COUNT; i++)
        CHILD_COUNT += ui->treeWidget->topLevelItem(i)->childCount();

    const int ROWS_COUNT = TOP_LEVEL_ITEMS_COUNT + CHILD_COUNT;  // Count of all rows
    for (int i = 0; i < PARAMETERS_COUNT+1; i++)
      _preset_table[i].resize(ROWS_COUNT);

    int row = 0;
    for (int top = 0; top < TOP_LEVEL_ITEMS_COUNT; top++) {
        _preset_table[0][row] = ui->treeWidget->topLevelItem(top)->text(0);
        _preset_table[PARAMETERS_COUNT][row] = "TopLewelItem";
        CHILD_COUNT = ui->treeWidget->topLevelItem(top)->childCount();
        for (int child = 0; child < CHILD_COUNT; child++) {
            row++;
            for (int column = 0; column < PARAMETERS_COUNT; column++)
                _preset_table[column][row] = ui->treeWidget->topLevelItem(top)->child(child)->text(column+7);
            _preset_table[PARAMETERS_COUNT][row] = "ChildItem";
        }
        row++;
    }
    Dump(_preset_table[0].size() << " x " << _preset_table.size());
}

void MainWindow::setPresetIcon(QTreeWidgetItem *item, bool collapsed)
{
    QString file;
    switch (_theme) {
    case Theme::GRAY:
    case Theme::DARK:
    case Theme::WAVE:
        file = (collapsed) ? QString::fromUtf8("folder.svg") :
                             QString::fromUtf8("folder_open.svg");
        break;
    case Theme::DEFAULT:
        file = (collapsed) ? QString::fromUtf8("folder_light.svg") :
                             QString::fromUtf8("folder_open_light.svg");
        break;
    }
    QIcon icon = QIcon(QPixmap(":/resources/icons/svg/" + file));
    item->setIcon(0, icon);
}

void MainWindow::onTreeCollapsed(QTreeWidgetItem *item)
{
    if (item)
        setPresetIcon(item, true);
}

void MainWindow::onTreeExpanded(QTreeWidgetItem *item)
{
    if (item)
        setPresetIcon(item, false);
}

void MainWindow::onTreeChanged(QTreeWidgetItem *item, int column)
{
    if (item->isSelected() && column == 0) {
        if (item->parent()) {
            const QString newPresetName = item->text(0);
            item->setText(30 + 7, newPresetName);
        }
        updatePresetTable();
    }
}

void MainWindow::onTreeDblClicked(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column)
    if (item->parent())
        onApplyPreset();
}

void MainWindow::providePresetContextMenu(const QPoint &pos) // Call tree items menu
{
    QTreeWidgetItem *item = ui->treeWidget->itemAt(pos);
    if (item) {
        const QPoint globPos = ui->treeWidget->mapToGlobal(pos + QPoint(0, 35));
        item->parent() ? presetMenu->exec(globPos) : sectionMenu->exec(globPos);
    }
}

/************************************************
** Message Windows
************************************************/

bool MainWindow::showDialogMessage(const QString &message)
{
    Message msg(this, MessType::DIALOG, message);
    if (msg.exec() == Dialog::Accept)
        return true;
    return false;
}

void MainWindow::showInfoMessage(const QString &message, const bool timer_mode)
{
    auto showMessage = [this, message, timer_mode](){
        Message msg(this, MessType::INFO, message, timer_mode);
        msg.exec();
    };
    if (isHidden()) {
        if (m_hideInTrayFlag && !timer_mode) {
            trayIcon->showMessage(message, tr("Task"), QSystemTrayIcon::Information, 151000);
        } else
        if (timer_mode) {
            show();
            showMessage();
        }
    } else {
        showMessage();
    }
}
