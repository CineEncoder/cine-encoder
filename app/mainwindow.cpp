#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "donate.h"
#include "settings.h"
#include "preset.h"
#include "taskcomplete.h"
#include "dialog.h"



#ifdef Q_OS_LINUX
    //#define GNOME_DESKTOP
    #define KDE_DESKTOP
    #ifndef UNICODE
        #define UNICODE
    #endif
    #include <signal.h>
    #include <MediaInfo/MediaInfo.h>
    using namespace MediaInfoLib;
#endif

#ifdef Q_OS_WIN64
    #ifdef __MINGW64__
        #ifdef _UNICODE
            #define _itot _itow
        #else
            #define _itot itoa
        #endif
    #endif
    #include "MediaInfoDLL/MediaInfoDLL.h"
    using namespace MediaInfoDLL;
#endif



Widget::Widget(QWidget *parent): QWidget(parent), ui(new Ui::Widget)
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

    raiseLayout = new QHBoxLayout(this);
    raiseThumb = new QLabel(this);
    raiseThumb->setAlignment(Qt::AlignCenter);
    raiseThumb->setText("No media");
    raiseThumb->setStyleSheet("color: #09161E; font: 64pt; font-style: oblique;");
    ui->tableWidget->setLayout(raiseLayout);
    raiseLayout->addWidget(raiseThumb);

    // **************************** Create docks ***********************************//

    QGridLayout *layout = new QGridLayout(this);
    QGridLayout *windowLayout = new QGridLayout(this);
    QGridLayout *centralwidgetLayout = new QGridLayout(this);

    window = new QMainWindow(this);
    window->setObjectName("CentralWindow");
    window->setWindowFlags(Qt::Widget);
    window->setDockNestingEnabled(true);
    ui->frame_middle->setLayout(layout);
    layout->addWidget(window);
    layout->setContentsMargins(6, 2, 6, 2);
    layout->setVerticalSpacing(0);
    layout->setHorizontalSpacing(0);

    centralWidget = new QWidget(this);
    window->setLayout(windowLayout);
    windowLayout->addWidget(centralWidget);
    window->setCentralWidget(centralWidget);
    centralWidget->setLayout(centralwidgetLayout);
    centralwidgetLayout->addWidget(ui->frame_task);
    centralwidgetLayout->setContentsMargins(0, 0, 0, 0);

    dock1 = new QDockWidget(tr("Presets"), this);
    dock1->setObjectName("Dock_presets");
    dock1->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    dock1->setWidget(ui->frameLeft);
    window->addDockWidget(Qt::LeftDockWidgetArea, dock1);

    dock2 = new QDockWidget(tr("Preview"), this);
    dock2->setObjectName("Dock_preview");
    dock2->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    dock2->setWidget(ui->frame_preview);
    window->addDockWidget(Qt::BottomDockWidgetArea, dock2);

    dock3 = new QDockWidget(tr("Source"), this);
    dock3->setObjectName("Dock_source");
    dock3->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    dock3->setWidget(ui->frame_source);
    window->addDockWidget(Qt::BottomDockWidgetArea, dock3);

    dock4 = new QDockWidget(tr("Output"), this);
    dock4->setObjectName("Dock_output");
    dock4->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    dock4->setWidget(ui->frame_output);
    window->addDockWidget(Qt::BottomDockWidgetArea, dock4);

    dock5 = new QDockWidget(tr("Options"), this);
    dock5->setObjectName("Dock_options");
    dock5->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    dock5->setWidget(ui->frameRight);
    window->addDockWidget(Qt::RightDockWidgetArea, dock5);

    dock6 = new QDockWidget(tr("Log"), this);
    dock6->setObjectName("Dock_log");
    dock6->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    dock6->setWidget(ui->frameLog);
    window->addDockWidget(Qt::RightDockWidgetArea, dock6);

    // **************************** Set Event Filters ***********************************//

    ui->centralwidget->installEventFilter(this);
    ui->centralwidget->setAttribute(Qt::WA_Hover, true);
    ui->frame_main->installEventFilter(this);
    ui->frame_main->setAttribute(Qt::WA_Hover, true);
    ui->frame_main->setAttribute(Qt::WA_NoMousePropagation, true);
    ui->frame_top->installEventFilter(this);
    raiseThumb->installEventFilter(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::showEvent(QShowEvent *event)   /*** Call set parameters ***/
{
    QWidget::showEvent(event);
    if (_windowActivated == false) {
        _windowActivated = true;
        std::cout << "Window Activated ..." << std::endl;
        setParameters();
    }
}

void Widget::closeEvent(QCloseEvent *event) /*** Show prompt when close app ***/
{
    event->ignore();
    _message = "Quit program?";
    bool confirm = call_dialog(_message);
    if (confirm == true)
    {
        std::cout << "Exit confirmed!" << std::endl;  // Debug info //
        short s1 = process_1->state();
        short s2 = process_5->state();

        if (s1 != 0) {
            process_1->kill();
        }
        if (s2 != 0) {
            process_5->kill();
        }

        if (_prs_file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QString line = "";
            for (int i = 0; i < PARAMETERS_COUNT; i++)
            {
                line += _cur_param[i] + QString("<%>");
            }
            line += QString::number(_pos_top) + QString("<%>") +  QString::number(_pos_cld) + QString("<%>\n");
            _prs_file.write(line.toUtf8());
            int column = _preset_table.size();  // Column count
            int row =  _preset_table[0].size();  // Row count
            if (row < 500 && column < PARAMETERS_COUNT+2)
            {
                for (int i = 0; i < row; i++)
                {
                    line = "";
                    for (int j = 0; j < column; j++)
                    {
                        line += _preset_table[j][i] + QString("<&>");
                    }
                    line += "\n";
                    _prs_file.write(line.toUtf8());
                }
            }
            _prs_file.close();
            std::cout << "Preset file saved..." << std::endl;  // Debug info //
        }

        // Save Main Widget
        _settings->beginGroup("MainWidget");
        _settings->setValue("MainWidget/geometry", this->saveGeometry());
        _settings->endGroup();

        // Save Main Window
        _settings->beginGroup("MainWindow");
        _settings->setValue("MainWindow/state", window->saveState());
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
        _settings->setValue("Settings/open_dir", _openDir);
        _settings->setValue("Settings/batch_mode", _batch_mode);
        _settings->setValue("Settings/tray", _hideInTrayFlag);
        /*
        _settings->beginWriteArray("Settings/cur_param");
        for (int i = 0; i < PARAMETERS_COUNT; i++)
        {
            _settings->setArrayIndex(i);
            _settings->setValue("parameter", _cur_param[i]);
        }
        _settings->endArray();
        _settings->setValue("Settings/preset/pos_top", _pos_top);
        _settings->setValue("Settings/preset/pos_cld", _pos_cld);
        */

        /*qRegisterMetaTypeStreamOperators<QList<int>>("QList<int>");
        QList<int> myList;
        for (int i = 0; i < PARAMETERS_COUNT; i++)
        {
            myList.append(i);
        }
        _settings->setValue("foo", QVariant::fromValue(myList));*/

        _settings->endGroup();

        //trayIcon->deleteLater();
        event->accept();
    }
}

void Widget::paintEvent(QPaintEvent *event) /*** Disable QTab draw base ***/
{
    if (event->type() == QEvent::Paint) {
        QList<QTabBar *> tabBars = findChildren<QTabBar *>("", Qt::FindChildrenRecursively);
        for (int var = 0; var < tabBars.size(); var++) {
            if (tabBars.at(var)->drawBase()) {
                 tabBars.at(var)->setDrawBase(false);
            }
        }
    }
}

void Widget::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason)
    {
        case QSystemTrayIcon::Trigger:
        case QSystemTrayIcon::DoubleClick:
            this->showNormal();
            break;
        default:
            break;
    }
}

void Widget::setTrayIconActions()
{
    minimizeAction = new QAction("Hide", this);
    restoreAction = new QAction("Show", this);
    quitAction = new QAction("Exit", this);

    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));
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
    //trayIcon->show();
}

void Widget::setParameters()    /*** Set parameters ***/
{
    // ***************************** Set parameters ***********************************//

    trayIcon = new QSystemTrayIcon(this);
    timer = new QTimer(this);
    timerCallSetThumbnail = new QTimer(this);
    timerCallSetThumbnail->setSingleShot(true);
    timerCallSetThumbnail->setInterval(800);
    connect(timer, SIGNAL(timeout()), this, SLOT(repeatHandler_Type_1()));
    connect(timerCallSetThumbnail, SIGNAL(timeout()), this, SLOT(repeatHandler_Type_2()));
    _preset_table.resize(PARAMETERS_COUNT+1);
    for (int i = 0; i < PARAMETERS_COUNT+1; i++) {
      _preset_table[i].resize(5);
    }

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
    menuView->addAction(dock1->toggleViewAction());
    menuView->addAction(dock2->toggleViewAction());
    menuView->addAction(dock3->toggleViewAction());
    menuView->addAction(dock4->toggleViewAction());
    menuView->addAction(dock5->toggleViewAction());
    menuView->addAction(dock6->toggleViewAction());
    ui->menuViewButton->setMenu(menuView);
    dock6->toggleViewAction()->setChecked(false);
    dock6->setVisible(false);

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

    // ****************************** Initialize variables ************************************//

    QDesktopWidget *screenSize = QApplication::desktop();
    int screenWidth = screenSize->width();
    int screenHeight = screenSize->height();
    int widthMainWindow = 1024;
    int heightMainWindow = 650;
    int x_pos = static_cast<int>(round(static_cast<float>(screenWidth - widthMainWindow)/2));
    int y_pos = static_cast<int>(round(static_cast<float>(screenHeight - heightMainWindow)/2));
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    _settingsWindowGeometry = "default";
    _presetWindowGeometry = "default";
    _openDir = QDir::homePath();
    _settings_path = QDir::homePath() + QString("/CineEncoder");
    _thumb_path = _settings_path + QString("/thumbnails");
    _settings_file = _settings_path + QString("/ce_settings");
    _preset_file = _settings_path + QString("/ce_preset_v3_3");
    _settings = new QSettings(_settings_path + QString("/ce_window.ini"), QSettings::IniFormat, this);
    _status_encode_btn = "start";
    _timer_interval = 30;
    _curTime = 0;
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
    _expandWindowsState = false;
    _hideInTrayFlag = false;
    clickPressedFlag = false;
    clickPressed_Left_ResizeFlag = false;
    clickPressed_Left_Top_ResizeFlag = false;
    clickPressed_Top_ResizeFlag = false;
    clickPressed_Right_Top_ResizeFlag = false;
    clickPressed_Right_ResizeFlag = false;
    clickPressed_Right_Bottom_ResizeFlag = false;
    clickPressed_Bottom_ResizeFlag = false;
    clickPressed_Left_Bottom_ResizeFlag = false;
    _protection = false;
    _batch_mode = false;
    _showHDR_mode = false;
    _row = -1;
    _theme = 0;
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
    process_1 = new QProcess(this);
    process_5 = new QProcess(this);
    process_1->setProcessChannelMode(QProcess::MergedChannels);
    process_1->setWorkingDirectory(QDir::homePath());

    // ****************************** Setup widgets ************************************//

    ui->labelAnimation->setMovie(animation);
    ui->labelAnimation->hide();
    ui->label_53->hide();
    ui->label_54->hide();
    ui->label_55->hide();
    ui->progressBar->hide();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->verticalHeader()->setFixedWidth(0);
    ui->tableWidget->setColumnWidth(columnIndex::FILENAME, 250);
    ui->tableWidget->setColumnWidth(columnIndex::FORMAT, 80);
    ui->tableWidget->setColumnWidth(columnIndex::RESOLUTION, 85);
    ui->tableWidget->setColumnWidth(columnIndex::DURATION, 70);
    ui->tableWidget->setColumnWidth(columnIndex::FPS, 70);
    ui->tableWidget->setColumnWidth(columnIndex::AR, 60);
    ui->tableWidget->setColumnWidth(columnIndex::STATUS, 80);

    for (int i = columnIndex::COLORRANGE; i <= columnIndex::MAXFALL; i++) {
        ui->tableWidget->setColumnWidth(i, 82);
    }

    for (int i = columnIndex::BITRATE; i <= columnIndex::COLORSPACE; i++) {
        ui->tableWidget->hideColumn(i);
    }

    for (int i = columnIndex::T_DUR; i <= columnIndex::T_ENDTIME; i++) {
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

    // ****************************** Read the files ************************************//

    _stn_file.setFileName(_settings_file);
    _prs_file.setFileName(_preset_file);

    if (_stn_file.exists())
    {
        std::cout << "Settings file exist ..." << std::endl;  // Debug info //
        if (_stn_file.open(QIODevice::ReadOnly | QIODevice::Text))   // Read settings from file
        {
            QStringList line;
            while(!_stn_file.atEnd()) {
                line << _stn_file.readLine();
            }

            std::cout << "Number of lines in settings file: " << line.size() << std::endl; // Debug info //
            if (line.size() == 10)
            {
                _temp_folder = line[0].replace("temp_folder:", "").replace("\n", "");
                _output_folder = line[1].replace("output_folder:", "").replace("\n", "");

                if (line[2].indexOf("true") != -1) {
                    _showHDR_mode = true;
                }

                if (line[3].indexOf("true") != -1) {
                    _protection = true;
                }

                _timer_interval = (line[4].replace("timer_interval:", "").replace("\n", "")).toInt();
                if (_timer_interval < 15) {
                    _timer_interval = 30;
                }
                timer->setInterval(_timer_interval*1000);

                _theme = (line[5].replace("theme:", "").replace("\n", "")).toInt();

                _prefxType = (line[6].replace("prefix_type:", "").replace("\n", "")).toInt();

                _suffixType = (line[7].replace("suffix_type:", "").replace("\n", "")).toInt();

                _prefixName = line[8].replace("prefix_name:", "").replace("\n", "");

                _suffixName = line[9].replace("suffix_name:", "").replace("\n", "");

            } else {
                std::cout << "Setting file error, not enough parameters!!! " << std::endl;  // Debug info //
            }
            std::cout << "Temp folder: " << _temp_folder.toStdString() << std::endl;  // Debug info //
            std::cout << "Output folder: " << _output_folder.toStdString() << std::endl;  // Debug info //
            std::cout << "Protection: " << _protection << std::endl;  // Debug info //
            std::cout << "Timer interval: " << _timer_interval << std::endl;  // Debug info //
            _stn_file.close();
        } else {
            std::cout << "Setting file error (cannot be open)." << std::endl;  // Debug info //
        }
    } else {
        std::cout << "Setting file not exist ..." << std::endl;  // Debug info //
    }

    if (_prs_file.exists())
    {
        std::cout << "Preset file exist ..." << std::endl;  // Debug info //
        if (_prs_file.open(QIODevice::ReadOnly | QIODevice::Text))   // Read preset from file
        {
            QStringList line;
            while(!_prs_file.atEnd()) {
                line << _prs_file.readLine();
            }

            std::cout << "Number of lines in preset file: " << line.size() << std::endl; // Debug info //
            if (line.size() > 0)
            {
                QStringList cur_param = line[0].split("<%>");
                if (cur_param.size() == PARAMETERS_COUNT+3)
                {
                    for (int k = 0; k < PARAMETERS_COUNT; k++) {
                        _cur_param[k] = cur_param[k];
                    }
                    _pos_top = cur_param[PARAMETERS_COUNT].toInt();
                    _pos_cld = cur_param[PARAMETERS_COUNT+1].toInt();
                    int n = line.size() - 1;
                    for (int i = 0; i < PARAMETERS_COUNT+1; i++) {
                      _preset_table[i].resize(n);
                    }
                    for (int j = 1; j <= n; j++) {
                        cur_param = line[j].split("<&>");
                        if (cur_param.size() == PARAMETERS_COUNT+2) {
                            for (int m = 0; m < PARAMETERS_COUNT+1; m++) {
                                _preset_table[m][j-1] = cur_param[m];
                            }
                        } else {
                            std::cout << "Preset column size: " << cur_param.size() << ". Error!!! Break!!!" << std::endl;  // Debug info //
                            break;
                        }
                    }
                } else {
                    std::cout << "Preset file error, uncorrect parameters!!! " << std::endl;  // Debug info //
                    set_defaults();
                }
            } else {
                std::cout << "Preset file error, not enough parameters!!! " << std::endl;  // Debug info //
                set_defaults();
            }
            _prs_file.close();
        } else {
            std::cout << "Preset file error (cannot be open)." << std::endl;  // Debug info //
            set_defaults();
        }
    } else {
        std::cout << "Preset file not exist ..." << std::endl;  // Debug info //
        set_defaults();
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
    QIcon sectionIcon;
    sectionIcon.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-folder.png"), QSize(), QIcon::Normal, QIcon::Off);
    for (int i = 0; i < NUM_ROWS; i++) {
        type = _preset_table[PARAMETERS_COUNT][i];
        if (type == "TopLewelItem") {
            QTreeWidgetItem *root = new QTreeWidgetItem();
            root->setText(0, _preset_table[0][i]);
            root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
            root->setFont(0, parentFont);
            root->setIcon(0, sectionIcon);
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
            updateInfoFields(_preset_table[1][i], _preset_table[2][i], _preset_table[3][i],
                                  _preset_table[4][i], _preset_table[11][i], _preset_table[12][i],
                                  _preset_table[21][i], child);
            setItemStyle(child);
            item->addChild(child);
        }
    }
    if (_pos_top != -1 && _pos_cld != -1) {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(_pos_top)->child(_pos_cld);
        ui->treeWidget->setCurrentItem(item);
    }
    std::cout << NUM_ROWS << " x " << NUM_COLUMNS << std::endl; // Table size
    for (int i = 7; i <= 29; i++) {
        ui->treeWidget->hideColumn(i);
    }

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

    // ************************** Set theme and geometry ******************************//

    if (_settings->childGroups().contains("MainWidget", Qt::CaseInsensitive)) {
        std::cout << "Key exists" << std::endl;

        // Restore Main Widget
        _settings->beginGroup("MainWidget");
        this->restoreGeometry(_settings->value("MainWidget/geometry").toByteArray());
        _settings->endGroup();

        // Restore Main Window
        _settings->beginGroup("MainWindow");
        window->restoreState(_settings->value("MainWindow/state").toByteArray());
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
        _openDir = _settings->value("Settings/open_dir").toString();
        _batch_mode = _settings->value("Settings/batch_mode").toBool();
        _hideInTrayFlag = _settings->value("Settings/tray").toBool();
        /*int arraySize = _settings->beginReadArray("Settings/cur_param");
        for (int i = 0; i < arraySize; i++)
        {
            _settings->setArrayIndex(i);
            _cur_param[i] = _settings->value("parameter").toString();
        }
        _settings->endArray();
        _pos_top = _settings->value("Settings/preset/pos_top").toInt;
        _pos_cld = _settings->value("Settings/preset/pos_cld").toInt;*/

        /*QList<int> myList = _settings->value("foo").value<QList<int>>();
        qDebug() << myList;*/

        _settings->endGroup();

    } else {
        std::cout << "Key doesn't exist" << std::endl;
        this->setGeometry(x_pos, y_pos, widthMainWindow, heightMainWindow);
        setDocksParameters();
    }

    if (this->isMaximized()) {
        _expandWindowsState = true;
        //setExpandIcon();
    }

    if (_batch_mode) {
        ui->comboBoxMode->blockSignals(true);
        ui->comboBoxMode->setCurrentIndex(1);
        ui->comboBoxMode->blockSignals(false);
    }
    setTrayIconActions();
    showTrayIcon();
    if (_hideInTrayFlag) {
        trayIcon->show();
    }
    setTheme(_theme);
}

void Widget::setDocksParameters()
{
    // ************************** Set Docks Parameters ********************************//

    int windowWidth = window->size().width();
    int windowHeight = window->size().height();

    int dock1Width = static_cast<int>(0.25f * windowWidth);
    int dock5Width = static_cast<int>(0.25f * windowWidth);

    int dock2Width = static_cast<int>(0.08f * windowWidth);
    int dock3Width = static_cast<int>(0.46f * windowWidth);
    int dock4Width = static_cast<int>(0.46f * windowWidth);

    int dock1Height = static_cast<int>(0.9f * windowHeight);
    int dock2Height = static_cast<int>(0.1f * windowHeight);

    window->resizeDocks({dock1, dock2, dock3, dock4, dock5},
                        {dock1Width, dock2Width, dock3Width, dock4Width, dock5Width}, Qt::Horizontal);
    window->resizeDocks({dock1, dock2, dock3, dock4, dock5},
                        {dock1Height, dock2Height, dock2Height, dock2Height, dock1Height}, Qt::Vertical);
}

void Widget::on_closeWindow_clicked()    /*** Close window signal ***/
{
    this->close();
}

void Widget::setExpandIcon()
{
    switch (_theme)
    {
        case 0:
        case 1:
        case 2:
            if (_expandWindowsState) {
                ui->expandWindow->setIcon(QIcon(":/resources/icons/16x16/cil-clone.png"));
            } else {
                ui->expandWindow->setIcon(QIcon(":/resources/icons/16x16/cil-media-stop.png"));}
            break;
        case 3:
            if (_expandWindowsState) {
                ui->expandWindow->setIcon(QIcon(":/resources/icons/16x16/cil-clone_black.png"));
            } else {
                ui->expandWindow->setIcon(QIcon(":/resources/icons/16x16/cil-media-stop_black.png"));}
            break;
    }
}

void Widget::on_expandWindow_clicked()    /*** Expand window ***/
{
    if (_expandWindowsState == false)
    {
        this->showMaximized();
        _expandWindowsState = true;
    } else {
        this->showNormal();
        _expandWindowsState = false;
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

void Widget::on_actionSettings_clicked()    /*** Settings ***/
{
    Settings settings(this);
    settings.setParameters(&_settingsWindowGeometry, &_stn_file, &_output_folder,
                           &_temp_folder, &_protection, &_showHDR_mode, &_timer_interval, &_theme,
                           &_prefixName, &_suffixName, &_prefxType, &_suffixType, &_hideInTrayFlag);
    settings.setModal(true);
    settings.exec();
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
}

void Widget::showMetadataEditor()
{
    if (ui->frameRight->isHidden()) {
        ui->frameRight->show();
    }
    ui->tabWidgetRight->setCurrentIndex(0);
}

void Widget::showAudioStreamsSelection()
{
    if (ui->frameRight->isHidden()) {
        ui->frameRight->show();
    }
    ui->tabWidgetRight->setCurrentIndex(1);
}

void Widget::showSubtitlesSelection()
{
    if (ui->frameRight->isHidden()) {
        ui->frameRight->show();
    }
    ui->tabWidgetRight->setCurrentIndex(2);
}

void Widget::showVideoSplitter()
{
    if (ui->frameRight->isHidden()) {
        ui->frameRight->show();
    }
    ui->tabWidgetRight->setCurrentIndex(3);
}

void Widget::get_current_data() /*** Get current data ***/
{
    _dur = (ui->tableWidget->item(_row, columnIndex::T_DUR)->text()).toDouble();
    _stream_size = ui->tableWidget->item(_row, columnIndex::T_STREAMSIZE)->text();
    _width = ui->tableWidget->item(_row, columnIndex::T_WIDTH)->text();
    _height = ui->tableWidget->item(_row, columnIndex::T_HEIGHT)->text();
    _fmt = ui->tableWidget->item(_row, columnIndex::FORMAT)->text();
    _fps = ui->tableWidget->item(_row, columnIndex::FPS)->text();

    double fps_double = _fps.toDouble();
    _fr_count = static_cast<int>(round(_dur * fps_double));

    _startTime = (ui->tableWidget->item(_row, columnIndex::T_STARTTIME)->text()).toDouble();
    _endTime = (ui->tableWidget->item(_row, columnIndex::T_ENDTIME)->text()).toDouble();

    //std::cout << "DURATION SEC COUNT <<<<<<<<<<<<<<<<<< : " << _dur << std::endl;
    //std::cout << "FRAME RATE COUNT <<<<<<<<<<<<<<<<<< : " << _fr_count << std::endl;

    QString curCodec = _fmt;
    QString curFps = _fps;
    QString curRes = ui->tableWidget->item(_row, columnIndex::RESOLUTION)->text();
    QString curAr = ui->tableWidget->item(_row, columnIndex::AR)->text();
    QString curBitrate = ui->tableWidget->item(_row, columnIndex::BITRATE)->text();
    QString curColorSampling = ui->tableWidget->item(_row, columnIndex::SUBSAMPLING)->text();
    QString curDepth = ui->tableWidget->item(_row, columnIndex::BITDEPTH)->text();
    QString curSpace = ui->tableWidget->item(_row, columnIndex::COLORSPACE)->text();

    _curPath = ui->tableWidget->item(_row, columnIndex::PATH)->text();
    _curFilename = ui->tableWidget->item(_row, columnIndex::FILENAME)->text();
    _input_file = _curPath + QString("/") + _curFilename;
    _hdr[CUR_COLOR_RANGE] = ui->tableWidget->item(_row, columnIndex::COLORRANGE)->text();       // color range
    _hdr[CUR_COLOR_PRIMARY] = ui->tableWidget->item(_row, columnIndex::COLORPRIM)->text();      // color primary
    _hdr[CUR_COLOR_MATRIX] = ui->tableWidget->item(_row, columnIndex::COLORMATRIX)->text();     // color matrix
    _hdr[CUR_TRANSFER] = ui->tableWidget->item(_row, columnIndex::TRANSFER)->text();            // transfer
    _hdr[CUR_MAX_LUM] = ui->tableWidget->item(_row, columnIndex::MAXLUM)->text();               // max lum
    _hdr[CUR_MIN_LUM] = ui->tableWidget->item(_row, columnIndex::MINLUM)->text();               // min lum
    _hdr[CUR_MAX_CLL] = ui->tableWidget->item(_row, columnIndex::MAXCLL)->text();               // max cll
    _hdr[CUR_MAX_FALL] = ui->tableWidget->item(_row, columnIndex::MAXFALL)->text();             // max fall
    _hdr[CUR_MASTER_DISPLAY] = ui->tableWidget->item(_row, columnIndex::MASTERDISPLAY)->text(); // master display
    _hdr[CUR_CHROMA_COORD] = ui->tableWidget->item(_row, columnIndex::T_CHROMACOORD)->text();   // chr coord
    _hdr[CUR_WHITE_COORD] = ui->tableWidget->item(_row, columnIndex::T_WHITECOORD)->text();     // white coord

    _videoMetadata[VIDEO_TITLE] = ui->tableWidget->item(_row, columnIndex::T_VIDEOTITLE)->text();
    _videoMetadata[VIDEO_AUTHOR] = ui->tableWidget->item(_row, columnIndex::T_VIDEOAUTHOR)->text();
    _videoMetadata[VIDEO_YEAR] = ui->tableWidget->item(_row, columnIndex::T_VIDEOYEAR)->text();
    _videoMetadata[VIDEO_PERFORMER] = ui->tableWidget->item(_row, columnIndex::T_VIDEOPERF)->text();
    _videoMetadata[VIDEO_DESCRIPTION] = ui->tableWidget->item(_row, columnIndex::T_VIDEODESCR)->text();
    _videoMetadata[VIDEO_MOVIENAME] = ui->tableWidget->item(_row, columnIndex::T_VIDEOMOVIENAME)->text();

    for (int p = 0; p < AMOUNT_AUDIO_STREAMS; p++) {
        _audioStreamCheckState[p] = (ui->tableWidget->item(_row, p + columnIndex::T_AUDIOCHECK_1)->text()).toInt();
        _audioLang[p] = ui->tableWidget->item(_row, p + columnIndex::T_AUDIOLANG_1)->text();
        _audioTitle[p] = ui->tableWidget->item(_row, p + columnIndex::T_AUDIOTITLE_1)->text();
    }

    for (int p = 0; p < AMOUNT_SUBTITLES; p++) {
        _subtitleCheckState[p] = (ui->tableWidget->item(_row, p + columnIndex::T_SUBCHECK_1)->text()).toInt();
        _subtitleLang[p] = ui->tableWidget->item(_row, p + columnIndex::T_SUBLANG_1)->text();
        _subtitleTitle[p] = ui->tableWidget->item(_row, p + columnIndex::T_TITLESUB_1)->text();
    }

    double halfTime = _dur/2;
    setThumbnail(_curFilename, halfTime, "high");
    ui->label_source->setText(ui->tableWidget->item(_row, columnIndex::FILENAME)->text());

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
        curDepth += " bit, ";
    }
    if (curBitrate != "") {
        curBitrate += " kbps; ";
    }
    QString sourceParam = curCodec + curRes + curFps + curAr + curSpace + curColorSampling + curDepth + curBitrate;

    int countAudioStreams = 0;
    QString curAudioStream("");
    while (countAudioStreams < AMOUNT_AUDIO_STREAMS) {
        curAudioStream = ui->tableWidget->item(_row, countAudioStreams + columnIndex::T_AUDIO_1)->text();
        if (curAudioStream == "") {
            break;
        }
        countAudioStreams++;
        sourceParam += QString("Audio #") + QString::number(countAudioStreams) + QString(": ") + curAudioStream + QString("; ");
    }

    int countSubtitles = 0;
    QString curSubtitle("");
    while (countSubtitles < AMOUNT_SUBTITLES) {
        curSubtitle = ui->tableWidget->item(_row, countSubtitles + columnIndex::T_SUBTITLE_1)->text();
        if (curSubtitle == "") {
            break;
        }
        countSubtitles++;
    }


    if (curCodec == "Undef, ") {
        sourceParam = "Undefined";
    }
    ui->textBrowser_1->clear();
    ui->textBrowser_1->setText(sourceParam);
    get_output_filename();

    //******************************** Set time widgets *****************************//

    ui->horizontalSlider->setMaximum(_fr_count);
    ui->lineEditStartTime->setText(timeConverter(_startTime));
    ui->lineEditEndTime->setText(timeConverter(_endTime));

    //******************************** Set video widgets *****************************//

    ui->lineEditTitleVideo->setEnabled(true);
    ui->lineEditAuthorVideo->setEnabled(true);
    ui->lineEditYearVideo->setEnabled(true);
    ui->lineEditPerfVideo->setEnabled(true);
    ui->lineEditDescriptionVideo->setEnabled(true);
    ui->lineEditMovieNameVideo->setEnabled(true);

    ui->lineEditTitleVideo->setText(_videoMetadata[VIDEO_TITLE]);
    ui->lineEditAuthorVideo->setText(_videoMetadata[VIDEO_AUTHOR]);
    ui->lineEditYearVideo->setText(_videoMetadata[VIDEO_YEAR]);
    ui->lineEditPerfVideo->setText(_videoMetadata[VIDEO_PERFORMER]);
    ui->lineEditDescriptionVideo->setText(_videoMetadata[VIDEO_DESCRIPTION]);
    ui->lineEditMovieNameVideo->setText(_videoMetadata[VIDEO_MOVIENAME]);

    ui->lineEditTitleVideo->setCursorPosition(0);
    ui->lineEditAuthorVideo->setCursorPosition(0);
    ui->lineEditYearVideo->setCursorPosition(0);
    ui->lineEditPerfVideo->setCursorPosition(0);
    ui->lineEditDescriptionVideo->setCursorPosition(0);
    ui->lineEditMovieNameVideo->setCursorPosition(0);

    //******************************** Set audio widgets *****************************//

    QList <QCheckBox *> checkBoxAudio;
    checkBoxAudio.append(ui->checkBoxAudio_1);
    checkBoxAudio.append(ui->checkBoxAudio_2);
    checkBoxAudio.append(ui->checkBoxAudio_3);
    checkBoxAudio.append(ui->checkBoxAudio_4);
    checkBoxAudio.append(ui->checkBoxAudio_5);
    checkBoxAudio.append(ui->checkBoxAudio_6);
    checkBoxAudio.append(ui->checkBoxAudio_7);
    checkBoxAudio.append(ui->checkBoxAudio_8);
    checkBoxAudio.append(ui->checkBoxAudio_9);

    QList <QLineEdit *> lineEditLangAudio;
    lineEditLangAudio.append(ui->lineEditLangAudio_1);
    lineEditLangAudio.append(ui->lineEditLangAudio_2);
    lineEditLangAudio.append(ui->lineEditLangAudio_3);
    lineEditLangAudio.append(ui->lineEditLangAudio_4);
    lineEditLangAudio.append(ui->lineEditLangAudio_5);
    lineEditLangAudio.append(ui->lineEditLangAudio_6);
    lineEditLangAudio.append(ui->lineEditLangAudio_7);
    lineEditLangAudio.append(ui->lineEditLangAudio_8);
    lineEditLangAudio.append(ui->lineEditLangAudio_9);

    QList <QLineEdit *> lineEditTitleAudio;
    lineEditTitleAudio.append(ui->lineEditTitleAudio_1);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_2);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_3);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_4);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_5);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_6);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_7);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_8);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_9);

    for (int q = 0; q < AMOUNT_AUDIO_STREAMS; q++) {
        checkBoxAudio.at(q)->setChecked(bool(_audioStreamCheckState[q]));
        lineEditLangAudio.at(q)->setText(_audioLang[q]);
        lineEditLangAudio.at(q)->setCursorPosition(0);
        lineEditTitleAudio.at(q)->setText(_audioTitle[q]);
        lineEditTitleAudio.at(q)->setCursorPosition(0);
    }

    for (int q = 0; q < countAudioStreams; q++) {
        checkBoxAudio.at(q)->setEnabled(true);
        lineEditLangAudio.at(q)->setEnabled(true);
        lineEditTitleAudio.at(q)->setEnabled(true);
    }

    //******************************** Set subtitle widgets *****************************//

    QList <QCheckBox *> checkBoxSubtitle;
    checkBoxSubtitle.append(ui->checkBoxSubtitle_1);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_2);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_3);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_4);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_5);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_6);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_7);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_8);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_9);

    QList <QLineEdit *> lineEditLangSubtitle;
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_1);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_2);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_3);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_4);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_5);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_6);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_7);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_8);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_9);

    QList <QLineEdit *> lineEditTitleSubtitle;
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_1);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_2);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_3);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_4);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_5);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_6);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_7);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_8);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_9);

    for (int q = 0; q < AMOUNT_SUBTITLES; q++) {
        checkBoxSubtitle.at(q)->setChecked(bool(_subtitleCheckState[q]));
        lineEditLangSubtitle.at(q)->setText(_subtitleLang[q]);
        lineEditLangSubtitle.at(q)->setCursorPosition(0);
        lineEditTitleSubtitle.at(q)->setText(_subtitleTitle[q]);
        lineEditTitleSubtitle.at(q)->setCursorPosition(0);
    }

    for (int q = 0; q < countSubtitles; q++) {
        checkBoxSubtitle.at(q)->setEnabled(true);
        lineEditLangSubtitle.at(q)->setEnabled(true);
        lineEditTitleSubtitle.at(q)->setEnabled(true);
    }
}

void Widget::setTheme(int &ind_theme)   /*** Set theme ***/
{
    QFile file;
    QString list("");
    switch (ind_theme)
    {
        case 0:
            file.setFileName(":/resources/css/style_0.css");
            raiseThumb->setStyleSheet("color: #09161E; font: 64pt; font-style: oblique;");
            break;
        case 1:
            file.setFileName(":/resources/css/style_1.css");
            raiseThumb->setStyleSheet("color: #09161E; font: 64pt; font-style: oblique;");
            break;
        case 2:
            file.setFileName(":/resources/css/style_2.css");
            raiseThumb->setStyleSheet("color: #09161E; font: 64pt; font-style: oblique;");
            break;
        case 3:
            file.setFileName(":/resources/css/style_3.css");
            raiseThumb->setStyleSheet("color: #E3E3E3; font: 64pt; font-style: oblique;");
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

void Widget::restore_initial_state()    /*** Restore initial state ***/
{
    animation->stop();
    add_files->setEnabled(true);
    remove_files->setEnabled(true);
    //select_preset->setEnabled(true);
    /*edit_metadata->setEnabled(true);
    select_audio->setEnabled(true);
    select_subtitles->setEnabled(true);
    split_video->setEnabled(true);*/
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

    ui->actionSettings->setEnabled(true);
    _status_encode_btn = "start";
    ui->actionEncode->setIcon(QIcon(":/resources/icons/16x16/cil-play.png"));
    ui->actionEncode->setToolTip("Encode");
}

bool Widget::eventFilter(QObject *watched, QEvent *event)    /*** Resize and move window ***/
{
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

    if (watched == ui->centralwidget) // *************** Resize window realisation ************************* //
    {
        if (!this->isMaximized())
        {
            if (event->type() == QEvent::HoverLeave)
            {
                QGuiApplication::restoreOverrideCursor();
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
                mouseCoordinate = ui->centralwidget->mapFromGlobal(QCursor::pos());
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

    if (watched == raiseThumb) // ************** Click thumb realisation ************** //
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton)
            {
                on_actionAdd_clicked();
                return true;
            }
            return false;
        }
        return false;
    }
    return QWidget::eventFilter(watched, event);
}

/************************************************
** Encoder
************************************************/

void Widget::make_preset()  /*** Make preset ***/
{
    std::cout << "Make preset..." << std::endl;                       // Debug information //
    int _CODEC = _cur_param[curParamIndex::CODEC].toInt();                           //p1 - int codec
    int _MODE = _cur_param[curParamIndex::MODE].toInt();                             //p2 - int mode
    QString _BQR = _cur_param[curParamIndex::BQR];                                   //p4 - QString bitrate/quantizer/ratefactor
    QString _MINRATE = _cur_param[curParamIndex::MINRATE];                           //p5-1 - QString minrate
    QString _MAXRATE = _cur_param[curParamIndex::MAXRATE];                           //p5 - QString maxrate
    QString _BUFSIZE = _cur_param[curParamIndex::BUFSIZE];                           //p6 - QString bufsize
    int _LEVEL = _cur_param[curParamIndex::LEVEL].toInt();
    int _FRAME_RATE = _cur_param[curParamIndex::FRAME_RATE].toInt();                 //p7 - int fps
    int _BLENDING = _cur_param[curParamIndex::BLENDING].toInt();                     //p8 - int blending
    int _WIDTH = _cur_param[curParamIndex::WIDTH].toInt();                           //p9 - int width
    int _HEIGHT = _cur_param[curParamIndex::HEIGHT].toInt();                         //p10 - int height
    int _PASS = _cur_param[curParamIndex::PASS].toInt();                             //p11 - int pass
    int _PRESET = _cur_param[curParamIndex::PRESET].toInt();                         //p12 - int preset
    int _COLOR_RANGE = _cur_param[curParamIndex::COLOR_RANGE].toInt();               //p13 - int color_range
    int _MATRIX = _cur_param[curParamIndex::MATRIX].toInt();
    int _PRIMARY = _cur_param[curParamIndex::PRIMARY].toInt();
    int _TRC = _cur_param[curParamIndex::TRC].toInt();
    QString _MIN_LUM = _cur_param[curParamIndex::MIN_LUM];                           //p14 - QString min_lum
    QString _MAX_LUM = _cur_param[curParamIndex::MAX_LUM];                           //p15 - QString max_lum
    QString _MAX_CLL = _cur_param[curParamIndex::MAX_CLL];                           //p16 - QString max_cll
    QString _MAX_FALL = _cur_param[curParamIndex::MAX_FALL];                         //p17 - QString max_fall
    int _MASTER_DISPLAY = _cur_param[curParamIndex::MASTER_DISPLAY].toInt();         //p18 - int master_disp
    QString _CHROMA_COORD = _cur_param[curParamIndex::CHROMA_COORD];                 //p19 - QString chroma_coord
    QString _WHITE_COORD = _cur_param[curParamIndex::WHITE_COORD];                   //p20 - QString white_coord
    int _AUDIO_CODEC = _cur_param[curParamIndex::AUDIO_CODEC].toInt();               //p21 - int acodec
    int _AUDIO_BITRATE = _cur_param[curParamIndex::AUDIO_BITRATE].toInt();           //p22 - int abitrate
    int _AUDIO_SAMPLING = _cur_param[curParamIndex::ASAMPLE_RATE].toInt();
    int _AUDIO_CHANNELS = _cur_param[curParamIndex::ACHANNELS].toInt();

    _preset_0 = "";
    _preset_pass1 = "";
    _preset = "";
    _preset_mkvmerge = "";
    _error_message = "";
    _flag_two_pass = false;
    _flag_hdr = false;
    _mux_mode = false;
    _fr_count = 0;
    ui->textBrowser_log->clear();

    /****************************************** Resize ****************************************/

    QString width[16] = {
        "Source", "7680", "4520", "4096", "3840", "3656", "2048", "1920",
        "1828",   "1440", "1280", "1024", "768",  "720",  "640",  "320"
    };

    QString height[32] = {
        "Source", "4320", "3112", "3072", "2664", "2540", "2468", "2304",
        "2214",   "2204", "2160", "2056", "1976", "1744", "1556", "1536",
        "1332",   "1234", "1152", "1107", "1102", "1080", "1028", "988",
        "872",    "768",  "720",  "576",  "540",  "486",  "480",  "240"
    };

    QString resize_vf = "";
    if ((width[_WIDTH] != "Source") && (height[_HEIGHT] != "Source")) {
        resize_vf = QString("scale=%1:%2,setsar=1:1").arg(width[_WIDTH], height[_HEIGHT]);
    }
    else if ((width[_WIDTH] != "Source") && (height[_HEIGHT] == "Source")) {
        resize_vf = QString("scale=%1:%2,setsar=1:1").arg(width[_WIDTH], _height);
    }
    else if ((width[_WIDTH] == "Source") && (height[_HEIGHT] != "Source")) {
        resize_vf = QString("scale=%1:%2,setsar=1:1").arg(_width, height[_HEIGHT]);
    }

    /******************************************* FPS *****************************************/

    QString frame_rate[14] = {
        "Source", "120", "60", "59.940", "50", "48", "30",
        "29.970", "25",  "24", "23.976", "20", "18", "16"
    };

    QString blending[4] = {
        "Simple", "Interpolated", "MCI", "Blend"
    };

    QString fps_vf = "";
    double fps_dest;

    if (frame_rate[_FRAME_RATE] != "Source") {
        fps_dest = frame_rate[_FRAME_RATE].toDouble();
        if (blending[_BLENDING] == "Simple") {
            fps_vf = QString("fps=fps=%1").arg(frame_rate[_FRAME_RATE]);
        }
        else if (blending[_BLENDING] == "Interpolated") {
            fps_vf = QString("framerate=fps=%1").arg(frame_rate[_FRAME_RATE]);
        }
        else if (blending[_BLENDING] == "MCI") {
            fps_vf = QString("minterpolate=fps=%1:mi_mode=mci:mc_mode=aobmc:me_mode=bidir:vsbmc=1")
                    .arg(frame_rate[_FRAME_RATE]);
        }
        else if (blending[_BLENDING] == "Blend") {
            fps_vf = QString("minterpolate=fps=%1:mi_mode=blend").arg(frame_rate[_FRAME_RATE]);
        }
    } else {
        fps_dest = _fps.toDouble();
    }

    QString transform = "";
    if (resize_vf != "" && fps_vf != "") {
        transform = QString("-vf ") + fps_vf + "," + resize_vf + QString(" ");
    }
    else if (resize_vf != "" && fps_vf == "") {
        transform = QString("-vf ") + resize_vf + QString(" ");
    }
    else if (resize_vf == "" && fps_vf != "") {
        transform = QString("-vf ") + fps_vf + QString(" ");
    }

    /****************************************** Split ****************************************/

    QString _splitStartParam = "";
    QString _splitParam = "";

    if (_endTime > 0 && _startTime < _endTime) {
        double duration = _endTime - _startTime;
        _fr_count = static_cast<int>(round(duration * fps_dest));
        int startFrame = static_cast<int>(round(_startTime * fps_dest));
        int endFrame = static_cast<int>(round(_endTime * fps_dest));
        int amountFrames = endFrame - startFrame;
        _splitStartParam = QString(" -ss %1").arg(QString::number(_startTime, 'f', 3));
        _splitParam = QString("-vframes %1 ").arg(QString::number(amountFrames));
    } else {
        _fr_count = static_cast<int>(round(_dur * fps_dest));
    }

    /************************************** Video metadata ************************************/

    QString videoMetadata[6] = {"", "", "", "", "", ""};
    QString _videoMetadataParam = "";
    QString globalTitle = ui->lineEditGlobalTitle->text();

    if (globalTitle != "") {
        videoMetadata[0] = QString("-metadata:s:v:0 title=%1 ").arg(globalTitle.replace(" ", "\u00A0"));
    } else {
        if (_videoMetadata[VIDEO_TITLE] != "") {
            videoMetadata[0] = QString("-metadata:s:v:0 title=%1 ").arg(_videoMetadata[VIDEO_TITLE]
                                                                        .replace(" ", "\u00A0"));
        } else {
            videoMetadata[0] = QString("-map_metadata:s:v:0 -1 ");
        }
    }
    if (_videoMetadata[VIDEO_MOVIENAME] != "") {
        videoMetadata[1] = QString("-metadata title=%1 ").arg(_videoMetadata[VIDEO_MOVIENAME]
                                                              .replace(" ", "\u00A0"));
    }
    if (_videoMetadata[VIDEO_AUTHOR] != "") {
        videoMetadata[2] = QString("-metadata author=%1 ").arg(_videoMetadata[VIDEO_AUTHOR]
                                                               .replace(" ", "\u00A0"));
    }
    if (_videoMetadata[VIDEO_DESCRIPTION] != "") {
        videoMetadata[3] = QString("-metadata description=%1 ").arg(_videoMetadata[VIDEO_DESCRIPTION]
                                                                    .replace(" ", "\u00A0"));
    }
    if (_videoMetadata[VIDEO_YEAR] != "") {
        videoMetadata[4] = QString("-metadata year=%1 ").arg(_videoMetadata[VIDEO_YEAR].replace(" ", ""));
    }
    if (_videoMetadata[VIDEO_PERFORMER] != "") {
        videoMetadata[5] = QString("-metadata author=%1 ").arg(_videoMetadata[VIDEO_PERFORMER]
                                                               .replace(" ", "\u00A0"));
    }
    for (int i = 0; i < 6; i++) {
        _videoMetadataParam += videoMetadata[i];
    }

    /************************************** Audio streams ************************************/

    QString audioLang[AMOUNT_AUDIO_STREAMS] = {"", "", "", "", "", "", "", "", ""};
    QString audioTitle[AMOUNT_AUDIO_STREAMS] = {"", "", "", "", "", "", "", "", ""};
    QString audioMap[AMOUNT_AUDIO_STREAMS] = {"", "", "", "", "", "", "", "", ""};
    QString _audioMapParam = "";
    QString _audioMetadataParam = "";
    int countDestAudioStream = 0;

    for (int k = 0; k < AMOUNT_AUDIO_STREAMS; k++) {
        if (_audioStreamCheckState[k] == 1) {
            audioMap[k] = QString("-map 0:a:%1? ").arg(QString::number(k));
            audioLang[k] = QString("-metadata:s:a:%1 language=%2 ")
                           .arg(QString::number(countDestAudioStream), _audioLang[k].replace(" ", "\u00A0"));
            audioTitle[k] = QString("-metadata:s:a:%1 title=%2 ")
                            .arg(QString::number(countDestAudioStream), _audioTitle[k].replace(" ", "\u00A0"));
            countDestAudioStream++;
        }
        _audioMapParam += audioMap[k];
        _audioMetadataParam += audioLang[k] + audioTitle[k];
    }

    /**************************************** Subtitles **************************************/

    QString subtitleLang[AMOUNT_SUBTITLES] = {"", "", "", "", "", "", "", "", ""};
    QString subtitleTitle[AMOUNT_SUBTITLES] = {"", "", "", "", "", "", "", "", ""};
    QString subtitleMap[AMOUNT_SUBTITLES] = {"", "", "", "", "", "", "", "", ""};
    QString _subtitleMapParam = "";
    QString _subtitleMetadataParam = "";
    int countDestSubtitleStream = 0;

    for (int k = 0; k < AMOUNT_SUBTITLES; k++) {
        if (_subtitleCheckState[k] == 1) {
            subtitleMap[k] = QString("-map 0:s:%1? ").arg(QString::number(k));
            subtitleLang[k] = QString("-metadata:s:s:%1 language=%2 ")
                           .arg(QString::number(countDestSubtitleStream), _subtitleLang[k].replace(" ", "\u00A0"));
            subtitleTitle[k] = QString("-metadata:s:s:%1 title=%2 ")
                            .arg(QString::number(countDestSubtitleStream), _subtitleTitle[k].replace(" ", "\u00A0"));
            countDestSubtitleStream++;
        }
        _subtitleMapParam += subtitleMap[k];
        _subtitleMetadataParam += subtitleLang[k] + subtitleTitle[k];
    }

    /*********************************** Intel QSV presets ************************************/

    QString intelQSV_H264_preset = "-vf hwmap=derive_device=qsv,format=qsv -c:v h264_qsv -profile:v high "
                                   "-low_power false ";
    QString intelQSV_Mpeg2_preset = "-vf hwmap=derive_device=qsv,format=qsv -c:v mpeg2_qsv -profile:v high "
                                    "-low_power false ";
#ifdef Q_OS_WIN64
    QString intelQSVhwaccel = " -hwaccel dxva2 -hwaccel_output_format dxva2_vld";
#endif

#ifdef Q_OS_LINUX
    QString intelQSVhwaccel = " -hwaccel vaapi -hwaccel_output_format vaapi";
#endif

    /************************************* XDCAM presets **************************************/

    QString xdcam_preset = "-pix_fmt yuv422p -c:v mpeg2video -profile:v 0 -flags ilme -top 1 "
                           "-metadata creation_time=now -vtag xd5c -timecode 01:00:00:00 ";

    /************************************* Codec module ***************************************/

    QString arr_codec[NUMBER_PRESETS][3] = {
        {"-pix_fmt yuv420p10le -c:v libx265 -profile:v main10 ",        "",               "1"},
        {"-pix_fmt yuv420p -c:v libx265 -profile:v main ",              "",               "0"},
        {"-pix_fmt yuv420p -c:v libx264 -profile:v high ",              "",               "0"},
        {intelQSV_H264_preset,                                          intelQSVhwaccel,  "0"},
        {intelQSV_Mpeg2_preset,                                         intelQSVhwaccel,  "0"},
        {"-pix_fmt p010le -c:v hevc_nvenc -profile:v main10 ",          " -hwaccel cuda", "1"},
        {"-pix_fmt yuv420p -c:v hevc_nvenc -profile:v main ",           " -hwaccel cuda", "0"},
        {"-pix_fmt yuv420p -c:v h264_nvenc -profile:v high ",           " -hwaccel cuda", "0"},
        {"-pix_fmt yuv420p10le -c:v libvpx-vp9 -speed 4 -profile:v 2 ", "",               "1"},
        {"-pix_fmt yuv420p -c:v libvpx-vp9 -speed 4 ",                  "",               "0"},
        {"-pix_fmt yuv422p10le -c:v prores_ks -profile:v 0 ",           "",               "1"},
        {"-pix_fmt yuv422p10le -c:v prores_ks -profile:v 1 ",           "",               "1"},
        {"-pix_fmt yuv422p10le -c:v prores_ks -profile:v 2 ",           "",               "1"},
        {"-pix_fmt yuv422p10le -c:v prores_ks -profile:v 3 ",           "",               "1"},
        {"-pix_fmt yuv444p10le -c:v prores_ks -profile:v 4 ",           "",               "1"},
        {"-pix_fmt yuv444p10le -c:v prores_ks -profile:v 5 ",           "",               "1"},
        {"-pix_fmt yuv422p -c:v dnxhd -profile:v dnxhr_lb ",            "",               "0"},
        {"-pix_fmt yuv422p -c:v dnxhd -profile:v dnxhr_sq ",            "",               "0"},
        {"-pix_fmt yuv422p -c:v dnxhd -profile:v dnxhr_hq ",            "",               "0"},
        {"-pix_fmt yuv422p10le -c:v dnxhd -profile:v dnxhr_hqx ",       "",               "1"},
        {"-pix_fmt yuv444p10le -c:v dnxhd -profile:v dnxhr_444 ",       "",               "1"},
        {xdcam_preset,                                                  "",               "0"},
        {"-movflags +write_colr -c:v copy ",                            "",               "1"}
    };
    QString codec = QString("-map 0:v:0? ") + _audioMapParam + _subtitleMapParam +
                    QString("-map_metadata -1 ") + _videoMetadataParam + _audioMetadataParam +
                    _subtitleMetadataParam + arr_codec[_CODEC][0];

    QString hwaccel = arr_codec[_CODEC][1];
    _flag_hdr = static_cast<bool>(arr_codec[_CODEC][2].toInt());

    /************************************* Level module **************************************/

    QString arr_level[NUMBER_PRESETS][21] = {
        {"Auto", "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "1b", "1.1", "1.2", "1.3", "2", "2.1", "2.2", "3",   "3.1", "3.2", "4",   "4.1", "4.2", "5", "5.1", "5.2", "6", "6.1", "6.2"},
        {"Auto", "1", "1b", "1.1", "1.2", "1.3", "2", "2.1", "2.2", "3",   "3.1", "3.2", "4",   "4.1", "4.2", "5", "5.1", "5.2", "6", "6.1", "6.2"},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "2",  "2.1", "3",   "3.1", "4", "4.1", "5",   "5.1", "5.2", "6",   "6.1", "6.2", "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "1", "1b", "1.1", "1.2", "1.3", "2", "2.1", "2.2", "3",   "3.1", "3.2", "4",   "4.1", "4.2", "5", "5.1", "5.2", "6", "6.1", "6.2"},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"2",    "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""},
        {"Auto", "",  "",   "",    "",    "",    "",  "",    "",    "",    "",    "",    "",    "",    "",    "",  "",    "",    "",  "",    ""}
    };

    QString level = "";
    QString selected_level = arr_level[_CODEC][_LEVEL];
    if (selected_level != "" && selected_level != "Auto") {
        level = QString("-level:v %1 ").arg(selected_level);
    }

    /************************************* Mode module ***************************************/

    QString arr_mode[NUMBER_PRESETS][5] = {
        {"CBR",    "ABR", "VBR", "CRF", "CQP"},
        {"CBR",    "ABR", "VBR", "CRF", "CQP"},
        {"CBR",    "ABR", "VBR", "CRF", "CQP"},
        {"VBR",    "",    "",    "",    ""},
        {"CBR",    "",    "",    "",    ""},
        {"VBR_NV", "",    "",    "",    ""},
        {"VBR_NV", "",    "",    "",    ""},
        {"VBR_NV", "",    "",    "",    ""},
        {"ABR",    "CRF", "",    "",    ""},
        {"ABR",    "CRF", "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"",       "",    "",    "",    ""},
        {"VBR",    "",    "",    "",    ""},
        {"",       "",    "",    "",    ""}
    };
    QString mode = "";
    QString bitrate = QString::number(1000000.0*_BQR.toDouble(), 'f', 0);
    QString minrate = QString::number(1000000.0*_MINRATE.toDouble(), 'f', 0);
    QString maxrate = QString::number(1000000.0*_MAXRATE.toDouble(), 'f', 0);
    QString bufsize = QString::number(1000000.0*_BUFSIZE.toDouble(), 'f', 0);
    QString selected_mode = arr_mode[_CODEC][_MODE];

    if (selected_mode == "CBR") {
        mode = QString("-b:v %1 -minrate %1 -maxrate %1 -bufsize %2 ").arg(bitrate, bufsize);
    }
    else if (selected_mode == "ABR") {
        mode = QString("-b:v %1 ").arg(bitrate);
    }
    else if (selected_mode == "VBR") {
        mode = QString("-b:v %1 -minrate %2 -maxrate %3 -bufsize %4 ").arg(bitrate, minrate, maxrate, bufsize);
    }
    else if (selected_mode == "VBR_NV") {
        mode = QString("-b:v %1 -minrate %2 -maxrate %3 -bufsize %4 -rc vbr_hq ").arg(bitrate, minrate, maxrate, bufsize);
    }
    else if (selected_mode == "CRF") {
        mode = QString("-crf %1 ").arg(_BQR);
    }
    else if (selected_mode == "CQP") {
        mode = QString("-b:v 0 -cq %1 -qmin %1 -qmax %1 ").arg(_BQR);
    }

    /************************************* Preset module ***************************************/

    QString arr_preset[NUMBER_PRESETS][10] = {
        {"None", "Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow",     "Slower", "Veryslow"},
        {"None", "Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow",     "Slower", "Veryslow"},
        {"None", "Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow",     "Slower", "Veryslow"},
        {"None", "Veryfast",  "Faster",    "Fast",     "Medium", "Slow", "Slower", "Veryslow", "",       ""},
        {"None", "Veryfast",  "Faster",    "Fast",     "Medium", "Slow", "Slower", "Veryslow", "",       ""},
        {"None", "Slow",      "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "Slow",      "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "Slow",      "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"",     "",          "",          "",         "",       "",     "",       "",         "",       ""}
    };
    QString preset = "";
    QString selected_preset = arr_preset[_CODEC][_PRESET];
    if (selected_preset != "" && selected_preset != "None") {
        preset = QString("-preset ") + selected_preset.toLower() + QString(" ");
    }

    /************************************* Pass module ***************************************/

    QString arr_pass[NUMBER_PRESETS][2] = {
        {"",                     "-x265-params pass=2 "},
        {"",                     "-x265-params pass=2 "},
        {"",                     "-pass 2 "},
        {"",                     ""},
        {"",                     ""},
        {"-2pass 1 ",            ""},
        {"-2pass 1 ",            ""},
        {"-2pass 1 ",            ""},
        {"",                     "-pass 2 "},
        {"",                     "-pass 2 "},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""},
        {"",                     ""}
    };
    QString pass1 = "";
    QString pass = arr_pass[_CODEC][_PASS];
    if (pass == "-x265-params pass=2 ") {
        pass1 = "-x265-params pass=1 ";
        _flag_two_pass = true;
    }
    if (pass == "-pass 2 ") {
        pass1 = "-pass 1 ";
        _flag_two_pass = true;
    }

    /************************************* Audio module ***************************************/

    QString arr_acodec[NUMBER_PRESETS][6] = {
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"AAC",   "AC3",    "DTS",    "Source", "",     ""},
        {"Opus",  "Vorbis", "Source", "",       "",     ""},
        {"Opus",  "Vorbis", "Source", "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "PCM24",  "PCM32",  "",       "",     ""},
        {"PCM16", "",       "",       "",       "",     ""},
        {"AAC",   "AC3",    "DTS",    "Vorbis", "Opus", "Source"}
    };

    QString arr_bitrate[5][17] = {
        {"384k",  "320k",  "256k",  "192k",  "128k",  "96k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AAC
        {"640k",  "448k",  "384k",  "256k",  "",      "",      "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AC3
        {"3840k", "3072k", "2048k", "1920k", "1536k", "1472k", "1344k", "1280k", "1152k", "1024k", "960k", "768k", "640k", "576k", "512k", "448k", "384k"}, // DTS
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // Vorbis
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""} // Opus
    };

    QString arr_sampling[12] = {
        "Source", "8000",  "11025", "16000", "22050",  "32000",
        "44100",  "48000", "88200", "96000", "176400", "192000"
    };

    QString arr_channels[3] = {
        "Source", "1", "2"
    };

    QString acodec = "";
    QString selected_acodec = arr_acodec[_CODEC][_AUDIO_CODEC];
    QString selected_bitrate = "";

    QString sampling = "";
    QString selected_sampling = arr_sampling[_AUDIO_SAMPLING];
    if (selected_sampling != "Source") {
        sampling = QString("-af aresample=%1:resampler=soxr ").arg(selected_sampling);
    }

    QString channels = "";
    QString selected_channels = arr_channels[_AUDIO_CHANNELS];
    if (selected_channels != "Source") {
        channels = QString(" -ac %1").arg(selected_channels);
    }


    if (selected_acodec == "AAC") {
        selected_bitrate = arr_bitrate[0][_AUDIO_BITRATE];
        acodec = QString("-c:a aac -b:a %1").arg(selected_bitrate);
    }
    else if (selected_acodec == "AC3") {
        selected_bitrate = arr_bitrate[1][_AUDIO_BITRATE];
        acodec = QString("-c:a ac3 -b:a %1").arg(selected_bitrate);
    }
    else if (selected_acodec == "DTS") {
        selected_bitrate = arr_bitrate[2][_AUDIO_BITRATE];
        acodec = QString("-strict -2 -c:a dca -b:a %1").arg(selected_bitrate);
    }
    else if (selected_acodec == "Vorbis") {
        selected_bitrate = arr_bitrate[3][_AUDIO_BITRATE];
        acodec = QString("-c:a libvorbis -b:a %1").arg(selected_bitrate);
    }
    else if (selected_acodec == "Opus") {
        selected_bitrate = arr_bitrate[4][_AUDIO_BITRATE];
        acodec = QString("-c:a libopus -b:a %1").arg(selected_bitrate);
    }
    else if (selected_acodec == "PCM16") {
        acodec = "-c:a pcm_s16le";
    }
    else if (selected_acodec == "PCM24") {
        acodec = "-c:a pcm_s24le";
    }
    else if (selected_acodec == "PCM32") {
        acodec = "-c:a pcm_s32le";
    }
    else if (selected_acodec == "Source") {
        acodec = "-c:a copy";
    }
    QString audio_param = sampling + acodec + channels;

    /************************************* Color module ***************************************/

    // color primaries

    QString arr_colorprim[11] = {
        "Source",    "bt470m",   "bt470bg",  "bt709",    "bt2020", "smpte170m",
        "smpte240m", "smpte428", "smpte431", "smpte432", "film"
    };

    QString colorprim = "";
    QString selected_colorprim = arr_colorprim[_PRIMARY];
    if (selected_colorprim == "Source") {
        if (_hdr[CUR_COLOR_PRIMARY] == "BT709") {
            colorprim = "-color_primaries bt709 ";
        }
        else if (_hdr[CUR_COLOR_PRIMARY] == "BT2020") {
            colorprim = "-color_primaries bt2020 ";
        }
        else if (_hdr[CUR_COLOR_PRIMARY] == "BT601 NTSC") {
            colorprim = "-color_primaries smpte170m ";
        }
        else if (_hdr[CUR_COLOR_PRIMARY] == "BT601 PAL") {
            colorprim = "-color_primaries bt470bg ";
        }
        else if (_hdr[CUR_COLOR_PRIMARY] == "BT470 System M") {
            colorprim = "-color_primaries bt470m ";
        }
        else if (_hdr[CUR_COLOR_PRIMARY] == "SMPTE 240M") {
            colorprim = "-color_primaries smpte240m ";
        }
        else if (_hdr[CUR_COLOR_PRIMARY] == "Generic film") {
            colorprim = "-color_primaries film ";
        }
        else if (_hdr[CUR_COLOR_PRIMARY] == "DCI P3") {
            colorprim = "-color_primaries smpte431 ";
        }
        else if (_hdr[CUR_COLOR_PRIMARY] == "XYZ") {
            colorprim = "-color_primaries smpte428 ";
        }
        else if (_hdr[CUR_COLOR_PRIMARY] == "Display P3") {
            colorprim = "-color_primaries smpte432 ";
        }
    }
    else {
        colorprim = QString("-color_primaries %1 ").arg(selected_colorprim);
    }

    // color matrix

    QString arr_colormatrix[14] = {
        "Source", "bt470bg", "bt709", "bt2020nc", "bt2020c", "smpte170m", "smpte240m",
        "smpte2085", "chroma-derived-nc", "chroma-derived-c", "fcc", "GBR", "ICtCp", "YCgCo"
    };

    QString colormatrix = "";
    QString selected_colormatrix = arr_colormatrix[_MATRIX];
    if (selected_colormatrix == "Source") {
        if (_hdr[CUR_COLOR_MATRIX] == "BT709") {
            colormatrix = "-colorspace bt709 ";
        }
        else if (_hdr[CUR_COLOR_MATRIX] == "BT2020nc") {
            colormatrix = "-colorspace bt2020nc ";
        }
        else if (_hdr[CUR_COLOR_MATRIX] == "BT2020c") {
            colormatrix = "-colorspace bt2020c ";
        }
        else if (_hdr[CUR_COLOR_MATRIX] == "FCC73682") {
            colormatrix = "-colorspace fcc ";
        }
        else if (_hdr[CUR_COLOR_MATRIX] == "BT470SystemB/G") {
            colormatrix = "-colorspace bt470bg ";
        }
        else if (_hdr[CUR_COLOR_MATRIX] == "SMPTE240M") {
            colormatrix = "-colorspace smpte240m ";
        }
        else if (_hdr[CUR_COLOR_MATRIX] == "YCgCo") {
            colormatrix = "-colorspace YCgCo ";
        }
        else if (_hdr[CUR_COLOR_MATRIX] == "Y'D'zD'x") {
            colormatrix = "-colorspace smpte2085 ";
        }
        else if (_hdr[CUR_COLOR_MATRIX] == "Chromaticity-derivednc") {
            colormatrix = "-colorspace chroma-derived-nc ";
        }
        else if (_hdr[CUR_COLOR_MATRIX] == "Chromaticity-derivedc") {
            colormatrix = "-colorspace chroma-derived-c ";
        }
        else if (_hdr[CUR_COLOR_MATRIX] == "ICtCp") {
            colormatrix = "-colorspace ICtCp ";
        }
        else if (_hdr[CUR_COLOR_MATRIX] == "BT601") {
            colormatrix = "-colorspace smpte170m ";
        }
        else if (_hdr[CUR_COLOR_MATRIX] == "Identity") {
            colormatrix = "-colorspace GBR ";
        }
    }
    else {
        colormatrix = QString("-colorspace %1 ").arg(selected_colormatrix);
    }

    // transfer characteristics

    QString arr_trc[17] = {
        "Source", "bt470m", "bt470bg", "bt709", "bt1361e", "bt2020-10", "bt2020-12", "smpte170m",
        "smpte240m", "smpte428", "smpte2084", "arib-std-b67", "linear", "log100", "log316",
        "iec61966-2-1", "iec61966-2-4"
    };

    QString transfer = "";
    QString selected_transfer = arr_trc[_TRC];
    if (selected_transfer == "Source") {
        if (_hdr[CUR_TRANSFER] == "BT709") {
            transfer = "-color_trc bt709 ";
        }
        else if (_hdr[CUR_TRANSFER] == "PQ") {
            transfer = "-color_trc smpte2084 ";
        }
        else if (_hdr[CUR_TRANSFER] == "HLG") {
            transfer = "-color_trc arib-std-b67 ";
        }
        else if (_hdr[CUR_TRANSFER] == "BT2020 (10-bit)") {
            transfer = "-color_trc bt2020-10 ";
        }
        else if (_hdr[CUR_TRANSFER] == "BT2020 (12-bit)") {
            transfer = "-color_trc bt2020-12 ";
        }
        else if (_hdr[CUR_TRANSFER] == "BT470 System M") {
            transfer = "-color_trc bt470m ";
        }
        else if (_hdr[CUR_TRANSFER] == "BT470 System B/G") {
            transfer = "-color_trc bt470bg ";
        }
        else if (_hdr[CUR_TRANSFER] == "SMPTE 240M") {
            transfer = "-color_trc smpte240m ";
        }
        else if (_hdr[CUR_TRANSFER] == "Linear") {
            transfer = "-color_trc linear ";
        }
        else if (_hdr[CUR_TRANSFER] == "Logarithmic (100:1)") {
            transfer = "-color_trc log100 ";
        }
        else if (_hdr[CUR_TRANSFER] == "Logarithmic (31622777:1)") {
            transfer = "-color_trc log316 ";
        }
        else if (_hdr[CUR_TRANSFER] == "xvYCC") {
            transfer = "-color_trc iec61966-2-4 ";
        }
        else if (_hdr[CUR_TRANSFER] == "BT1361") {
            transfer = "-color_trc bt1361e ";
        }
        else if (_hdr[CUR_TRANSFER] == "sRGB/sYCC") {
            transfer = "-color_trc iec61966-2-1 ";
        }
        else if (_hdr[CUR_TRANSFER] == "SMPTE 428M") {
            transfer = "-color_trc smpte428 ";
        }
        else if (_hdr[CUR_TRANSFER] == "BT601") {
            transfer = "-color_trc smpte170m ";
        }
    }
    else {
        transfer = QString("-color_trc %1 ").arg(selected_transfer);
    }

    /************************************* HDR module ***************************************/

    QString color_range = "";
    QString max_lum = "";
    QString min_lum = "";
    QString max_cll = "";
    QString max_fall = "";
    QString chroma_coord = "";
    QString white_coord = "";
    if (_flag_hdr == true) {

        /********************************* Color range module **********************************/

        if (_COLOR_RANGE == 0) {                             // color range
            if (_hdr[CUR_COLOR_RANGE] == "Limited") {
                color_range = "-color_range tv ";
            }
            else if (_hdr[CUR_COLOR_RANGE] == "Full") {
                color_range = "-color_range pc ";
            }
        }
        else if (_COLOR_RANGE == 1) {
            color_range = "-color_range pc ";
        }
        else if (_COLOR_RANGE == 2) {
            color_range = "-color_range tv ";
        }

        /************************************* Lum module ***************************************/

        if (_MAX_LUM != "") {                           // max lum
            max_lum = QString("-s max-luminance=%1 ").arg(_MAX_LUM);
        } else {
            if (_hdr[CUR_MAX_LUM] != "") {
                max_lum = QString("-s max-luminance=%1 ").arg(_hdr[CUR_MAX_LUM]);
            } else {
                max_lum = "-d max-luminance ";
            }
        }

        if (_MIN_LUM != "") {                           // min lum
            min_lum = QString("-s min-luminance=%1 ").arg(_MIN_LUM);
        } else {
            if (_hdr[CUR_MIN_LUM] != "") {
                min_lum = QString("-s min-luminance=%1 ").arg(_hdr[CUR_MIN_LUM]);
            } else {
                min_lum = "-d min-luminance ";
            }
        }

        if (_MAX_CLL != "") {                           // max cll
            max_cll = QString("-s max-content-light=%1 ").arg(_MAX_CLL);
        } else {
            if (_hdr[CUR_MAX_CLL] != "") {
                max_cll = QString("-s max-content-light=%1 ").arg(_hdr[CUR_MAX_CLL]);
            } else {
                max_cll = "-d max-content-light ";
            }
        }

        if (_MAX_FALL != "") {                           // max fall
            max_fall = QString("-s max-frame-light=%1 ").arg(_MAX_FALL);
        } else {
            if (_hdr[CUR_MAX_FALL] != "") {
                max_fall = QString("-s max-frame-light=%1 ").arg(_hdr[CUR_MAX_FALL]);
            } else {
                max_fall = "-d max-frame-light ";
            }
        }

        /************************************* Display module ***************************************/

        QString chroma_coord_curr_red_x = "";
        QString chroma_coord_curr_red_y = "";
        QString chroma_coord_curr_green_x = "";
        QString chroma_coord_curr_green_y = "";
        QString chroma_coord_curr_blue_x = "";
        QString chroma_coord_curr_blue_y = "";
        QString white_coord_curr_x = "";
        QString white_coord_curr_y = "";
        if (_MASTER_DISPLAY == 0) {     // From source
            if (_hdr[CUR_MASTER_DISPLAY] == "Display P3") {
                chroma_coord_curr_red_x = "0.680";
                chroma_coord_curr_red_y = "0.320";
                chroma_coord_curr_green_x = "0.265";
                chroma_coord_curr_green_y = "0.690";
                chroma_coord_curr_blue_x = "0.150";
                chroma_coord_curr_blue_y = "0.060";
                white_coord_curr_x = "0.3127";
                white_coord_curr_y = "0.3290";
            }
            if (_hdr[CUR_MASTER_DISPLAY] == "DCI P3") {
                chroma_coord_curr_red_x = "0.680";
                chroma_coord_curr_red_y = "0.320";
                chroma_coord_curr_green_x = "0.265";
                chroma_coord_curr_green_y = "0.690";
                chroma_coord_curr_blue_x = "0.150";
                chroma_coord_curr_blue_y = "0.060";
                white_coord_curr_x = "0.314";
                white_coord_curr_y = "0.3510";
            }
            if (_hdr[CUR_MASTER_DISPLAY] == "BT.2020") {
                chroma_coord_curr_red_x = "0.708";
                chroma_coord_curr_red_y = "0.292";
                chroma_coord_curr_green_x = "0.170";
                chroma_coord_curr_green_y = "0.797";
                chroma_coord_curr_blue_x = "0.131";
                chroma_coord_curr_blue_y = "0.046";
                white_coord_curr_x = "0.3127";
                white_coord_curr_y = "0.3290";
            }
            if (_hdr[CUR_MASTER_DISPLAY] == "BT.709") {
                chroma_coord_curr_red_x = "0.640";
                chroma_coord_curr_red_y = "0.330";
                chroma_coord_curr_green_x = "0.30";
                chroma_coord_curr_green_y = "0.60";
                chroma_coord_curr_blue_x = "0.150";
                chroma_coord_curr_blue_y = "0.060";
                white_coord_curr_x = "0.3127";
                white_coord_curr_y = "0.3290";
            }
            if (_hdr[CUR_MASTER_DISPLAY] == "Undefined") {
                QStringList chr = _hdr[CUR_CHROMA_COORD].split(",");
                if (chr.size() == 6) {
                    chroma_coord_curr_red_x = chr[0];
                    chroma_coord_curr_red_y = chr[1];
                    chroma_coord_curr_green_x = chr[2];
                    chroma_coord_curr_green_y = chr[3];
                    chroma_coord_curr_blue_x = chr[4];
                    chroma_coord_curr_blue_y = chr[5];
                } else {
                    _message = "Incorrect master display chroma coordinates source parameters!";
                    call_task_complete(_message, false);
                    return;
                }
                QStringList wht = _hdr[CUR_WHITE_COORD].split(",");
                if (wht.size() == 2) {
                    white_coord_curr_x = wht[0];
                    white_coord_curr_y = wht[1];
                } else {
                    _message = "Incorrect master display white point coordinates source parameters!";
                    call_task_complete(_message, false);
                    return;
                }
            }
        }
        if (_MASTER_DISPLAY == 1) {     // Display P3
            chroma_coord_curr_red_x = "0.680";
            chroma_coord_curr_red_y = "0.320";
            chroma_coord_curr_green_x = "0.265";
            chroma_coord_curr_green_y = "0.690";
            chroma_coord_curr_blue_x = "0.150";
            chroma_coord_curr_blue_y = "0.060";
            white_coord_curr_x = "0.3127";
            white_coord_curr_y = "0.3290";
        }
        if (_MASTER_DISPLAY == 2) {     // DCI P3
            chroma_coord_curr_red_x = "0.680";
            chroma_coord_curr_red_y = "0.320";
            chroma_coord_curr_green_x = "0.265";
            chroma_coord_curr_green_y = "0.690";
            chroma_coord_curr_blue_x = "0.150";
            chroma_coord_curr_blue_y = "0.060";
            white_coord_curr_x = "0.314";
            white_coord_curr_y = "0.3510";
        }
        if (_MASTER_DISPLAY == 3) {     // BT.2020
            chroma_coord_curr_red_x = "0.708";
            chroma_coord_curr_red_y = "0.292";
            chroma_coord_curr_green_x = "0.170";
            chroma_coord_curr_green_y = "0.797";
            chroma_coord_curr_blue_x = "0.131";
            chroma_coord_curr_blue_y = "0.046";
            white_coord_curr_x = "0.3127";
            white_coord_curr_y = "0.3290";
        }
        if (_MASTER_DISPLAY == 4) {     // BT.709
            chroma_coord_curr_red_x = "0.640";
            chroma_coord_curr_red_y = "0.330";
            chroma_coord_curr_green_x = "0.30";
            chroma_coord_curr_green_y = "0.60";
            chroma_coord_curr_blue_x = "0.150";
            chroma_coord_curr_blue_y = "0.060";
            white_coord_curr_x = "0.3127";
            white_coord_curr_y = "0.3290";
        }
        if (_MASTER_DISPLAY == 5) {     // Custom
            QStringList chr = _CHROMA_COORD.split(",");
            if (chr.size() == 6) {
                chroma_coord_curr_red_x = chr[0];
                chroma_coord_curr_red_y = chr[1];
                chroma_coord_curr_green_x = chr[2];
                chroma_coord_curr_green_y = chr[3];
                chroma_coord_curr_blue_x = chr[4];
                chroma_coord_curr_blue_y = chr[5];
            }
            QStringList wht = _WHITE_COORD.split(",");
            if (wht.size() == 2) {
                white_coord_curr_x = wht[0];
                white_coord_curr_y = wht[1];
            }
        }

        if (chroma_coord_curr_red_x == "") {
            chroma_coord = "-d chromaticity-coordinates-red-x -d chromaticity-coordinates-red-y "
                           "-d chromaticity-coordinates-green-x -d chromaticity-coordinates-green-y "
                           "-d chromaticity-coordinates-blue-x -d chromaticity-coordinates-blue-y ";
        } else {
            chroma_coord = QString("-s chromaticity-coordinates-red-x=%1 -s chromaticity-coordinates-red-y=%2 "
                                   "-s chromaticity-coordinates-green-x=%3 -s chromaticity-coordinates-green-y=%4 "
                                   "-s chromaticity-coordinates-blue-x=%5 -s chromaticity-coordinates-blue-y=%6 ")
                                   .arg(chroma_coord_curr_red_x, chroma_coord_curr_red_y, chroma_coord_curr_green_x,
                                        chroma_coord_curr_green_y, chroma_coord_curr_blue_x, chroma_coord_curr_blue_y);
        }
        if (white_coord_curr_x == "") {
            white_coord = "-d white-coordinates-x -d white-coordinates-y ";
        } else {
            white_coord = QString("-s white-coordinates-x=%1 -s white-coordinates-y=%2 ").arg(white_coord_curr_x, white_coord_curr_y);
        }
    }

    /************************************* Result module ***************************************/

    _preset_0 = "-hide_banner" + hwaccel + _splitStartParam;
    _preset_pass1 = _splitParam + transform + codec + level + preset + mode + pass1 + color_range
            + colorprim + colormatrix + transfer + "-an -f null /dev/null";
    _preset = _splitParam + transform + codec + level + preset + mode + pass + color_range
            + colorprim + colormatrix + transfer + audio_param;
    _preset_mkvmerge = QString("%1%2%3%4%5%6 ").arg(max_cll, max_fall, max_lum, min_lum, chroma_coord, white_coord);
    std::cout << "Flag two-pass: " << _flag_two_pass << std::endl;
    std::cout << "Flag HDR: " << _flag_hdr << std::endl;
    std::cout << "preset_0: " << _preset_0.toStdString() << std::endl;
    if ((_flag_two_pass == true) && (_flag_hdr == true)) {
        std::cout << "preset_pass1: " << _preset_pass1.toStdString() << std::endl;
        std::cout << "preset: " << _preset.toStdString() << std::endl;
        std::cout << "preset_mkvpropedit: " << _preset_mkvmerge.toStdString() << std::endl;
        ui->textBrowser_log->append(QString("Preset pass 1: ") + _preset_0 + QString(" -i <input file> ") +
                                    _preset_pass1 + QString("\n"));
        ui->textBrowser_log->append(QString("Preset pass 2: ") + _preset_0 + QString(" -i <input file> ") +
                                    _preset  + QString(" -y <output file>\n"));
        ui->textBrowser_log->append(QString("Preset mkvpropedit: ") + _preset_mkvmerge  + QString("\n"));
    }
    else if ((_flag_two_pass == true) && (_flag_hdr == false)) {
        std::cout << "preset_pass1: " << _preset_pass1.toStdString() << std::endl;
        std::cout << "preset: " << _preset.toStdString() << std::endl;
        ui->textBrowser_log->append(QString("Preset pass 1: ") + _preset_0 + QString(" -i <input file> ") +
                                    _preset_pass1 + QString("\n"));
        ui->textBrowser_log->append(QString("Preset pass 2: ") + _preset_0 + QString(" -i <input file> ") +
                                    _preset  + QString(" -y <output file>\n"));
    }
    else if ((_flag_two_pass == false) && (_flag_hdr == true)) {
        std::cout << "preset: " << _preset.toStdString() << std::endl;
        std::cout << "preset_mkvpropedit: " << _preset_mkvmerge.toStdString() << std::endl;
        ui->textBrowser_log->append(QString("Preset: ") + _preset_0 + QString(" -i <input file> ") +
                                    _preset  + QString(" -y <output file>\n"));
        ui->textBrowser_log->append(QString("Preset mkvpropedit: ") + _preset_mkvmerge  + QString("\n"));
    }
    else if ((_flag_two_pass == false) && (_flag_hdr == false)) {
        std::cout << "preset: " << _preset.toStdString() << std::endl;
        ui->textBrowser_log->append(QString("Preset: ") + _preset_0 + QString(" -i <input file> ") +
                                    _preset  + QString(" -y <output file>\n"));
    }
    encode();
}

void Widget::encode()   /*** Encode ***/
{
    std::cout << "Encode ..." << std::endl;  //  Debug info //
    QStringList arguments;
    _calling_pr_1 = true;
    process_1->disconnect();
    connect(process_1, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_1()));
    connect(process_1, SIGNAL(finished(int)), this, SLOT(error()));
    ui->progressBar->setValue(0);
    if (_mux_mode == true) {
        std::cout << "Muxing mode ..." << std::endl;  //  Debug info //
        ui->label_53->setText("Muxing:");
        arguments << "-hide_banner" << "-i" << _temp_file << "-map" << "0:v:0?" << "-map" << "0:a?"
                  << "-map" << "0:s?" << "-movflags" << "+write_colr"
                  << "-c:v" << "copy" << "-c:a" << "copy" << "-y" << _output_file;
    } else {
        if (_fr_count == 0) {
            _status_encode_btn = "start";
            ui->actionEncode->setIcon(QIcon(":/resources/icons/16x16/cil-play.png"));
            ui->actionEncode->setToolTip("Encode");
            _message = "The file does not contain FPS information!\nSelect the correct input file!";
            call_task_complete(_message, false);
            return;
        }

        QTableWidgetItem *newItem_status = new QTableWidgetItem("Encoding");
        newItem_status->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(_row, columnIndex::STATUS, newItem_status);

        add_files->setEnabled(false);
        remove_files->setEnabled(false);
        //select_preset->setEnabled(false);
        /*edit_metadata->setEnabled(false);
        select_audio->setEnabled(false);
        select_subtitles->setEnabled(false);
        split_video->setEnabled(false);*/
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

        ui->actionSettings->setEnabled(false);
        ui->labelAnimation->show();
        animation->start();
        ui->label_53->show();
        ui->label_54->show();
        ui->label_55->show();
        ui->progressBar->show();
        _loop_start = time(nullptr);
        if (_flag_two_pass == false && _flag_hdr == false) {
            std::cout << "Encode non HDR..." << std::endl;  //  Debug info //
            ui->label_53->setText("Encoding:");
            arguments << _preset_0.split(" ") << "-i" << _input_file << _preset.split(" ") << "-y" << _output_file;
        }
        if (_flag_two_pass == false && _flag_hdr == true) {
            std::cout << "Encode HDR..." << std::endl;  //  Debug info //
            ui->label_53->setText("Encoding:");
            arguments << _preset_0.split(" ") << "-i" << _input_file << _preset.split(" ") << "-y" << _temp_file;
        }
        if (_flag_two_pass == true) {
            std::cout << "Encode 1-st pass..." << std::endl;  //  Debug info //
            ui->label_53->setText("1-st pass:");
            arguments << _preset_0.split(" ") << "-y" << "-i" << _input_file << _preset_pass1.split(" ");
        }
    }
    //qDebug() << arguments;
    process_1->start("ffmpeg", arguments);
    if (!process_1->waitForStarted()) {
        std::cout << "cmd command not found!!!" << std::endl;
        process_1->disconnect();
        restore_initial_state();
        _message = "An unknown error occurred!\n Possible FFMPEG not installed.\n";
        call_task_complete(_message, false);
    }
}

void Widget::add_metadata() /*** Add metedata ***/
{
    std::cout << "Add metadata ..." << std::endl;  //  Debug info //
    _calling_pr_1 = true;
    process_1->disconnect();
    connect(process_1, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_2()));
    connect(process_1, SIGNAL(finished(int)), this, SLOT(error()));
    ui->label_53->setText("Add data:");
    ui->progressBar->setValue(0);
    QStringList arguments;
    arguments << "--edit" << "track:1" << _preset_mkvmerge.split(" ") << _temp_file;
    process_1->start("mkvpropedit", arguments);
    if (!process_1->waitForStarted()) {
        std::cout << "cmd command not found!!!" << std::endl;
        process_1->disconnect();
        restore_initial_state();
        _message = "An unknown error occured!\n Possible mkvtoolnix not installed.\n";
        call_task_complete(_message, false);
    };
}

void Widget::complete() /*** Complete ***/
{
    std::cout << "Complete ..." << std::endl;  //  Debug info //
    process_1->disconnect();
    QTableWidgetItem *newItem_status = new QTableWidgetItem("Done!");
    newItem_status->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(_row, columnIndex::STATUS, newItem_status);
    animation->stop();
    ui->label_53->hide();
    ui->label_54->hide();
    ui->label_55->hide();
    ui->labelAnimation->hide();
    ui->progressBar->hide();
    if (_flag_hdr == true) {
        QDir().remove(_temp_file);
    }
    if (_batch_mode == true) {
        int row = ui->tableWidget->currentRow();
        int numRows = ui->tableWidget->rowCount();
        if (numRows > (row + 1)) {
            ui->tableWidget->selectRow(row + 1);
            make_preset();
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
            _message = QString("Task completed!\n\n Elapsed time: ") + timeConverter(elps_t);
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
        _message = QString("Task completed!\n\n Elapsed time: ") + timeConverter(elps_t);
        call_task_complete(_message, false);
    }
    QDir().remove(QDir::homePath() + QString("/ffmpeg2pass-0.log"));
    QDir().remove(QDir::homePath() + QString("/ffmpeg2pass-0.log.mbtree"));
    QDir().remove(QDir::homePath() + QString("/x265_2pass.log"));
    QDir().remove(QDir::homePath() + QString("/x265_2pass.log.cutree"));
}

void Widget::progress_1()   /*** Progress 1 ***/
{
    QString line = process_1->readAllStandardOutput();
    QString line_mod6 = line.replace("   ", " ").replace("  ", " ").replace("  ", " ").replace("= ", "=");
    //std::cout << line_mod6.toStdString() << std::endl;
    int pos_err_1 = line_mod6.indexOf("[error]:");
    int pos_err_2 = line_mod6.indexOf("Error");
    int pos_err_3 = line_mod6.indexOf(" @ ");
    if (pos_err_1 != -1) {
        QStringList error = line_mod6.split(":");
        if (error.size() >= 2) {
            _error_message = error[1];
            //std::cout << _error_message.toStdString() << std::endl;
        }
    }
    if (pos_err_2 != -1) {
        _error_message = line_mod6;
        //std::cout << _error_message.toStdString() << std::endl;
    }
    if (pos_err_3 != -1) {
        QStringList error = line_mod6.split("]");
        if (error.size() >= 2) {
            _error_message = error[1];
            //std::cout << _error_message.toStdString() << std::endl;
        }
    }
    int pos_st = line_mod6.indexOf("frame=");
    if (pos_st == 0) {
        QStringList data = line_mod6.split(" ");
        QString frame_qstr = data[0].replace("frame=", "");
        int frame = frame_qstr.toInt();
        if (frame == 0) {
            frame = 1;
        }
        time_t iter_start = time(nullptr);
        int timer = static_cast<int>(iter_start - _loop_start);
        float full_time = static_cast<float>(timer * _fr_count) / (frame);
        float rem_time = full_time - static_cast<float>(timer);
        if (rem_time < 0.0f) {
            rem_time = 0.0f;
        }
        if (rem_time > MAXIMUM_ALLOWED_TIME) {
            rem_time = MAXIMUM_ALLOWED_TIME;
        }
        ui->label_55->setText(timeConverter(rem_time));

        float percent = static_cast<float>(frame * 100) / _fr_count;
        int percent_int = static_cast<int>(round(percent));
        if (percent_int > 100) {
            percent_int = 100;
        }
        ui->progressBar->setValue(percent_int);

        if ((percent_int >= 95) && (_calling_pr_1 == true)) {
             disconnect(process_1, SIGNAL(finished(int)), this, SLOT(error()));
             if (_mux_mode == true) {
                 connect(process_1, SIGNAL(finished(int)), this, SLOT(complete()));
             } else {
                 if (_flag_two_pass == false && _flag_hdr == true) {
                     disconnect(process_1, SIGNAL(finished(int)), this, SLOT(encode()));
                     connect(process_1, SIGNAL(finished(int)), this, SLOT(add_metadata()));
                 }
                 if (_flag_two_pass == false && _flag_hdr == false) {
                     disconnect(process_1, SIGNAL(finished(int)), this, SLOT(encode()));
                     connect(process_1, SIGNAL(finished(int)), this, SLOT(complete()));
                 }
                 if (_flag_two_pass == true) {
                     connect(process_1, SIGNAL(finished(int)), this, SLOT(encode()));
                     _flag_two_pass = false;
                 }
             }
             _calling_pr_1 = false;
        }
    }
}

void Widget::progress_2()   /*** Progress 2 ***/
{
    QString line = process_1->readAllStandardOutput();
    int pos_st = line.indexOf("Done.");
    int pos_nf = line.indexOf("Nothing to do.");
    if ((pos_st != -1) or (pos_nf != -1)) {
        int percent = 100;
        ui->progressBar->setValue(percent);
        if ((percent == 100) && (_calling_pr_1 == true)) {
            disconnect(process_1, SIGNAL(finished(int)), this, SLOT(error()));
            _mux_mode = true;
            _loop_start = time(nullptr);
            _calling_pr_1 = false;
            connect(process_1, SIGNAL(finished(int)), this, SLOT(encode()));
        };
    };
}

void Widget::pause()    /*** Pause ***/
{
    if (_protection == true) {
        timer->stop();
    }
    short s1 = process_1->state();
    qint64 pr1 = process_1->processId();
    std::cout << "State procedure_1: " << s1 << " PID: " << pr1 << std::endl;
    if (s1 != 0) {
        QTableWidgetItem *newItem_status = new QTableWidgetItem("Pause");
        newItem_status->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(_row, columnIndex::STATUS, newItem_status);
        animation->stop();
#ifdef Q_OS_WIN
        _PROCESS_INFORMATION *pi = process_1->pid();  // pause for Windows
        SuspendThread(pi->hThread);  // pause for Windows
#else
        kill(pid_t(process_1->processId()), SIGSTOP);  // pause for Unix
#endif
    }
}

void Widget::resume()   /*** Resume ***/
{
    if (_protection == true) {
        timer->start();
    }
    short s1 = process_1->state();
    qint64 pr1 = process_1->processId();
    std::cout << "State procedure_1: " << s1 << " PID: " << pr1 << std::endl;
    if (s1 != 0) {
        QTableWidgetItem *newItem_status = new QTableWidgetItem("Encoding");
        newItem_status->setTextAlignment(Qt::AlignCenter);
        ui->tableWidget->setItem(_row, columnIndex::STATUS, newItem_status);
        animation->start();
#ifdef Q_OS_WIN
        _PROCESS_INFORMATION *pi = process_1->pid();  // pause for Windows
        ResumeThread(pi->hThread);  // pause for Windows
#else
        kill(pid_t(process_1->processId()), SIGCONT); // resume for Unix
#endif
    }
}

void Widget::cancel()   /*** Stop execute ***/
{
    std::cout << "Stop execute ..." << std::endl;  //  Debug info //
    if (_protection == true) {
        timer->stop();
    }
    process_1->disconnect();
    QTableWidgetItem *newItem_status = new QTableWidgetItem("Stop");
    newItem_status->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(_row, columnIndex::STATUS, newItem_status);
    ui->label_53->hide();
    ui->label_54->hide();
    ui->label_55->hide();
    ui->labelAnimation->hide();
    ui->progressBar->hide();
    restore_initial_state();
    _message = "The current encoding process has been canceled!\n";
    call_task_complete(_message, false);
}

void Widget::error()  /*** Error ***/
{
    std::cout << "Error_1 ..." << std::endl;  //  Debug info //
    if (_protection == true) {
        timer->stop();
    }
    process_1->disconnect();
    QTableWidgetItem *newItem_status = new QTableWidgetItem("Error!");
    newItem_status->setTextAlignment(Qt::AlignCenter);
    ui->tableWidget->setItem(_row, columnIndex::STATUS, newItem_status);
    restore_initial_state();
    if (_error_message != "") {
        _message = "An error occurred: " + _error_message;
    } else {
        _message = "Unexpected error occurred!";
    }
    call_task_complete(_message, false);
}

void Widget::repeatHandler_Type_1()  /*** Repeat handler ***/
{
    std::cout<< "Call by timer..." << std::endl;
    on_actionEncode_clicked();
    call_task_complete("Pause", true);
    on_actionEncode_clicked();
}

/************************************************
** Task Window
************************************************/

void Widget::on_actionAdd_clicked() /*** Add files ***/
{
    QFileDialog *openFilesWindow = new QFileDialog(nullptr);
    openFilesWindow->setFileMode(QFileDialog::ExistingFiles);
#ifdef Q_OS_WIN
    openFilesWindow->setOptions(QFileDialog::DontResolveSymlinks);
#endif
#ifdef GNOME_DESKTOP
    openFilesWindow->setOptions(QFileDialog::DontUseNativeDialog | QFileDialog::DontResolveSymlinks);
#endif
#ifdef KDE_DESKTOP
    openFilesWindow->setOptions(QFileDialog::DontResolveSymlinks);
#endif
    openFilesWindow->setDirectory(_openDir);
    openFilesWindow->setMinimumWidth(600);
    openFilesWindow->setWindowTitle("Open Files");
    openFilesWindow->setNameFilter(tr("Video Files: *.avi, *.m2ts, *.m4v, *.mkv, *.mov, *.mp4, "
                                      "*.mpeg, *.mpg, *.mxf, *.ts, *.webm (*.avi *.m2ts *.m4v "
                                      "*.mkv *.mov *.mp4 *.mpeg *.mpg *.mxf *.ts *.webm);;All files (*.*)"));
    openFilesWindow->setWindowFlags(Qt::Dialog | Qt::SubWindow);
    openFilesWindow->exec();
    int res = openFilesWindow->result();
    const QStringList openFileNames = openFilesWindow->selectedFiles();
    delete openFilesWindow;
    if (res == 0) {
        return;
    } else {
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
    ui->tableWidget->sortByColumn(columnIndex::FILENAME, Qt::DescendingOrder);
}

void Widget::on_buttonSortUp_clicked()    /*** Sort table ***/
{
    ui->tableWidget->sortByColumn(columnIndex::FILENAME, Qt::AscendingOrder);
}

void Widget::on_actionEncode_clicked()  /*** Encode button ***/
{
    if (_status_encode_btn == "start") {
        std::cout << "Status encode btn: start" << std::endl;  // Debug info //
        int cnt = ui->tableWidget->rowCount();
        if (cnt == 0) {
            _message = "Select input file first!";
            call_task_complete(_message, false);
            return;
        }
        if (_pos_cld == -1) {
            _message = "Select preset first!";
            call_task_complete(_message, false);
            return;
        }
        _status_encode_btn = "pause";
        ui->actionEncode->setIcon(QIcon(":/resources/icons/16x16/cil-pause.png"));
        ui->actionEncode->setToolTip("Pause");
        _strt_t = time(nullptr);
        if (_protection == true) {
            timer->start();
        }
        make_preset();
        return;
    }
    if (_status_encode_btn == "pause") {
        std::cout << "Status encode btn: pause" << std::endl;  // Debug info //
        pause();
        _status_encode_btn = "resume";
        ui->actionEncode->setIcon(QIcon(":/resources/icons/16x16/cil-forward.png"));
        ui->actionEncode->setToolTip("Resume");
        return;
    }
    if (_status_encode_btn == "resume") {
        std::cout << "Status encode btn: resume" << std::endl;  // Debug info //
        resume();
        _status_encode_btn = "pause";
        ui->actionEncode->setIcon(QIcon(":/resources/icons/16x16/cil-pause.png"));
        ui->actionEncode->setToolTip("Pause");
        return;
    }
}

void Widget::on_actionStop_clicked()    /*** Stop ***/
{
    std::cout << "Call Stop ..." << std::endl;  //  Debug info //
    short s1 = process_1->state();
    if (s1 != 0) {
        _message = "Stop encoding?";
        bool confirm = call_dialog(_message);
        if (confirm == true) {
            process_1->disconnect();
            connect(process_1, SIGNAL(finished(int)), this, SLOT(cancel()));
            process_1->kill();
        }
    }
}

void Widget::openFiles(const QStringList &openFileNames)    /*** Open files ***/
{
    ui->labelAnimation->hide();
    ui->label_53->hide();
    ui->label_54->hide();
    ui->label_55->hide();
    ui->progressBar->hide();
    MediaInfo MI;
    int i = 1;
    int countFileNames = openFileNames.size();
    while (i <= countFileNames)
    {
        int numRows = ui->tableWidget->rowCount();
        std::wstring filePath_wstr = openFileNames.at(i-1).toStdWString();
        std::wstring::size_type separator = filePath_wstr.rfind('/');
        if (separator == std::wstring::npos) {
            _message = "Unexpected error while trying to perform file name detection.";
            call_task_complete(_message, false);
            return;
        }
        ui->tableWidget->setRowCount(numRows + 1);
        QString inputFile = QString::fromStdWString(filePath_wstr.substr(separator + 1));
        QString inputFolder = QString::fromStdWString(filePath_wstr.substr(0, separator));
        if (i == 1) {
            _openDir = inputFolder;
        }
        MI.Open(filePath_wstr);
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

        ui->tableWidget->setItem(numRows, columnIndex::FILENAME, newItem_file);
        ui->tableWidget->setItem(numRows, columnIndex::PATH, newItem_folder);
        ui->tableWidget->setItem(numRows, columnIndex::FORMAT, newItem_fmt);
        ui->tableWidget->setItem(numRows, columnIndex::RESOLUTION, newItem_resolution);
        ui->tableWidget->setItem(numRows, columnIndex::DURATION, newItem_duration_time);
        ui->tableWidget->setItem(numRows, columnIndex::FPS, newItem_fps);
        ui->tableWidget->setItem(numRows, columnIndex::AR, newItem_aspect_ratio);
        ui->tableWidget->setItem(numRows, columnIndex::BITRATE, newItem_bitrate);
        ui->tableWidget->setItem(numRows, columnIndex::SUBSAMPLING, newItem_subsampling);
        ui->tableWidget->setItem(numRows, columnIndex::BITDEPTH, newItem_bit_depth);
        ui->tableWidget->setItem(numRows, columnIndex::COLORSPACE, newItem_color_space);
        ui->tableWidget->setItem(numRows, columnIndex::COLORRANGE, newItem_color_range);
        ui->tableWidget->setItem(numRows, columnIndex::COLORPRIM, newItem_color_primaries);
        ui->tableWidget->setItem(numRows, columnIndex::COLORMATRIX, newItem_color_matrix);
        ui->tableWidget->setItem(numRows, columnIndex::TRANSFER, newItem_transfer_characteristics);
        ui->tableWidget->setItem(numRows, columnIndex::MAXLUM, newItem_max_lum);
        ui->tableWidget->setItem(numRows, columnIndex::MINLUM, newItem_min_lum);
        ui->tableWidget->setItem(numRows, columnIndex::MAXCLL, newItem_max_cll);
        ui->tableWidget->setItem(numRows, columnIndex::MAXFALL, newItem_max_fall);
        ui->tableWidget->setItem(numRows, columnIndex::MASTERDISPLAY, newItem_mastering_display);
        ui->tableWidget->setItem(numRows, columnIndex::T_CHROMACOORD, newItem_chroma_coord);
        ui->tableWidget->setItem(numRows, columnIndex::T_WHITECOORD, newItem_white_coord);
        ui->tableWidget->setItem(numRows, columnIndex::T_DUR, newItem_duration);
        ui->tableWidget->setItem(numRows, columnIndex::T_STREAMSIZE, newItem_stream_size);
        ui->tableWidget->setItem(numRows, columnIndex::T_WIDTH, newItem_width);
        ui->tableWidget->setItem(numRows, columnIndex::T_HEIGHT, newItem_height);
        ui->tableWidget->setItem(numRows, columnIndex::T_VIDEOTITLE, newItem_videoTitle);
        ui->tableWidget->setItem(numRows, columnIndex::T_VIDEOAUTHOR, newItem_videoAuthor);
        ui->tableWidget->setItem(numRows, columnIndex::T_VIDEOYEAR, newItem_videoYear);
        ui->tableWidget->setItem(numRows, columnIndex::T_VIDEOPERF, newItem_videoPerformer);
        ui->tableWidget->setItem(numRows, columnIndex::T_VIDEODESCR, newItem_videoDescription);
        ui->tableWidget->setItem(numRows, columnIndex::T_VIDEOMOVIENAME, newItem_videoMovieName);
        ui->tableWidget->setItem(numRows, columnIndex::T_STARTTIME, newItem_startTime);
        ui->tableWidget->setItem(numRows, columnIndex::T_ENDTIME, newItem_endTime);

        if (mastering_display_color_primaries != "") {
            newItem_file->setIcon(QIcon(":/resources/icons/16x16/cil-hdr.png"));
        }
        else if (height_qstr.toInt() >=1 && height_qstr.toInt() < 720) {
            newItem_file->setIcon(QIcon(":/resources/icons/16x16/cil-camera-roll.png"));
        }
        else if (height_qstr.toInt() >= 720 && height_qstr.toInt() < 2160) {
            newItem_file->setIcon(QIcon(":/resources/icons/16x16/cil-hd.png"));
        }
        else if (height_qstr.toInt() >= 2160) {
            newItem_file->setIcon(QIcon(":/resources/icons/16x16/cil-4k.png"));
        }
        else {
            newItem_file->setIcon(QIcon(":/resources/icons/16x16/cil-file.png"));
        }

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
            ui->tableWidget->setItem(numRows, j + columnIndex::T_AUDIO_1, newItem_audio);
            ui->tableWidget->setItem(numRows, j + columnIndex::T_AUDIOLANG_1, newItem_lang);
            ui->tableWidget->setItem(numRows, j + columnIndex::T_AUDIOTITLE_1, newItem_title);
            ui->tableWidget->setItem(numRows, j + columnIndex::T_AUDIOCHECK_1, newItem_checkstate);
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
            ui->tableWidget->setItem(numRows, j + columnIndex::T_SUBTITLE_1, newItem_subtitle);
            ui->tableWidget->setItem(numRows, j + columnIndex::T_SUBLANG_1, newItem_lang);
            ui->tableWidget->setItem(numRows, j + columnIndex::T_TITLESUB_1, newItem_title);
            ui->tableWidget->setItem(numRows, j + columnIndex::T_SUBCHECK_1, newItem_checkstate);
        }
        MI.Close();
        i++;
    }
    ui->tableWidget->selectRow(ui->tableWidget->rowCount() - 1);
}

void Widget::on_tableWidget_itemSelectionChanged()  /*** Item selection changed ***/
{
    ui->labelAnimation->hide();
    ui->label_53->hide();
    ui->label_54->hide();
    ui->label_55->hide();
    ui->progressBar->hide();

    // **************************** Disable audio widgets ***********************************//
    QList <QCheckBox *> checkBoxAudio;
    checkBoxAudio.append(ui->checkBoxAudio_1);
    checkBoxAudio.append(ui->checkBoxAudio_2);
    checkBoxAudio.append(ui->checkBoxAudio_3);
    checkBoxAudio.append(ui->checkBoxAudio_4);
    checkBoxAudio.append(ui->checkBoxAudio_5);
    checkBoxAudio.append(ui->checkBoxAudio_6);
    checkBoxAudio.append(ui->checkBoxAudio_7);
    checkBoxAudio.append(ui->checkBoxAudio_8);
    checkBoxAudio.append(ui->checkBoxAudio_9);

    QList <QLineEdit *> lineEditLangAudio;
    lineEditLangAudio.append(ui->lineEditLangAudio_1);
    lineEditLangAudio.append(ui->lineEditLangAudio_2);
    lineEditLangAudio.append(ui->lineEditLangAudio_3);
    lineEditLangAudio.append(ui->lineEditLangAudio_4);
    lineEditLangAudio.append(ui->lineEditLangAudio_5);
    lineEditLangAudio.append(ui->lineEditLangAudio_6);
    lineEditLangAudio.append(ui->lineEditLangAudio_7);
    lineEditLangAudio.append(ui->lineEditLangAudio_8);
    lineEditLangAudio.append(ui->lineEditLangAudio_9);

    QList <QLineEdit *> lineEditTitleAudio;
    lineEditTitleAudio.append(ui->lineEditTitleAudio_1);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_2);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_3);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_4);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_5);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_6);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_7);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_8);
    lineEditTitleAudio.append(ui->lineEditTitleAudio_9);

    for (int q = 0; q < AMOUNT_AUDIO_STREAMS; q++) {
        checkBoxAudio.at(q)->setEnabled(false);
        lineEditLangAudio.at(q)->setEnabled(false);
        lineEditTitleAudio.at(q)->setEnabled(false);
    }

    // **************************** Disable subtitle widgets ***********************************//
    QList <QCheckBox *> checkBoxSubtitle;
    checkBoxSubtitle.append(ui->checkBoxSubtitle_1);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_2);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_3);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_4);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_5);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_6);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_7);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_8);
    checkBoxSubtitle.append(ui->checkBoxSubtitle_9);

    QList <QLineEdit *> lineEditLangSubtitle;
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_1);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_2);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_3);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_4);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_5);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_6);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_7);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_8);
    lineEditLangSubtitle.append(ui->lineEditLangSubtitle_9);

    QList <QLineEdit *> lineEditTitleSubtitle;
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_1);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_2);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_3);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_4);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_5);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_6);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_7);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_8);
    lineEditTitleSubtitle.append(ui->lineEditTitleSubtitle_9);

    for (int q = 0; q < AMOUNT_SUBTITLES; q++) {
        checkBoxSubtitle.at(q)->setEnabled(false);
        lineEditLangSubtitle.at(q)->setEnabled(false);
        lineEditTitleSubtitle.at(q)->setEnabled(false);
    }

    _row = ui->tableWidget->currentRow();
    if (_row != -1) {
        raiseThumb->hide();
        get_current_data();

    } else {
        // ********************************* Reset widgets ***************************************//
        raiseThumb->show();
        ui->labelThumb->clear();
        ui->textBrowser_1->clear();
        ui->textBrowser_2->clear();
        ui->labelThumb->setText("Preview");
        ui->label_source->setText("");
        ui->label_output->setText("");

        ui->lineEditStartTime->clear();
        ui->lineEditEndTime->clear();

        // **************************** Reset metadata widgets ***********************************//
        ui->lineEditTitleVideo->clear();
        ui->lineEditAuthorVideo->clear();
        ui->lineEditYearVideo->clear();
        ui->lineEditPerfVideo->clear();
        ui->lineEditDescriptionVideo->clear();
        ui->lineEditMovieNameVideo->clear();

        ui->lineEditTitleVideo->setEnabled(false);
        ui->lineEditAuthorVideo->setEnabled(false);
        ui->lineEditYearVideo->setEnabled(false);
        ui->lineEditPerfVideo->setEnabled(false);
        ui->lineEditDescriptionVideo->setEnabled(false);
        ui->lineEditMovieNameVideo->setEnabled(false);

        // **************************** Reset audio widgets ***********************************//
        for (int q = 0; q < AMOUNT_AUDIO_STREAMS; q++) {
            lineEditLangAudio.at(q)->clear();
            lineEditTitleAudio.at(q)->clear();
            checkBoxAudio.at(q)->setChecked(false);
        }

        // **************************** Reset subtitle widgets ***********************************//
        for (int q = 0; q < AMOUNT_SUBTITLES; q++) {
            lineEditLangSubtitle.at(q)->clear();
            lineEditTitleSubtitle.at(q)->clear();
            checkBoxSubtitle.at(q)->setChecked(false);
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
        _startTime = 0;
        _endTime = 0;

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

QString Widget::timeConverter(float &time)     /*** Time converter to hh:mm:ss ***/
{
    int h = static_cast<int>(trunc(time / 3600));
    int m = static_cast<int>(trunc((time - float(h * 3600)) / 60));
    int s = static_cast<int>(round(time - float(h * 3600) - float(m * 60)));
    QString hrs = QString::number(h);
    QString min = QString::number(m);
    QString sec = QString::number(s);
    std::ostringstream sstr;
    sstr << std::setw(2) << std::setfill('0') << hrs.toStdString() << ":"
         << std::setw(2) << std::setfill('0') << min.toStdString() << ":"
         << std::setw(2) << std::setfill('0') << sec.toStdString();
    std::string time_str = sstr.str();

    return QString::fromStdString(time_str);
}

void Widget::on_comboBoxMode_currentIndexChanged(int index)
{
    if (index == 0) {
        _batch_mode = false;
    } else {
        _batch_mode = true;
    }
}

/************************************************
** Preview Window
************************************************/

void Widget::setThumbnail(QString curFilename, double time, QString quality)     /*** Thumbnail ***/
{
    QString qualityParam = "-vf scale=144:-1 -compression_level 10 -pix_fmt rgb24";
    if (quality == "low")
    {
        qualityParam = "-vf scale=144:-1,format=pal8,dctdnoiz=4.5";
    }
    const QString time_qstr = QString::number(time, 'f', 3);
    const QString tmb_name = curFilename.replace(".", "_").replace(" ", "_") + time_qstr;
    const QString tmb_file = _thumb_path + QString("/") + tmb_name + QString(".png");
    QFile tmb(tmb_file);
    if (!tmb.exists()) {
        //std::cout<< "Thumbnail file not exist and created..." << std::endl;
        QStringList cmd;
        cmd << "-hide_banner" << "-ss" << time_qstr << "-i" << _input_file
            << qualityParam.split(" ") << "-vframes" << "1" << "-y" << tmb_file;
        process_5->start("ffmpeg", cmd);
        process_5->waitForFinished();
    }
    ui->labelThumb->setPixmap(QPixmap(tmb_file).scaled(ui->labelThumb->size(),
                              Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void Widget::repeatHandler_Type_2()  /*** Repeat handler ***/
{
    std::cout << "Call by timer... " << std::endl;
    if (_row != -1) {
        setThumbnail(_curFilename, _curTime, "high");
    }
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
    ui->textBrowser_2->setText(_cur_param[curParamIndex::PRESET_NAME]);
    QString file_without_ext("");
    QString extension("");
    QString suffix("");
    QString prefix("");
    QString arr[NUMBER_PRESETS][5] = {
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MPG", "AVI", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"WebM", "MKV", "",    "",     ""},
        {"WebM", "MKV", "",    "",     ""},
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
        {"MKV",  "MOV", "MP4", "M2TS", "TS"}
    };
    extension = arr[_cur_param[curParamIndex::CODEC].toInt()][_cur_param[curParamIndex::CONTAINER].toInt()].toLower();

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
    QString output_folder_name = callFileDialog("Select output folder");
    if (output_folder_name.isEmpty()) {
        return;
    }
    _output_folder = output_folder_name;
    if (_row != -1) {
        get_output_filename();
    }
}

QString Widget::callFileDialog(const QString title)  /*** Call file dialog ***/
{
    QFileDialog *selectFolderWindow = new QFileDialog(nullptr);
    selectFolderWindow->setFileMode(QFileDialog::DirectoryOnly);
#ifdef Q_OS_WIN
    selectFolderWindow->setOptions(QFileDialog::ShowDirsOnly |
                                   QFileDialog::ReadOnly);
#endif
#ifdef GNOME_DESKTOP
    selectFolderWindow->setOptions(QFileDialog::ShowDirsOnly |
                                   QFileDialog::DontUseNativeDialog |
                                   QFileDialog::ReadOnly);
#endif
#ifdef KDE_DESKTOP
    selectFolderWindow->setOptions(QFileDialog::ShowDirsOnly |
                                   QFileDialog::ReadOnly);
#endif
    QString directory = QDir::homePath();
    if (_output_folder != "") {
        directory = _output_folder;
    }
    selectFolderWindow->setDirectory(directory);
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

/************************************************
** Options Window
************************************************/

void Widget::on_lineEditTitleVideo_editingFinished()
{
    if (_row != -1) {
        QString videoTitle = ui->lineEditTitleVideo->text();
        QTableWidgetItem *newItem_videoTitle = new QTableWidgetItem(videoTitle);
        ui->tableWidget->setItem(_row, columnIndex::T_VIDEOTITLE, newItem_videoTitle);
        _videoMetadata[VIDEO_TITLE] = ui->tableWidget->item(_row, columnIndex::T_VIDEOTITLE)->text();
    }
}

void Widget::on_lineEditAuthorVideo_editingFinished()
{
    if (_row != -1) {
        QString videoAuthor = ui->lineEditAuthorVideo->text();
        QTableWidgetItem *newItem_videoAuthor = new QTableWidgetItem(videoAuthor);
        ui->tableWidget->setItem(_row, columnIndex::T_VIDEOAUTHOR, newItem_videoAuthor);
        _videoMetadata[VIDEO_AUTHOR] = ui->tableWidget->item(_row, columnIndex::T_VIDEOAUTHOR)->text();
    }
}

void Widget::on_lineEditYearVideo_editingFinished()
{
    if (_row != -1) {
        QString videoYear = ui->lineEditYearVideo->text();
        QTableWidgetItem *newItem_videoYear = new QTableWidgetItem(videoYear);
        ui->tableWidget->setItem(_row, columnIndex::T_VIDEOYEAR, newItem_videoYear);
        _videoMetadata[VIDEO_YEAR] = ui->tableWidget->item(_row, columnIndex::T_VIDEOYEAR)->text();
    }
}

void Widget::on_lineEditPerfVideo_editingFinished()
{
    if (_row != -1) {
        QString videoPerf = ui->lineEditPerfVideo->text();
        QTableWidgetItem *newItem_videoPerf = new QTableWidgetItem(videoPerf);
        ui->tableWidget->setItem(_row, columnIndex::T_VIDEOPERF, newItem_videoPerf);
        _videoMetadata[VIDEO_PERFORMER] = ui->tableWidget->item(_row, columnIndex::T_VIDEOPERF)->text();
    }
}

void Widget::on_lineEditMovieNameVideo_editingFinished()
{
    if (_row != -1) {
        QString videoMovieName = ui->lineEditMovieNameVideo->text();
        QTableWidgetItem *newItem_videoMovieName = new QTableWidgetItem(videoMovieName);
        ui->tableWidget->setItem(_row, columnIndex::T_VIDEOMOVIENAME, newItem_videoMovieName);
        _videoMetadata[VIDEO_MOVIENAME] = ui->tableWidget->item(_row, columnIndex::T_VIDEOMOVIENAME)->text();
    }
}

void Widget::on_lineEditDescriptionVideo_editingFinished()
{
    if (_row != -1) {
        QString videoDescription = ui->lineEditDescriptionVideo->text();
        QTableWidgetItem *newItem_videoDescription = new QTableWidgetItem(videoDescription);
        ui->tableWidget->setItem(_row, columnIndex::T_VIDEODESCR, newItem_videoDescription);
        _videoMetadata[VIDEO_DESCRIPTION] = ui->tableWidget->item(_row, columnIndex::T_VIDEODESCR)->text();
    }
}

void Widget::on_checkBoxAudio_1_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxAudio_1->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOCHECK_1, newItem_checkstate);
        _audioStreamCheckState[0] = (ui->tableWidget->item(_row, columnIndex::T_AUDIOCHECK_1)->text()).toInt();
    }
}

void Widget::on_checkBoxAudio_2_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxAudio_2->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOCHECK_2, newItem_checkstate);
        _audioStreamCheckState[1] = (ui->tableWidget->item(_row, columnIndex::T_AUDIOCHECK_2)->text()).toInt();
    }
}

void Widget::on_checkBoxAudio_3_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxAudio_3->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOCHECK_3, newItem_checkstate);
        _audioStreamCheckState[2] = (ui->tableWidget->item(_row, columnIndex::T_AUDIOCHECK_3)->text()).toInt();
    }
}

void Widget::on_checkBoxAudio_4_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxAudio_4->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOCHECK_4, newItem_checkstate);
        _audioStreamCheckState[3] = (ui->tableWidget->item(_row, columnIndex::T_AUDIOCHECK_4)->text()).toInt();
    }
}

void Widget::on_checkBoxAudio_5_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxAudio_5->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOCHECK_5, newItem_checkstate);
        _audioStreamCheckState[4] = (ui->tableWidget->item(_row, columnIndex::T_AUDIOCHECK_5)->text()).toInt();
    }
}

void Widget::on_checkBoxAudio_6_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxAudio_6->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOCHECK_6, newItem_checkstate);
        _audioStreamCheckState[5] = (ui->tableWidget->item(_row, columnIndex::T_AUDIOCHECK_6)->text()).toInt();
    }
}

void Widget::on_checkBoxAudio_7_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxAudio_7->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOCHECK_7, newItem_checkstate);
        _audioStreamCheckState[6] = (ui->tableWidget->item(_row, columnIndex::T_AUDIOCHECK_7)->text()).toInt();
    }
}

void Widget::on_checkBoxAudio_8_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxAudio_8->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOCHECK_8, newItem_checkstate);
        _audioStreamCheckState[7] = (ui->tableWidget->item(_row, columnIndex::T_AUDIOCHECK_8)->text()).toInt();
    }
}

void Widget::on_checkBoxAudio_9_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxAudio_9->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOCHECK_9, newItem_checkstate);
        _audioStreamCheckState[8] = (ui->tableWidget->item(_row, columnIndex::T_AUDIOCHECK_9)->text()).toInt();
    }
}

void Widget::on_lineEditLangAudio_1_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_1->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_1, newItem_langAudio);
        _audioLang[0] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_1)->text();
    }
}

void Widget::on_lineEditLangAudio_2_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_2->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_2, newItem_langAudio);
        _audioLang[1] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_2)->text();
    }
}

void Widget::on_lineEditLangAudio_3_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_3->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_3, newItem_langAudio);
        _audioLang[2] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_3)->text();
    }
}

void Widget::on_lineEditLangAudio_4_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_4->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_4, newItem_langAudio);
        _audioLang[3] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_4)->text();
    }
}

void Widget::on_lineEditLangAudio_5_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_5->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_5, newItem_langAudio);
        _audioLang[4] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_5)->text();
    }
}

void Widget::on_lineEditLangAudio_6_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_6->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_6, newItem_langAudio);
        _audioLang[5] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_6)->text();
    }
}

void Widget::on_lineEditLangAudio_7_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_7->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_7, newItem_langAudio);
        _audioLang[6] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_7)->text();
    }
}

void Widget::on_lineEditLangAudio_8_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_8->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_8, newItem_langAudio);
        _audioLang[7] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_8)->text();
    }
}

void Widget::on_lineEditLangAudio_9_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_9->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_9, newItem_langAudio);
        _audioLang[8] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_9)->text();
    }
}

void Widget::on_lineEditTitleAudio_1_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_1->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_1, newItem_titleAudio);
        _audioTitle[0] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_1)->text();
    }
}

void Widget::on_lineEditTitleAudio_2_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_2->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_2, newItem_titleAudio);
        _audioTitle[1] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_2)->text();
    }
}

void Widget::on_lineEditTitleAudio_3_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_3->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_3, newItem_titleAudio);
        _audioTitle[2] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_3)->text();
    }
}

void Widget::on_lineEditTitleAudio_4_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_4->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_4, newItem_titleAudio);
        _audioTitle[3] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_4)->text();
    }
}

void Widget::on_lineEditTitleAudio_5_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_5->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_5, newItem_titleAudio);
        _audioTitle[4] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_5)->text();
    }
}

void Widget::on_lineEditTitleAudio_6_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_6->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_6, newItem_titleAudio);
        _audioTitle[5] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_6)->text();
    }
}

void Widget::on_lineEditTitleAudio_7_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_7->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_7, newItem_titleAudio);
        _audioTitle[6] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_7)->text();
    }
}

void Widget::on_lineEditTitleAudio_8_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_8->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_8, newItem_titleAudio);
        _audioTitle[7] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_8)->text();
    }
}

void Widget::on_lineEditTitleAudio_9_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_9->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_9, newItem_titleAudio);
        _audioTitle[8] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_9)->text();
    }
}

void Widget::on_checkBoxSubtitle_1_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxSubtitle_1->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBCHECK_1, newItem_checkstate);
        _subtitleCheckState[0] = (ui->tableWidget->item(_row, columnIndex::T_SUBCHECK_1)->text()).toInt();
    }
}

void Widget::on_checkBoxSubtitle_2_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxSubtitle_2->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBCHECK_2, newItem_checkstate);
        _subtitleCheckState[1] = (ui->tableWidget->item(_row, columnIndex::T_SUBCHECK_2)->text()).toInt();
    }
}

void Widget::on_checkBoxSubtitle_3_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxSubtitle_3->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBCHECK_3, newItem_checkstate);
        _subtitleCheckState[2] = (ui->tableWidget->item(_row, columnIndex::T_SUBCHECK_3)->text()).toInt();
    }
}

void Widget::on_checkBoxSubtitle_4_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxSubtitle_4->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBCHECK_4, newItem_checkstate);
        _subtitleCheckState[3] = (ui->tableWidget->item(_row, columnIndex::T_SUBCHECK_4)->text()).toInt();
    }
}

void Widget::on_checkBoxSubtitle_5_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxSubtitle_5->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBCHECK_5, newItem_checkstate);
        _subtitleCheckState[4] = (ui->tableWidget->item(_row, columnIndex::T_SUBCHECK_5)->text()).toInt();
    }
}

void Widget::on_checkBoxSubtitle_6_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxSubtitle_6->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBCHECK_6, newItem_checkstate);
        _subtitleCheckState[5] = (ui->tableWidget->item(_row, columnIndex::T_SUBCHECK_6)->text()).toInt();
    }
}

void Widget::on_checkBoxSubtitle_7_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxSubtitle_7->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBCHECK_7, newItem_checkstate);
        _subtitleCheckState[6] = (ui->tableWidget->item(_row, columnIndex::T_SUBCHECK_7)->text()).toInt();
    }
}

void Widget::on_checkBoxSubtitle_8_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxSubtitle_8->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBCHECK_8, newItem_checkstate);
        _subtitleCheckState[7] = (ui->tableWidget->item(_row, columnIndex::T_SUBCHECK_8)->text()).toInt();
    }
}

void Widget::on_checkBoxSubtitle_9_clicked()
{
    if (_row != -1) {
        QString state_qstr = "0";
        int state = ui->checkBoxSubtitle_9->checkState();
        if (state == 2) {
            state_qstr = "1";
        }
        QTableWidgetItem *newItem_checkstate = new QTableWidgetItem(state_qstr);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBCHECK_9, newItem_checkstate);
        _subtitleCheckState[8] = (ui->tableWidget->item(_row, columnIndex::T_SUBCHECK_9)->text()).toInt();
    }
}

void Widget::on_lineEditLangSubtitle_1_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_1->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_1, newItem_langSubtitle);
        _subtitleLang[0] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_1)->text();
    }
}

void Widget::on_lineEditLangSubtitle_2_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_2->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_2, newItem_langSubtitle);
        _subtitleLang[1] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_2)->text();
    }
}

void Widget::on_lineEditLangSubtitle_3_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_3->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_3, newItem_langSubtitle);
        _subtitleLang[2] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_3)->text();
    }
}

void Widget::on_lineEditLangSubtitle_4_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_4->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_4, newItem_langSubtitle);
        _subtitleLang[3] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_4)->text();
    }
}

void Widget::on_lineEditLangSubtitle_5_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_5->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_5, newItem_langSubtitle);
        _subtitleLang[4] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_5)->text();
    }
}

void Widget::on_lineEditLangSubtitle_6_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_6->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_6, newItem_langSubtitle);
        _subtitleLang[5] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_6)->text();
    }
}

void Widget::on_lineEditLangSubtitle_7_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_7->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_7, newItem_langSubtitle);
        _subtitleLang[6] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_7)->text();
    }
}

void Widget::on_lineEditLangSubtitle_8_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_8->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_8, newItem_langSubtitle);
        _subtitleLang[7] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_8)->text();
    }
}

void Widget::on_lineEditLangSubtitle_9_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_9->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_9, newItem_langSubtitle);
        _subtitleLang[8] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_9)->text();
    }
}

void Widget::on_lineEditTitleSubtitle_1_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_1->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_1, newItem_titleSubtitle);
        _subtitleTitle[0] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_1)->text();
    }
}

void Widget::on_lineEditTitleSubtitle_2_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_2->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_2, newItem_titleSubtitle);
        _subtitleTitle[1] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_2)->text();
    }
}

void Widget::on_lineEditTitleSubtitle_3_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_3->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_3, newItem_titleSubtitle);
        _subtitleTitle[2] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_3)->text();
    }
}

void Widget::on_lineEditTitleSubtitle_4_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_4->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_4, newItem_titleSubtitle);
        _subtitleTitle[3] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_4)->text();
    }
}

void Widget::on_lineEditTitleSubtitle_5_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_5->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_5, newItem_titleSubtitle);
        _subtitleTitle[4] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_5)->text();
    }
}

void Widget::on_lineEditTitleSubtitle_6_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_6->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_6, newItem_titleSubtitle);
        _subtitleTitle[5] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_6)->text();
    }
}

void Widget::on_lineEditTitleSubtitle_7_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_7->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_7, newItem_titleSubtitle);
        _subtitleTitle[6] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_7)->text();
    }
}

void Widget::on_lineEditTitleSubtitle_8_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_8->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_8, newItem_titleSubtitle);
        _subtitleTitle[7] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_8)->text();
    }
}

void Widget::on_lineEditTitleSubtitle_9_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_9->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_9, newItem_titleSubtitle);
        _subtitleTitle[8] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_9)->text();
    }
}

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
        if (_startTime > _endTime && _endTime != 0.0)
        {
            _startTime = _endTime;
        }
        ui->lineEditStartTime->setText(timeConverter(_startTime));
        QTableWidgetItem *newItem_startTime = new QTableWidgetItem(QString::number(_startTime, 'f', 3));
        ui->tableWidget->setItem(_row, columnIndex::T_STARTTIME, newItem_startTime);
    }
}

void Widget::on_buttonSetEndTime_clicked()
{
    if (_row != -1) {
        _endTime = _curTime;
        if (_endTime < _startTime)
        {
            _endTime =_startTime;
        }
        ui->lineEditEndTime->setText(timeConverter(_endTime));
        QTableWidgetItem *newItem_endTime = new QTableWidgetItem(QString::number(_endTime, 'f', 3));
        ui->tableWidget->setItem(_row, columnIndex::T_ENDTIME, newItem_endTime);
    }
}

QString Widget::timeConverter(double &time)     /*** Time converter to hh:mm:ss.msc ***/
{
    int h = static_cast<int>(trunc(time / 3600));
    int m = static_cast<int>(trunc((time - double(h * 3600)) / 60));
    int s = static_cast<int>(trunc(time - double(h * 3600) - double(m * 60)));
    int ms = static_cast<int>(round(1000 * (time - double(h * 3600) - double(m * 60) - double(s))));

    QString hrs = QString::number(h);
    QString min = QString::number(m);
    QString sec = QString::number(s);
    QString msec = QString::number(ms);

    std::ostringstream sstr;
    sstr << std::setw(2) << std::setfill('0') << hrs.toStdString() << ":"
         << std::setw(2) << std::setfill('0') << min.toStdString() << ":"
         << std::setw(2) << std::setfill('0') << sec.toStdString() << "."
         << std::setw(3) << msec.toStdString();
    std::string time_str = sstr.str();

    return QString::fromStdString(time_str);
}

/************************************************
** Preset Window
************************************************/

void Widget::set_defaults() /*** Set default presets ***/
{
    std::cout<< "Set defaults..." << std::endl;
    QStringList line;
    QFile file;
    file.setFileName(":/resources/data/default_presets.txt");
    file.open(QFile::ReadOnly);
    while(!file.atEnd()) {
        line << file.readLine();
    }
    QStringList cur_param = line[0].split("<%>");
    for (int k = 0; k < PARAMETERS_COUNT; k++) {
        _cur_param[k] = cur_param[k];
    }
    _pos_top = cur_param[PARAMETERS_COUNT].toInt();
    _pos_cld = cur_param[PARAMETERS_COUNT+1].toInt();
    int n = line.size() - 1;
    for (int i = 0; i < PARAMETERS_COUNT+1; i++) {
      _preset_table[i].resize(n);
    }
    for (int j = 1; j <= n; j++) {
        cur_param = line[j].split("<&>");
        for (int m = 0; m < PARAMETERS_COUNT+1; m++) {
            _preset_table[m][j-1] = cur_param[m];
        }
    }
}

void Widget::on_buttonApplyPreset_clicked()  /*** Apply preset ***/
{
    int index = ui->treeWidget->currentIndex().row();
    if (index < 0) {
        std::cout << "Negative index..." << std::endl;
        _message = "Select preset first!\n";
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
        _message = "Select preset first!\n";
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
        std::cout << "Negative index..." << std::endl;
        return;
    }
    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem != nullptr) {
        // Item is child...
        _message = "Delete?";
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
            _message = "Delete presets first!\n";
            call_task_complete(_message, false);
        }
    }
}

void Widget::on_actionEdit_preset_clicked()  /*** Edit preset ***/
{
    int index = ui->treeWidget->currentIndex().row();
    if (index < 0) {
        std::cout << "Negative index..." << std::endl;
        _message = "Select preset first!\n";
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
        presetWindow.setParameters(&_presetWindowGeometry, _new_param);
        presetWindow.setModal(true);
        presetWindow.exec();  //******************** Go to Preset and wait for return *********************//
        for (int k = 0; k < PARAMETERS_COUNT; k++) {
            item->setText(k+7, _new_param[k]);
        }

        updateInfoFields(_new_param[1], _new_param[2], _new_param[3], _new_param[4],
                              _new_param[11], _new_param[12], _new_param[21], item);

        int index_top = ui->treeWidget->indexOfTopLevelItem(parentItem);
        int index_child = parentItem->indexOfChild(item);
        if (_pos_top == index_top && _pos_cld == index_child) {
            for (int k = 0; k < PARAMETERS_COUNT; k++) {
                _cur_param[k] = item->text(k+7);
            };
            if (_row != -1) {
                get_output_filename();
            }
        }
        updatePresetTable();

    } else {
        // Item is parent...
        _message = "Select preset first!\n";
        call_task_complete(_message, false);
    }
}

void Widget::add_section()  /*** Add section ***/
{
    QFont parentFont;
    parentFont.setBold(true);
    QIcon sectionIcon;
    sectionIcon.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-folder.png"), QSize(), QIcon::Normal, QIcon::Off);
    QTreeWidgetItem *root = new QTreeWidgetItem();
    root->setText(0, "New section");
    root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    root->setFont(0, parentFont);
    root->setIcon(0, sectionIcon);
    ui->treeWidget->addTopLevelItem(root);
    ui->treeWidget->setCurrentItem(root);
    root->setFirstColumnSpanned(true);
    updatePresetTable();
}

void Widget::add_preset()  /*** Add preset ***/
{
    int index = ui->treeWidget->currentIndex().row();
    if (index < 0) {
        std::cout << "Negative index..." << std::endl;
        return;
    }

    QTreeWidgetItem *item = ui->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    QTreeWidgetItem *child = new QTreeWidgetItem();

    QStringList line;
    QFile file;
    file.setFileName(":/resources/data/default_presets.txt");
    file.open(QFile::ReadOnly);
    while(!file.atEnd()) {
        line << file.readLine();
    }
    QStringList cur_param = line[0].split("<%>");
    for (int k = 0; k < PARAMETERS_COUNT; k++) {
        child->setText(k + 7, cur_param[k]);
    }
    //child->setText(0 + 7, "New preset");

    updateInfoFields(cur_param[1], cur_param[2], cur_param[3], cur_param[4],
                          cur_param[11], cur_param[12], cur_param[21], child);
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

void Widget::setItemStyle(QTreeWidgetItem *item)
{
    QColor foregroundChildColor;
    foregroundChildColor.setRgb(qRgb(50, 100, 157));
    item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    item->setTextAlignment(0, Qt::AlignLeft | Qt::AlignVCenter);
    item->setForeground(0, foregroundChildColor);
    for (int column = 1; column < 7; column++)
    {
        item->setTextAlignment(column, Qt::AlignCenter);
        item->setForeground(column, foregroundChildColor);
    }
}

void Widget::updateCurPresetPos(int &index_top, int &index_child)
{
    std::cout << "Pos top: " << _pos_top << " Pos child: " << _pos_cld
              << " >>>>>> Index top: " << index_top << " Index child: " << index_child << std::endl;
    //if (index_child == _pos_cld) {
    _pos_cld = -1;
    _cur_param[curParamIndex::PRESET_NAME] = "Preset not selected";
    if (_row != -1) {
        get_output_filename();
    }
    //}
}

void Widget::updateInfoFields(QString &codec_qstr, QString &mode_qstr, QString &container_qstr,
                              QString &bqr_qstr, QString &pass_qstr, QString &preset_qstr,
                              QString &acodec_qstr, QTreeWidgetItem *item)
{
    int codec = codec_qstr.toInt();
    int mode = mode_qstr.toInt();
    int preset = preset_qstr.toInt();
    int pass = pass_qstr.toInt();
    int acodec = acodec_qstr.toInt();
    int container = container_qstr.toInt();
    item->setText(0, updateFieldCodec(codec));
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
        "H.265/HEVC 4:2:0 10 bit  ",
        "H.265/HEVC 4:2:0 8 bit  ",
        "H.264/AVC 4:2:0 8 bit  ",
        "Intel QSV H.264/AVC 4:2:0 8 bit  ",
        "Intel QSV MPEG-2 4:2:0 8 bit  ",
        "NVENC H.265/HEVC 4:2:0 10 bit  ",
        "NVENC H.265/HEVC 4:2:0 8 bit  ",
        "NVENC H.264/AVC 4:2:0 8 bit  ",
        "VP9 4:2:0 10 bit  ",
        "VP9 4:2:0 8 bit  ",
        "ProRes Proxy  ",
        "ProRes LT  ",
        "ProRes Standard  ",
        "ProRes HQ  ",
        "ProRes 4444  ",
        "ProRes 4444XQ  ",
        "DNxHR LB  ",
        "DNxHR SQ  ",
        "DNxHR HQ  ",
        "DNxHR HQX  ",
        "DNxHR 444  ",
        "XDCAM HD422  ",
        "From source  "
    };
    return arr_codec[codec];
}

QString Widget::updateFieldMode(int &codec, int &mode)
{
    QString arr_mode[NUMBER_PRESETS][5] = {
        {"CBR",  "ABR", "VBR", "CRF", "CQP"},
        {"CBR",  "ABR", "VBR", "CRF", "CQP"},
        {"CBR",  "ABR", "VBR", "CRF", "CQP"},
        {"VBR",  "",    "",    "",    ""},
        {"CBR",  "",    "",    "",    ""},
        {"VBR",  "",    "",    "",    ""},
        {"VBR",  "",    "",    "",    ""},
        {"VBR",  "",    "",    "",    ""},
        {"ABR",  "CRF", "",    "",    ""},
        {"ABR",  "CRF", "",    "",    ""},
        {"Auto", "",    "",    "",    ""},
        {"Auto", "",    "",    "",    ""},
        {"Auto", "",    "",    "",    ""},
        {"Auto", "",    "",    "",    ""},
        {"Auto", "",    "",    "",    ""},
        {"Auto", "",    "",    "",    ""},
        {"Auto", "",    "",    "",    ""},
        {"Auto", "",    "",    "",    ""},
        {"Auto", "",    "",    "",    ""},
        {"Auto", "",    "",    "",    ""},
        {"Auto", "",    "",    "",    ""},
        {"VBR",  "",    "",    "",    ""},
        {"Auto", "",    "",    "",    ""}
    };
    return arr_mode[codec][mode];
}

QString Widget::updateFieldPreset(int &codec, int &preset)
{
    QString arr_preset[NUMBER_PRESETS][10] = {
        {"None", "Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow",     "Slower", "Veryslow"},
        {"None", "Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow",     "Slower", "Veryslow"},
        {"None", "Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow",     "Slower", "Veryslow"},
        {"None", "Veryfast",  "Faster",    "Fast",     "Medium", "Slow", "Slower", "Veryslow", "",       ""},
        {"None", "Veryfast",  "Faster",    "Fast",     "Medium", "Slow", "Slower", "Veryslow", "",       ""},
        {"None", "Slow",      "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "Slow",      "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "Slow",      "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""},
        {"None", "",          "",          "",         "",       "",     "",       "",         "",       ""}
    };
    return arr_preset[codec][preset];
}

QString Widget::updateFieldPass(int &codec, int &pass)
{
    QString arr_pass[NUMBER_PRESETS][2] = {
        {"1 Pass", "2 Pass"},
        {"1 Pass", "2 Pass"},
        {"1 Pass", "2 Pass"},
        {"Auto",   ""},
        {"Auto",   ""},
        {"2 Pass", ""},
        {"2 Pass", ""},
        {"2 Pass", ""},
        {"1 Pass", "2 Pass"},
        {"1 Pass", "2 Pass"},
        {"Auto",   ""},
        {"Auto",   ""},
        {"Auto",   ""},
        {"Auto",   ""},
        {"Auto",   ""},
        {"Auto",   ""},
        {"Auto",   ""},
        {"Auto",   ""},
        {"Auto",   ""},
        {"Auto",   ""},
        {"Auto",   ""},
        {"Auto",   ""},
        {"Auto",   ""}
    };
    return arr_pass[codec][pass];
}

QString Widget::updateFieldAcodec(int &codec, int &acodec)
{
    QString arr_acodec[NUMBER_PRESETS][6] = {
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"AAC",        "AC3",        "DTS",        "Source", "",     ""},
        {"Opus",       "Vorbis",     "Source",     "",       "",     ""},
        {"Opus",       "Vorbis",     "Source",     "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"PCM 16 bit", "PCM 24 bit", "PCM 32 bit", "",       "",     ""},
        {"PCM 16 bit", "",           "",           "",       "",     ""},
        {"AAC",        "AC3",        "DTS",        "Vorbis", "Opus", "Source"}
    };
    return arr_acodec[codec][acodec];
}

QString Widget::updateFieldContainer(int &codec, int &container)
{
    QString arr_container[NUMBER_PRESETS][5] = {
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MPG", "AVI", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"WebM", "MKV", "",    "",     ""},
        {"WebM", "MKV", "",    "",     ""},
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
        case 0:
        case 1:
        case 2:
            if (collapsed) {
                path = QString::fromUtf8(":/resources/icons/16x16/cil-folder.png");
            } else {
                path = QString::fromUtf8(":/resources/icons/16x16/cil-folder-open.png");}
            break;
        case 3:
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
        updatePresetTable();
    }
}

/************************************************
** Dialogs Windows
************************************************/

bool Widget::call_dialog(const QString &_message)  /*** Call dialog ***/
{
    bool acceptFlag = false;
    Dialog dialog(this);
    dialog.setMessage(_message, &acceptFlag);
    dialog.setModal(true);
    dialog.exec();
    return acceptFlag;
}

void Widget::call_task_complete(const QString &_message, const bool &_timer_mode)  /*** Call task complete ***/
{
    if (this->isHidden()) {
        if (_hideInTrayFlag && !_timer_mode) {
            trayIcon->showMessage(_message, "Task", QSystemTrayIcon::Information, 151000);
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

