#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "donate.h"
#include "settings.h"
#include "selectpreset.h"
#include "taskcomplete.h"
#include "dialog.h"

#ifdef Q_OS_LINUX
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



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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
}

MainWindow::~MainWindow()
{
    delete raiseLayout;
    delete ui;
}

void MainWindow::showEvent(QShowEvent *event)   //************** Call set parameters ****************//
{
    QMainWindow::showEvent(event);
    if (_windowActivated == false)
    {
        _windowActivated = true;
        setParameters();
    }
}

void MainWindow::setParameters()    //***************************** Set parameters ******************//
{
    // **************************** Set front label ***********************************//

    raiseThumb->setAlignment(Qt::AlignCenter);
    raiseThumb->setPixmap(QPixmap(":/resources/images/logo.png"));
    ui->tableWidget->setLayout(raiseLayout);
    raiseLayout->addWidget(raiseThumb);

    // ***************************** Set parameters ***********************************//

    ui->frame_hint->installEventFilter(this);
    ui->centralwidget->installEventFilter(this);
    raiseThumb->installEventFilter(this);
    timerCallSetThumbnail->setSingleShot(true);
    timerCallSetThumbnail->setInterval(800);
    connect(timerCallSetThumbnail, SIGNAL(timeout()), this, SLOT(repeatHandler_Type_2()));
    connect(timer, SIGNAL(timeout()), this, SLOT(repeat_handler()));
    _preset_table.resize(24);
    for (int i = 0; i < 24; i++) {
      _preset_table[i].resize(5);
    }

    // ***************************** Top menu actions ***********************************//

    connect(add_files, &QAction::triggered, this, &MainWindow::on_actionAdd_clicked);
    connect(remove_files, &QAction::triggered, this, &MainWindow::on_actionRemove_clicked);
    connect(close_prog, &QAction::triggered, this, &MainWindow::on_closeWindow_clicked);
    QMenu* menuFiles = new QMenu(this);
    menuFiles->addAction(add_files);
    menuFiles->addAction(remove_files);
    menuFiles->addSeparator();
    menuFiles->addAction(close_prog);
    ui->menuFileButton->setMenu(menuFiles);

    connect(select_preset, &QAction::triggered, this, &MainWindow::on_actionPreset_clicked);
    connect(encode_files, &QAction::triggered, this, &MainWindow::on_actionEncode_clicked);
    connect(stop_encode, &QAction::triggered, this, &MainWindow::on_actionStop_clicked);
    QMenu* menuEdit = new QMenu(this);
    menuEdit->addAction(select_preset);
    menuEdit->addSeparator();
    menuEdit->addAction(encode_files);
    menuEdit->addAction(stop_encode);
    ui->menuEditButton->setMenu(menuEdit);

    connect(edit_metadata, &QAction::triggered, this, &MainWindow::showMetadataEditor);
    connect(select_audio, &QAction::triggered, this, &MainWindow::showAudioStreamsSelection);
    connect(select_subtitles, &QAction::triggered, this, &MainWindow::showSubtitlesSelection);
    connect(split_video, &QAction::triggered, this, &MainWindow::showSplitVideo);
    QMenu* menuTools = new QMenu(this);
    menuTools->addAction(edit_metadata);
    menuTools->addSeparator();
    menuTools->addAction(select_audio);
    menuTools->addAction(select_subtitles);
    menuTools->addSeparator();
    menuTools->addAction(split_video);
    ui->menuToolsButton->setMenu(menuTools);

    connect(settings, &QAction::triggered, this, &MainWindow::on_actionSettings_clicked);
    QMenu* menuPreferences = new QMenu(this);
    menuPreferences->addAction(settings);
    ui->menuPreferencesButton->setMenu(menuPreferences);

    connect(about, &QAction::triggered, this, &MainWindow::on_actionAbout_clicked);
    connect(donate, &QAction::triggered, this, &MainWindow::on_actionDonate_clicked);
    QMenu* menuAbout = new QMenu(this);
    menuAbout->addAction(about);
    menuAbout->addSeparator();
    menuAbout->addAction(donate);
    ui->menuAboutButton->setMenu(menuAbout);

    // ****************************** Initialize variables ************************************//

    QDesktopWidget *screenSize = QApplication::desktop();
    int screenWidth = screenSize->width();
    int screenHeight = screenSize->height();
    int widthMainWindow = 1024;
    int heightMainWindow = 670;
    int x_pos = static_cast<int>(round(static_cast<float>(screenWidth - widthMainWindow)/2));
    int y_pos = static_cast<int>(round(static_cast<float>(screenHeight - heightMainWindow)/2));
    bool expandFlag = false;
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    _openDir = QDir::homePath();
    _settings_path = QDir::homePath() + QString("/CineEncoder");
    _thumb_path = _settings_path + QString("/thumbnails");
    _settings_file = _settings_path + QString("/ce_settings");
    _preset_file = _settings_path + QString("/ce_preset");
    _window_file = _settings_path + QString("/ce_window");
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
    _protection = false;
    _batch_mode = false;
    _showHDR_mode = false;
    _row = -1;
    _theme = 0;
    ui->frameLeft->hide();
    ui->buttonLeftWindow->hide();
    animation->setFileName(":/resources/icons/Animated/cil-spinner-circle.gif");
    ui->labelAnimation->setMovie(animation);
    ui->labelAnimation->hide();
    ui->label_53->hide();
    ui->label_54->hide();
    ui->label_55->hide();
    ui->progressBar->hide();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->horizontalHeader()->setVisible(true);
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

    process_1->setProcessChannelMode(QProcess::MergedChannels);
    process_1->setWorkingDirectory(QDir::homePath());

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
    _wind_file.setFileName(_window_file);
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
            if (line.size() == 7)
            {
                _temp_folder = line[0].replace("temp_folder:", "").replace("\n", "");
                _output_folder = line[1].replace("output_folder:", "").replace("\n", "");

                if (line[2].indexOf("true") != -1) {
                    _batch_mode = true;
                }

                if (line[3].indexOf("true") != -1) {
                    _showHDR_mode = true;
                }

                if (line[4].indexOf("true") != -1) {
                    _protection = true;
                }

                _timer_interval = (line[5].replace("timer_interval:", "").replace("\n", "")).toInt();
                if (_timer_interval < 15) {
                    _timer_interval = 30;
                }
                timer->setInterval(_timer_interval*1000);
                _theme = (line[6].replace("theme:", "").replace("\n", "")).toInt();
            } else {
                std::cout << "Setting file error, not enough parameters!!! " << std::endl;  // Debug info //
            }
            std::cout << "Temp folder: " << _temp_folder.toStdString() << std::endl;  // Debug info //
            std::cout << "Output folder: " << _output_folder.toStdString() << std::endl;  // Debug info //
            std::cout << "Batch mode: " << _batch_mode << std::endl;  // Debug info //
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
            if (line.size() > 2)
            {
                QStringList cur_param = line[0].split("<%>");
                if (cur_param.size() == 26)
                {
                    for (int k = 0; k < 23; k++) {
                        _cur_param[k] = cur_param[k];
                    }
                    _pos_top = cur_param[23].toInt();
                    _pos_cld = cur_param[24].toInt();
                    int n = line.size() - 1;
                    for (int i = 0; i < 24; i++) {
                      _preset_table[i].resize(n);
                    }
                    for (int j = 1; j <= n; j++) {
                        cur_param = line[j].split("<&>");
                        if (cur_param.size() == 25) {
                            for (int m = 0; m < 24; m++) {
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
    if (_wind_file.exists())
    {
        std::cout << "Window settings file exist ..." << std::endl;  // Debug info //
        if (_wind_file.open(QIODevice::ReadOnly | QIODevice::Text))   // Read settings from file
        {
            QStringList line;
            while(!_wind_file.atEnd())
            {
                line << _wind_file.readLine();
            }

            std::cout << "Number of lines in window settings file: " << line.size() << std::endl; // Debug info //
            if (line.size() == 6)
            {
                if (line[0].indexOf("true") != -1) {
                    expandFlag = true;
                }
                x_pos = line[1].replace("pos_x:", "").replace("\n", "").toInt();
                y_pos = line[2].replace("pos_y:", "").replace("\n", "").toInt();
                widthMainWindow = line[3].replace("width:", "").replace("\n", "").toInt();
                heightMainWindow = line[4].replace("height:", "").replace("\n", "").toInt();
                _openDir = line[5].replace("open_dir:", "").replace("\n", "");
            } else {
                std::cout << "Window settings file error, not enough parameters!!! " << std::endl;  // Debug info //
            }
            std::cout << "x_pos: " << x_pos << std::endl;  // Debug info //
            std::cout << "y_pos: " << y_pos << std::endl;  // Debug info //
            std::cout << "widthMainWindow: " << widthMainWindow << std::endl;  // Debug info //
            std::cout << "heightMainWindow: " << heightMainWindow << std::endl;  // Debug info //
            std::cout << "expand flag: " << expandFlag << std::endl;  // Debug info //
            _wind_file.close();
        } else {
            std::cout << "Window settings file error (cannot be open)." << std::endl;  // Debug info //
        }
    } else {
        std::cout << "Window settings file not exist ..." << std::endl;  // Debug info //
    }
    setTheme(_theme);
    if (expandFlag == true) {
        on_expandWindow_clicked();
    } else {
        this->setGeometry(x_pos, y_pos, widthMainWindow, heightMainWindow);
    }
}

void MainWindow::on_closeWindow_clicked()
{
    this->close();
}

void MainWindow::closeEvent(QCloseEvent *event) //*********** Show prompt when close app ************//
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
            for (int i = 0; i < 23; i++)
            {
                line += _cur_param[i] + QString("<%>");
            }
            line += QString::number(_pos_top) + QString("<%>") +  QString::number(_pos_cld) + QString("<%>\n");
            _prs_file.write(line.toUtf8());
            int m = _preset_table.size();  // Column count
            int n =  _preset_table[0].size();  // Row count
            if (n < 500 && m < 25)
            {
                for (int i = 0; i < n; i++)
                {
                    line = "";
                    for (int j = 0; j < m; j++)
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
        if (_wind_file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            if (_expandWindowsState) {
                _wind_file.write("expand:true\n");
            } else {
                _wind_file.write("expand:false\n");
            };
            QString line_1 = QString("pos_x:") + QString::number(this->pos().x()) + QString("\n");
            _wind_file.write(line_1.toUtf8());
            QString line_2 = QString("pos_y:") + QString::number(this->pos().y()) + QString("\n");
            _wind_file.write(line_2.toUtf8());
            QString line_3 = QString("width:") + QString::number(this->width()) + QString("\n");
            _wind_file.write(line_3.toUtf8());
            QString line_4 = QString("height:") + QString::number(this->height()) + QString("\n");
            _wind_file.write(line_4.toUtf8());
            QString line_5 = QString("open_dir:") + _openDir + QString("\n");
            _wind_file.write(line_5.toUtf8());
            _wind_file.close();
        }
        event->accept();
    }
}

void MainWindow::on_expandWindow_clicked()
{
    if (_expandWindowsState == false)
    {
        this->showMaximized();
        _expandWindowsState = true;
        QIcon iconShowMax;
        iconShowMax.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-clone.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->expandWindow->setIcon(iconShowMax);
    } else {
        this->showNormal();
        _expandWindowsState = false;
        QIcon iconShowNorm;
        iconShowNorm.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-media-stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->expandWindow->setIcon(iconShowNorm);
    }
}

void MainWindow::on_hideWindow_clicked()
{
    this->showMinimized();
}

void MainWindow::on_actionAdd_clicked() //**************************** Add files ********************//
{
    QFileDialog *openFilesWindow = new QFileDialog(this);
    openFilesWindow->setFileMode(QFileDialog::ExistingFiles);
#ifdef Q_OS_WIN
    openFilesWindow->setOptions(QFileDialog::DontResolveSymlinks);
#else
    openFilesWindow->setOptions(QFileDialog::DontUseNativeDialog | QFileDialog::DontResolveSymlinks);
#endif
    openFilesWindow->setStyleSheet("QWidget {color: rgb(10, 10, 10); background-color: "
                                   "rgb(120, 120, 120);} QHeaderView {color: rgb(10, 10, 10); "
                                   "background-color: transparent;} QHeaderView::section:horizontal "
                                   "{height: 20px; padding: 0px; border: 1px solid rgb(160, 160, 160); "
                                   "border-top-left-radius: 0px; border-top-right-radius: 0px; "
                                   "background-color: rgb(160, 160, 160);} QScrollBar {background-color: "
                                   "rgb(160, 160, 160);}");
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

void MainWindow::openFiles(const QStringList &openFileNames)
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
        for (int j = 0; j < AMOUNT_AUDIO_SRTEAMS; j++) {
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
        for (int j = 0; j < AMOUNT_AUDIO_SRTEAMS; j++) {
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
        i++;
    }
    MI.Close();
    ui->tableWidget->selectRow(ui->tableWidget->rowCount() - 1);
}

void MainWindow::on_actionRemove_clicked()  //******************* Remove file from table ************//
{
    _row = ui->tableWidget->currentRow();
    if (_row != -1) {
        ui->tableWidget->removeRow(_row);
    }
}

void MainWindow::on_tableWidget_itemSelectionChanged()  //******* Item selection changed ************//
{
    ui->labelAnimation->hide();
    ui->label_53->hide();
    ui->label_54->hide();
    ui->label_55->hide();
    ui->progressBar->hide();

    // **************************** Disable audio widgets ***********************************//
    ui->checkBoxAudio_1->setEnabled(false);
    ui->checkBoxAudio_2->setEnabled(false);
    ui->checkBoxAudio_3->setEnabled(false);
    ui->checkBoxAudio_4->setEnabled(false);
    ui->checkBoxAudio_5->setEnabled(false);
    ui->checkBoxAudio_6->setEnabled(false);
    ui->checkBoxAudio_7->setEnabled(false);
    ui->checkBoxAudio_8->setEnabled(false);
    ui->checkBoxAudio_9->setEnabled(false);

    ui->lineEditLangAudio_1->setEnabled(false);
    ui->lineEditLangAudio_2->setEnabled(false);
    ui->lineEditLangAudio_3->setEnabled(false);
    ui->lineEditLangAudio_4->setEnabled(false);
    ui->lineEditLangAudio_5->setEnabled(false);
    ui->lineEditLangAudio_6->setEnabled(false);
    ui->lineEditLangAudio_7->setEnabled(false);
    ui->lineEditLangAudio_8->setEnabled(false);
    ui->lineEditLangAudio_9->setEnabled(false);

    ui->lineEditTitleAudio_1->setEnabled(false);
    ui->lineEditTitleAudio_2->setEnabled(false);
    ui->lineEditTitleAudio_3->setEnabled(false);
    ui->lineEditTitleAudio_4->setEnabled(false);
    ui->lineEditTitleAudio_5->setEnabled(false);
    ui->lineEditTitleAudio_6->setEnabled(false);
    ui->lineEditTitleAudio_7->setEnabled(false);
    ui->lineEditTitleAudio_8->setEnabled(false);
    ui->lineEditTitleAudio_9->setEnabled(false);

    // **************************** Disable subtitle widgets ***********************************//
    ui->checkBoxSubtitle_1->setEnabled(false);
    ui->checkBoxSubtitle_2->setEnabled(false);
    ui->checkBoxSubtitle_3->setEnabled(false);
    ui->checkBoxSubtitle_4->setEnabled(false);
    ui->checkBoxSubtitle_5->setEnabled(false);
    ui->checkBoxSubtitle_6->setEnabled(false);
    ui->checkBoxSubtitle_7->setEnabled(false);
    ui->checkBoxSubtitle_8->setEnabled(false);
    ui->checkBoxSubtitle_9->setEnabled(false);

    ui->lineEditLangSubtitle_1->setEnabled(false);
    ui->lineEditLangSubtitle_2->setEnabled(false);
    ui->lineEditLangSubtitle_3->setEnabled(false);
    ui->lineEditLangSubtitle_4->setEnabled(false);
    ui->lineEditLangSubtitle_5->setEnabled(false);
    ui->lineEditLangSubtitle_6->setEnabled(false);
    ui->lineEditLangSubtitle_7->setEnabled(false);
    ui->lineEditLangSubtitle_8->setEnabled(false);
    ui->lineEditLangSubtitle_9->setEnabled(false);

    ui->lineEditTitleSubtitle_1->setEnabled(false);
    ui->lineEditTitleSubtitle_2->setEnabled(false);
    ui->lineEditTitleSubtitle_3->setEnabled(false);
    ui->lineEditTitleSubtitle_4->setEnabled(false);
    ui->lineEditTitleSubtitle_5->setEnabled(false);
    ui->lineEditTitleSubtitle_6->setEnabled(false);
    ui->lineEditTitleSubtitle_7->setEnabled(false);
    ui->lineEditTitleSubtitle_8->setEnabled(false);
    ui->lineEditTitleSubtitle_9->setEnabled(false);

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
        ui->lineEditLangAudio_1->clear();
        ui->lineEditLangAudio_2->clear();
        ui->lineEditLangAudio_3->clear();
        ui->lineEditLangAudio_4->clear();
        ui->lineEditLangAudio_5->clear();
        ui->lineEditLangAudio_6->clear();
        ui->lineEditLangAudio_7->clear();
        ui->lineEditLangAudio_8->clear();
        ui->lineEditLangAudio_9->clear();

        ui->lineEditTitleAudio_1->clear();
        ui->lineEditTitleAudio_2->clear();
        ui->lineEditTitleAudio_3->clear();
        ui->lineEditTitleAudio_4->clear();
        ui->lineEditTitleAudio_5->clear();
        ui->lineEditTitleAudio_6->clear();
        ui->lineEditTitleAudio_7->clear();
        ui->lineEditTitleAudio_8->clear();
        ui->lineEditTitleAudio_9->clear();

        ui->checkBoxAudio_1->setChecked(false);
        ui->checkBoxAudio_2->setChecked(false);
        ui->checkBoxAudio_3->setChecked(false);
        ui->checkBoxAudio_4->setChecked(false);
        ui->checkBoxAudio_5->setChecked(false);
        ui->checkBoxAudio_6->setChecked(false);
        ui->checkBoxAudio_7->setChecked(false);
        ui->checkBoxAudio_8->setChecked(false);
        ui->checkBoxAudio_9->setChecked(false);

        // **************************** Reset subtitle widgets ***********************************//
        ui->lineEditLangSubtitle_1->clear();
        ui->lineEditLangSubtitle_2->clear();
        ui->lineEditLangSubtitle_3->clear();
        ui->lineEditLangSubtitle_4->clear();
        ui->lineEditLangSubtitle_5->clear();
        ui->lineEditLangSubtitle_6->clear();
        ui->lineEditLangSubtitle_7->clear();
        ui->lineEditLangSubtitle_8->clear();
        ui->lineEditLangSubtitle_9->clear();

        ui->lineEditTitleSubtitle_1->clear();
        ui->lineEditTitleSubtitle_2->clear();
        ui->lineEditTitleSubtitle_3->clear();
        ui->lineEditTitleSubtitle_4->clear();
        ui->lineEditTitleSubtitle_5->clear();
        ui->lineEditTitleSubtitle_6->clear();
        ui->lineEditTitleSubtitle_7->clear();
        ui->lineEditTitleSubtitle_8->clear();
        ui->lineEditTitleSubtitle_9->clear();

        ui->checkBoxSubtitle_1->setChecked(false);
        ui->checkBoxSubtitle_2->setChecked(false);
        ui->checkBoxSubtitle_3->setChecked(false);
        ui->checkBoxSubtitle_4->setChecked(false);
        ui->checkBoxSubtitle_5->setChecked(false);
        ui->checkBoxSubtitle_6->setChecked(false);
        ui->checkBoxSubtitle_7->setChecked(false);
        ui->checkBoxSubtitle_8->setChecked(false);
        ui->checkBoxSubtitle_9->setChecked(false);

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
        for (int i = 0; i < AMOUNT_AUDIO_SRTEAMS; i++) {
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

void MainWindow::make_preset()  //*********************************** Make preset *******************//
{
    std::cout << "Make preset..." << std::endl;                       // Debug information //
    int _CODEC = _cur_param[CODEC].toInt();                           //p1 - int codec
    int _MODE = _cur_param[MODE].toInt();                             //p2 - int mode
    QString _BQR = _cur_param[BQR];                                   //p4 - QString bitrate/quantizer/ratefactor
    QString _MAXRATE = _cur_param[MAXRATE];                           //p5 - QString maxrate
    QString _BUFSIZE = _cur_param[BUFSIZE];                           //p6 - QString bufsize
    bool _RESIZE_ENABLED = bool(_cur_param[RESIZE_ENABLED].toInt());  //p7 - bool checkBox_2 isEnabled
    int _RESIZE_CHECKSTATE = _cur_param[RESIZE_CHECKSTATE].toInt();   //p8 - int  checkBox_2 checkState
    QString _WIDTH = _cur_param[WIDTH];                               //p9 - QString width
    QString _HEIGHT = _cur_param[HEIGHT];                             //p10 - QString height
    int _PASS = _cur_param[PASS].toInt();                             //p11 - int pass
    int _PRESET = _cur_param[PRESET].toInt();                         //p12 - int preset
    int _COLOR_RANGE = _cur_param[COLOR_RANGE].toInt();               //p13 - int color_range
    QString _MIN_LUM = _cur_param[MIN_LUM];                           //p14 - QString min_lum
    QString _MAX_LUM = _cur_param[MAX_LUM];                           //p15 - QString max_lum
    QString _MAX_CLL = _cur_param[MAX_CLL];                           //p16 - QString max_cll
    QString _MAX_FALL = _cur_param[MAX_FALL];                         //p17 - QString max_fall
    int _MASTER_DISPLAY = _cur_param[MASTER_DISPLAY].toInt();         //p18 - int master_disp
    QString _CHROMA_COORD = _cur_param[CHROMA_COORD];                 //p19 - QString chroma_coord
    QString _WHITE_COORD = _cur_param[WHITE_COORD];                   //p20 - QString white_coord
    int _AUDIO_CODEC = _cur_param[AUDIO_CODEC].toInt();               //p21 - int acodec
    int _AUDIO_BITRATE = _cur_param[AUDIO_BITRATE].toInt();           //p22 - int abitrate

    _preset_0 = "";
    _preset_pass1 = "";
    _preset = "";
    _preset_mkvmerge = "";
    _error_message = "";
    _flag_two_pass = false;
    _flag_hdr = false;
    _mux_mode = false;
    _fr_count = 0;

    QString resize;
    if ((_RESIZE_ENABLED == true) && (_RESIZE_CHECKSTATE == 2)) {
        if ((_WIDTH != "") && (_HEIGHT != "") && (_WIDTH != "Source") && (_HEIGHT != "Source")) {
            resize = QString("-vf scale=%1:%2 ").arg(_WIDTH, _HEIGHT);
        }
    }

    QString _splitStartParam = "";
    QString _splitParam = "";
    double fps_double = _fps.toDouble();

    if (_endTime > 0 && _startTime < _endTime) {
        double duration = _endTime - _startTime;
        _fr_count = static_cast<int>(round(duration * fps_double));
        int startFrame = static_cast<int>(round(_startTime * fps_double));
        int endFrame = static_cast<int>(round(_endTime * fps_double));
        int amountFrames = endFrame - startFrame;
        _splitStartParam = QString(" -ss %1").arg(QString::number(_startTime, 'f', 3));
        _splitParam = QString("-vframes %1 ").arg(QString::number(amountFrames));
    } else {
        _fr_count = static_cast<int>(round(_dur * fps_double));
    }

    QString videoMetadata[6] = {"", "", "", "", "", ""};
    QString _videoMetadataParam = "";
    QString globalTitle = ui->lineEditGlobalTitle->text();

    if (globalTitle != "") {
        videoMetadata[0] = QString("-metadata:s:v:0 title=%1 ").arg(globalTitle.replace(" ", "\u00A0"));
    } else {
        if (_videoMetadata[VIDEO_TITLE] != "") {
            videoMetadata[0] = QString("-metadata:s:v:0 title=%1 ").arg(_videoMetadata[VIDEO_TITLE].replace(" ", "\u00A0"));
        } else {
            videoMetadata[0] = QString("-map_metadata:s:v:0 -1 ");
        }
    }
    if (_videoMetadata[VIDEO_MOVIENAME] != "") {
        videoMetadata[1] = QString("-metadata title=%1 ").arg(_videoMetadata[VIDEO_MOVIENAME].replace(" ", "\u00A0"));
    }
    if (_videoMetadata[VIDEO_AUTHOR] != "") {
        videoMetadata[2] = QString("-metadata author=%1 ").arg(_videoMetadata[VIDEO_AUTHOR].replace(" ", "\u00A0"));
    }
    if (_videoMetadata[VIDEO_DESCRIPTION] != "") {
        videoMetadata[3] = QString("-metadata description=%1 ").arg(_videoMetadata[VIDEO_DESCRIPTION].replace(" ", "\u00A0"));
    }
    if (_videoMetadata[VIDEO_YEAR] != "") {
        videoMetadata[4] = QString("-metadata year=%1 ").arg(_videoMetadata[VIDEO_YEAR].replace(" ", ""));
    }
    if (_videoMetadata[VIDEO_PERFORMER] != "") {
        videoMetadata[5] = QString("-metadata author=%1 ").arg(_videoMetadata[VIDEO_PERFORMER].replace(" ", "\u00A0"));
    }
    for (int i = 0; i < 6; i++) {
        _videoMetadataParam += videoMetadata[i];
    }

    QString audioLang[AMOUNT_AUDIO_SRTEAMS] = {"", "", "", "", "", "", "", "", ""};
    QString audioTitle[AMOUNT_AUDIO_SRTEAMS] = {"", "", "", "", "", "", "", "", ""};
    QString audioMap[AMOUNT_AUDIO_SRTEAMS] = {"", "", "", "", "", "", "", "", ""};
    QString _audioMapParam = "";
    QString _audioMetadataParam = "";
    int countDestAudioStream = 0;

    for (int k = 0; k < AMOUNT_AUDIO_SRTEAMS; k++) {
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


    QString arr_codec[20][3] = {
        {"-pix_fmt yuv420p10le -c:v libx265 -profile:v main10 ",        "",               "1"},
        {"-pix_fmt yuv420p -c:v libx265 -profile:v main ",              "",               "0"},
        {"-pix_fmt yuv420p -c:v libx264 -profile:v high ",              "",               "0"},
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
        {"-movflags +write_colr -c:v copy ",                            "",               "1"}
    };
    QString codec = QString("-map 0:v:0? ") + _audioMapParam + _subtitleMapParam +
                    QString("-map_metadata -1 ") + _videoMetadataParam + _audioMetadataParam +
                    _subtitleMetadataParam + arr_codec[_CODEC][0];

    QString hwaccel = arr_codec[_CODEC][1];
    _flag_hdr = static_cast<bool>(arr_codec[_CODEC][2].toInt());

    QString arr_mode[20][5] = {
        {"CBR", "ABR", "VBR", "CRF", "CQP"},
        {"CBR", "ABR", "VBR", "CRF", "CQP"},
        {"CBR", "ABR", "VBR", "CRF", "CQP"},
        {"VBR", "",    "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"VBR", "",    "",    "",    ""},
        {"ABR", "CRF", "",    "",    ""},
        {"ABR", "CRF", "",    "",    ""},
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
        {"",    "",    "",    "",    ""}
    };
    QString mode = "";
    if (arr_mode[_CODEC][_MODE] == "CBR") {
        mode = QString("-b:v %1 -minrate %1 -maxrate %1 -bufsize %1 ").arg(_BQR);
    }
    if (arr_mode[_CODEC][_MODE] == "ABR") {
        mode = QString("-b:v %1 ").arg(_BQR);
    }
    if (arr_mode[_CODEC][_MODE] == "VBR") {
        if ((_CODEC >= 3) && (_CODEC <= 5)) {
            mode = QString("-b:v %1 -maxrate %2 -bufsize %3 -rc vbr_hq -rc-lookahead:v 32 ").arg(_BQR, _MAXRATE, _BUFSIZE);
        } else {
            mode = QString("-b:v %1 -maxrate %2 -bufsize %3 ").arg(_BQR, _MAXRATE, _BUFSIZE);
        }
    }
    if (arr_mode[_CODEC][_MODE] == "CRF") {
        mode = QString("-b:v 0 -crf %1 ").arg(_BQR);
    }
    if (arr_mode[_CODEC][_MODE] == "CQP") {
        mode = QString("-b:v 0 -cq %1 -qmin %1 -qmax %1 ").arg(_BQR);
    }

    QString arr_preset[20][9] = {
        {"Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow", "Slower", "Veryslow"},
        {"Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow", "Slower", "Veryslow"},
        {"Ultrafast", "Superfast", "Veryfast", "Faster", "Fast", "Medium", "Slow", "Slower", "Veryslow"},
        {"Slow",      "",          "",         "",       "",     "",       "",     "",       ""},
        {"Slow",      "",          "",         "",       "",     "",       "",     "",       ""},
        {"Slow",      "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""},
        {"",          "",          "",         "",       "",     "",       "",     "",       ""}
    };
    QString preset = "";
    if (arr_preset[_CODEC][_PRESET] != "") {
        preset = "-preset " + arr_preset[_CODEC][_PRESET].toLower() + " ";
    }

    QString arr_pass[20][2] = {
        {"",                     "-x265-params pass=2 "},
        {"",                     "-x265-params pass=2 "},
        {"",                     "-pass 2 "},
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

    QString arr_acodec[20][6] = {
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
        {"AAC",   "AC3",    "DTS",    "Vorbis", "Opus", "Source"}
    };

    QString arr_bitrate[5][17] = {
        {"384k",  "320k",  "256k",  "192k",  "128k",  "96k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AAC
        {"640k",  "448k",  "384k",  "256k",  "",      "",      "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // AC3
        {"3840k", "3072k", "2048k", "1920k", "1536k", "1472k", "1344k", "1280k", "1152k", "1024k", "960k", "768k", "640k", "576k", "512k", "448k", "384k"}, // DTS
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""}, // Vorbis
        {"448k",  "384k",  "256k",  "128k",  "96k",   "64k",   "",      "",      "",      "",      "",     "",     "",     "",     "",     "",     ""} // Opus
    };
    QString acodec = "";
    if (arr_acodec[_CODEC][_AUDIO_CODEC] == "AAC") {
        acodec = QString("-c:a aac -b:a %1").arg(arr_bitrate[0][_AUDIO_BITRATE]);
    }
    if (arr_acodec[_CODEC][_AUDIO_CODEC] == "AC3") {
        acodec = QString("-c:a ac3 -b:a %1").arg(arr_bitrate[1][_AUDIO_BITRATE]);
    }
    if (arr_acodec[_CODEC][_AUDIO_CODEC] == "DTS") {
        acodec = QString("-strict -2 -c:a dca -b:a %1 -ar 48000").arg(arr_bitrate[2][_AUDIO_BITRATE]);
    }
    if (arr_acodec[_CODEC][_AUDIO_CODEC] == "Vorbis") {
        acodec = QString("-c:a libvorbis -ac 2 -b:a %1").arg(arr_bitrate[3][_AUDIO_BITRATE]);
    }
    if (arr_acodec[_CODEC][_AUDIO_CODEC] == "Opus") {
        acodec = QString("-c:a libopus -ac 2 -b:a %1").arg(arr_bitrate[4][_AUDIO_BITRATE]);
    }
    if (arr_acodec[_CODEC][_AUDIO_CODEC] == "PCM16") {
        acodec = "-c:a pcm_s16le";
    }
    if (arr_acodec[_CODEC][_AUDIO_CODEC] == "PCM24") {
        acodec = "-c:a pcm_s24le";
    }
    if (arr_acodec[_CODEC][_AUDIO_CODEC] == "PCM32") {
        acodec = "-c:a pcm_s32le";
    }
    if (arr_acodec[_CODEC][_AUDIO_CODEC] == "Source") {
        acodec = "-c:a copy";
    }

    QString colorprim = "";                         // color primaries
    if (_hdr[CUR_COLOR_PRIMARY] == "BT709") {
        colorprim = "-color_primaries bt709 ";
    }
    if (_hdr[CUR_COLOR_PRIMARY] == "BT2020") {
        colorprim = "-color_primaries bt2020 ";
    }
    if (_hdr[CUR_COLOR_PRIMARY] == "BT601 NTSC") {
        colorprim = "-color_primaries smpte170m ";
    }
    if (_hdr[CUR_COLOR_PRIMARY] == "BT601 PAL") {
        colorprim = "-color_primaries bt470bg ";
    }
    if (_hdr[CUR_COLOR_PRIMARY] == "BT470 System M") {
        colorprim = "-color_primaries bt470m ";
    }
    if (_hdr[CUR_COLOR_PRIMARY] == "SMPTE 240M") {
        colorprim = "-color_primaries smpte240m ";
    }
    if (_hdr[CUR_COLOR_PRIMARY] == "Generic film") {
        colorprim = "-color_primaries film ";
    }
    if (_hdr[CUR_COLOR_PRIMARY] == "DCI P3") {
        colorprim = "-color_primaries smpte431 ";
    }
    if (_hdr[CUR_COLOR_PRIMARY] == "XYZ") {
        colorprim = "-color_primaries smpte428 ";
    }
    if (_hdr[CUR_COLOR_PRIMARY] == "Display P3") {
        colorprim = "-color_primaries smpte432 ";
    }

    QString colormatrix = "";                       // color matrix
    if (_hdr[CUR_COLOR_MATRIX] == "BT709") {
        colormatrix = "-colorspace bt709 ";
    }
    if (_hdr[CUR_COLOR_MATRIX] == "BT2020nc") {
        colormatrix = "-colorspace bt2020nc ";
    }
    if (_hdr[CUR_COLOR_MATRIX] == "BT2020c") {
        colormatrix = "-colorspace bt2020c ";
    }
    if (_hdr[CUR_COLOR_MATRIX] == "FCC73682") {
        colormatrix = "-colorspace fcc ";
    }
    if (_hdr[CUR_COLOR_MATRIX] == "BT470SystemB/G") {
        colormatrix = "-colorspace bt470bg ";
    }
    if (_hdr[CUR_COLOR_MATRIX] == "SMPTE240M") {
        colormatrix = "-colorspace smpte240m ";
    }
    if (_hdr[CUR_COLOR_MATRIX] == "YCgCo") {
        colormatrix = "-colorspace YCgCo ";
    }
    if (_hdr[CUR_COLOR_MATRIX] == "Y'D'zD'x") {
        colormatrix = "-colorspace smpte2085 ";
    }
    if (_hdr[CUR_COLOR_MATRIX] == "Chromaticity-derivednc") {
        colormatrix = "-colorspace chroma-derived-nc ";
    }
    if (_hdr[CUR_COLOR_MATRIX] == "Chromaticity-derivedc") {
        colormatrix = "-colorspace chroma-derived-c ";
    }
    if (_hdr[CUR_COLOR_MATRIX] == "ICtCp") {
        colormatrix = "-colorspace ICtCp ";
    }
    if (_hdr[CUR_COLOR_MATRIX] == "BT601") {
        colormatrix = "-colorspace smpte170m ";
    }
    if (_hdr[CUR_COLOR_MATRIX] == "Identity") {
        colormatrix = "-colorspace GBR ";
    }

    QString transfer = "";                          // transfer characteristics
    if (_hdr[CUR_TRANSFER] == "BT709") {
        transfer = "-color_trc bt709 ";
    }
    if (_hdr[CUR_TRANSFER] == "PQ") {
        transfer = "-color_trc smpte2084 ";
    }
    if (_hdr[CUR_TRANSFER] == "HLG") {
        transfer = "-color_trc arib-std-b67 ";
    }
    if (_hdr[CUR_TRANSFER] == "BT2020 (10-bit)") {
        transfer = "-color_trc bt2020-10 ";
    }
    if (_hdr[CUR_TRANSFER] == "BT2020 (12-bit)") {
        transfer = "-color_trc bt2020-12 ";
    }
    if (_hdr[CUR_TRANSFER] == "BT470 System M") {
        transfer = "-color_trc bt470m ";
    }
    if (_hdr[CUR_TRANSFER] == "BT470 System B/G") {
        transfer = "-color_trc bt470bg ";
    }
    if (_hdr[CUR_TRANSFER] == "SMPTE 240M") {
        transfer = "-color_trc smpte240m ";
    }
    if (_hdr[CUR_TRANSFER] == "Linear") {
        transfer = "-color_trc linear ";
    }
    if (_hdr[CUR_TRANSFER] == "Logarithmic (100:1)") {
        transfer = "-color_trc log100 ";
    }
    if (_hdr[CUR_TRANSFER] == "Logarithmic (31622777:1)") {
        transfer = "-color_trc log316 ";
    }
    if (_hdr[CUR_TRANSFER] == "xvYCC") {
        transfer = "-color_trc iec61966-2-4 ";
    }
    if (_hdr[CUR_TRANSFER] == "BT1361") {
        transfer = "-color_trc bt1361e ";
    }
    if (_hdr[CUR_TRANSFER] == "sRGB/sYCC") {
        transfer = "-color_trc iec61966-2-1 ";
    }
    if (_hdr[CUR_TRANSFER] == "SMPTE 428M") {
        transfer = "-color_trc smpte428 ";
    }
    if (_hdr[CUR_TRANSFER] == "BT601") {
        transfer = "-color_trc smpte170m ";
    }

    QString color_range = "";
    QString max_lum = "";
    QString min_lum = "";
    QString max_cll = "";
    QString max_fall = "";
    QString chroma_coord = "";
    QString white_coord = "";
    if (_flag_hdr == true) {
        if (_COLOR_RANGE == 0) {                             // color range
            if (_hdr[CUR_COLOR_RANGE] == "Limited") {
                color_range = "-color_range tv ";
            }
            if (_hdr[CUR_COLOR_RANGE] == "Full") {
                color_range = "-color_range pc ";
            }
        }
        if (_COLOR_RANGE == 1) {
            color_range = "-color_range pc ";
        }
        if (_COLOR_RANGE == 2) {
            color_range = "-color_range tv ";
        }

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

    _preset_0 = "-hide_banner" + hwaccel + _splitStartParam;
    _preset_pass1 = _splitParam + resize  + codec + preset + mode + pass1 + color_range + colorprim + colormatrix + transfer + "-an -f null /dev/null";
    _preset = _splitParam + resize + codec + preset + mode + pass + color_range + colorprim + colormatrix + transfer + acodec;
    _preset_mkvmerge = QString("%1%2%3%4%5%6 ").arg(max_cll, max_fall, max_lum, min_lum, chroma_coord, white_coord);
    std::cout << "Flag two-pass: " << _flag_two_pass << std::endl;
    std::cout << "Flag HDR: " << _flag_hdr << std::endl;
    std::cout << "preset_0: " << _preset_0.toStdString() << std::endl;
    if ((_flag_two_pass == true) && (_flag_hdr == true)) {
        std::cout << "preset_pass1: " << _preset_pass1.toStdString() << std::endl;
        std::cout << "preset: " << _preset.toStdString() << std::endl;
        std::cout << "preset_mkvpropedit: " << _preset_mkvmerge.toStdString() << std::endl;
    }
    if ((_flag_two_pass == true) && (_flag_hdr == false)) {
        std::cout << "preset_pass1: " << _preset_pass1.toStdString() << std::endl;
        std::cout << "preset: " << _preset.toStdString() << std::endl;
    }
    if ((_flag_two_pass == false) && (_flag_hdr == true)) {
        std::cout << "preset: " << _preset.toStdString() << std::endl;
        std::cout << "preset_mkvpropedit: " << _preset_mkvmerge.toStdString() << std::endl;
    }
    if ((_flag_two_pass == false) && (_flag_hdr == false)) {
        std::cout << "preset: " << _preset.toStdString() << std::endl;
    }
    encode();
}

void MainWindow::encode()   //***************************************** Encode **********************//
{
    std::cout << "Encode ..." << std::endl;  //  Debug info //
    QStringList arguments;
    _calling_pr_1 = true;
    process_1->disconnect();
    connect(process_1, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_1()));
    connect(process_1, SIGNAL(finished(int)), this, SLOT(error_1()));
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
            QIcon icon_start;
            icon_start.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-play.png"), QSize(), QIcon::Normal, QIcon::Off);
            ui->actionEncode->setIcon(icon_start);
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
        select_preset->setEnabled(false);
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
        ui->actionPreset->setEnabled(false);
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

void MainWindow::add_metadata() //********************************** Add metedata *******************//
{
    std::cout << "Add metadata ..." << std::endl;  //  Debug info //
    _calling_pr_1 = true;
    process_1->disconnect();
    connect(process_1, SIGNAL(readyReadStandardOutput()), this, SLOT(progress_2()));
    connect(process_1, SIGNAL(finished(int)), this, SLOT(error_1()));
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

void MainWindow::complete() //**************************************** Complete *********************//
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

void MainWindow::progress_1()   //*********************************** Progress 1 ********************//
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
             disconnect(process_1, SIGNAL(finished(int)), this, SLOT(error_1()));
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

void MainWindow::progress_2()   //*********************************** Progress 2 ********************//
{
    QString line = process_1->readAllStandardOutput();
    int pos_st = line.indexOf("Done.");
    int pos_nf = line.indexOf("Nothing to do.");
    if ((pos_st != -1) or (pos_nf != -1)) {
        int percent = 100;
        ui->progressBar->setValue(percent);
        if ((percent == 100) && (_calling_pr_1 == true)) {
            disconnect(process_1, SIGNAL(finished(int)), this, SLOT(error_1()));
            _mux_mode = true;
            _loop_start = time(nullptr);
            _calling_pr_1 = false;
            connect(process_1, SIGNAL(finished(int)), this, SLOT(encode()));
        };
    };
}

void MainWindow::on_actionPreset_clicked()  // ****************** Call Preset Window *************** //
{
    SelectPreset select_preset(this);
    select_preset.setParameters(&_pos_top, &_pos_cld);
    select_preset.setModal(true);
    select_preset.exec();  // ************************ Call preset window and wait for return ******** //
    if (_row != -1) {
        get_output_filename();
    }
}

void MainWindow::on_actionEncode_clicked()  // ********************* Encode button ***************** //
{
    if (_status_encode_btn == "start") {
        std::cout << "Status encode btn: start" << std::endl;  // Debug info //
        int cnt = ui->tableWidget->rowCount();
        if (cnt == 0) {
            _message = "Select input file first!";
            call_task_complete(_message, false);
            return;
        }
        if (_cur_param[PRESET_NAME] == "") {
            _message = "Select preset first!";
            call_task_complete(_message, false);
            return;
        }
        _status_encode_btn = "pause";
        QIcon icon_pause;
        icon_pause.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-pause.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->actionEncode->setIcon(icon_pause);
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
        QIcon icon_resume;
        icon_resume.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-forward.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->actionEncode->setIcon(icon_resume);
        ui->actionEncode->setToolTip("Resume");
        return;
    }
    if (_status_encode_btn == "resume") {
        std::cout << "Status encode btn: resume" << std::endl;  // Debug info //
        resume();
        _status_encode_btn = "pause";
        QIcon icon_pause;
        icon_pause.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-pause.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->actionEncode->setIcon(icon_pause);
        ui->actionEncode->setToolTip("Pause");
        return;
    }
}

void MainWindow::pause()    //***************************************** Pause ***********************//
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

void MainWindow::resume()   //***************************************** Resume **********************//
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

void MainWindow::on_actionAbout_clicked()   //************************* About ***********************//
{
    About about(this);
    about.setModal(true);
    about.setParameters();
    about.exec();
}

void MainWindow::on_actionDonate_clicked()   //************************ Donate **********************//
{
    Donate donate(this);
    donate.setModal(true);
    donate.setParameters();
    donate.exec();
}

void MainWindow::on_actionSettings_clicked()    //******************** Settings *********************//
{
    Settings settings(this);
    settings.setParameters(&_batch_mode, &_stn_file, &_output_folder, &_temp_folder, &_protection, &_showHDR_mode, &_timer_interval, &_theme);
    settings.setModal(true);
    settings.exec();
    timer->setInterval(_timer_interval*1000);
    setTheme(_theme);
    if (_row != -1) {
        get_output_filename();
    }
}

void MainWindow::on_actionStop_clicked()    //************************** Stop ***********************//
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

void MainWindow::cancel()   //************************************** Stop execute *******************//
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

void MainWindow::error_1()  //***************************************** Error ***********************//
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

void MainWindow::get_current_data() //**************************** Get current data *****************//
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

    for (int p = 0; p < AMOUNT_AUDIO_SRTEAMS; p++) {
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
    while (countAudioStreams < AMOUNT_AUDIO_SRTEAMS) {
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

    for (int q = 0; q < AMOUNT_AUDIO_SRTEAMS; q++) {
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

void MainWindow::get_output_filename()  //************************ Get output data ******************//
{
    ui->textBrowser_2->clear();
    ui->textBrowser_2->setText(_cur_param[PRESET_NAME]);
    QString file_without_ext("");
    QString prefix("");
    QString suffix("");
    QString arr[20][5] = {
        {"MKV",  "MOV", "MP4", "",     ""},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
        {"MKV",  "MOV", "MP4", "M2TS", "TS"},
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
        {"MKV",  "MOV", "MP4", "M2TS", "TS"}
    };
    prefix = arr[_cur_param[CODEC].toInt()][_cur_param[CONTAINER].toInt()].toLower();
    //QTime ct = QTime::currentTime();
    //QString ct_qstr = ct.toString();
    suffix = QString("_encoded_00") + QString::number(_row) + QString(".");
    std::wstring file_name_wstr = _curFilename.toStdWString();
    std::wstring::size_type separator = file_name_wstr.rfind('.');
    if (separator != std::wstring::npos) {
        file_without_ext = QString::fromStdWString(file_name_wstr.substr(0, separator));
    } else {
        file_without_ext = _curFilename;
    }
    QString _output_file_name = file_without_ext + suffix + prefix;
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

void MainWindow::set_defaults() //****************************** Set default presets ****************//
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
    for (int k = 0; k < 23; k++) {
        _cur_param[k] = cur_param[k];
    }
    _pos_top = cur_param[23].toInt();
    _pos_cld = cur_param[24].toInt();
    int n = line.size() - 1;
    for (int i = 0; i < 24; i++) {
      _preset_table[i].resize(n);
    }
    for (int j = 1; j <= n; j++) {
        cur_param = line[j].split("<&>");
        for (int m = 0; m < 24; m++) {
            _preset_table[m][j-1] = cur_param[m];
        }
    }
}

void MainWindow::restore_initial_state()    //***************** Restore initial state ***************//
{
    animation->stop();
    add_files->setEnabled(true);
    remove_files->setEnabled(true);
    select_preset->setEnabled(true);
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
    ui->actionPreset->setEnabled(true);
    ui->actionSettings->setEnabled(true);
    _status_encode_btn = "start";
    QIcon icon_start;
    icon_start.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-play.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->actionEncode->setIcon(icon_start);
    ui->actionEncode->setToolTip("Encode");
}

bool MainWindow::call_dialog(const QString &_message)  // Call dialog ******//
{
    bool acceptFlag = false;
    Dialog dialog(this);
    dialog.setMessage(_message, &acceptFlag);
    dialog.setModal(true);
    dialog.exec();
    return acceptFlag;
}

void MainWindow::call_task_complete(const QString &_message, const bool &_timer_mode)  // Call task complete ******//
{
    Taskcomplete taskcomplete(this);
    taskcomplete.setMessage(_message, _timer_mode);
    taskcomplete.setModal(true);
    taskcomplete.exec();
}

void MainWindow::repeat_handler()   //************* Repeat handler ***************//
{
    std::cout<< "Call by timer..." << std::endl;
    on_actionEncode_clicked();
    call_task_complete("Pause", true);
    on_actionEncode_clicked();
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
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
            return QMainWindow::eventFilter(watched, event);
        }
    }

    if (watched == ui->centralwidget) // *************** Resize window realisation ************************* //
    {
        if (!this->isMaximized())
        {
            if (event->type() == QEvent::MouseButtonPress)
            {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->button() == Qt::LeftButton)
                {
                    curWidth = this->width();
                    curHeight = this->height();
                    mouseClickCoordinate = mouse_event->pos();
                    if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() > 62) && (mouseClickCoordinate.y() < (curHeight-12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                        clickPressed_Left_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() < 12))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                        clickPressed_Left_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > 12) && (mouseClickCoordinate.x() < (curWidth - 120)) && (mouseClickCoordinate.y() < 3))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                        clickPressed_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (curWidth - 12)) && (mouseClickCoordinate.y() < 12))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                        clickPressed_Right_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (curWidth - 12)) && (mouseClickCoordinate.y() > 62) && (mouseClickCoordinate.y() < (curHeight-12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                        clickPressed_Right_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (curWidth - 12)) && (mouseClickCoordinate.y() > (curHeight - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                        clickPressed_Right_Bottom_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > 12) && (mouseClickCoordinate.x() < (curWidth - 12)) && (mouseClickCoordinate.y() > (curHeight - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                        clickPressed_Bottom_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() > (curHeight - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                        clickPressed_Left_Bottom_ResizeFlag = true;
                    }
                    return QMainWindow::eventFilter(watched, event);
                }
            }
            else if (event->type() == QEvent::MouseMove)
            {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->buttons() & Qt::LeftButton)
                {
                    int deltaX = mouse_event->globalPos().x() - mouseClickCoordinate.x();
                    int deltaY = mouse_event->globalPos().y() - mouseClickCoordinate.y();
                    int deltaWidth = static_cast<int>(mouse_event->localPos().x()) - mouseClickCoordinate.x();
                    int deltaHeight = static_cast<int>(mouse_event->localPos().y()) - mouseClickCoordinate.y();
                    if (clickPressed_Left_ResizeFlag == true)
                    {
                        this->setGeometry(deltaX, this->pos().y(), this->width() - deltaWidth, curHeight);
                    }
                    else if (clickPressed_Left_Top_ResizeFlag == true)
                    {
                        this->setGeometry(deltaX, deltaY, this->width() - deltaWidth, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Top_ResizeFlag == true)
                    {
                        this->setGeometry(this->pos().x(), deltaY, curWidth, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Right_Top_ResizeFlag == true)
                    {
                        this->setGeometry(this->pos().x(), deltaY, curWidth + deltaWidth, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Right_ResizeFlag == true)
                    {
                        this->setGeometry(this->pos().x(), this->pos().y(), curWidth + deltaWidth, curHeight);
                    }
                    else if (clickPressed_Right_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(this->pos().x(), this->pos().y(), curWidth + deltaWidth, curHeight + deltaHeight);
                    }
                    else if (clickPressed_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(this->pos().x(), this->pos().y(), curWidth, curHeight + deltaHeight);
                    }
                    else if (clickPressed_Left_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(deltaX, this->pos().y(), this->width() - deltaWidth, curHeight + deltaHeight);
                    }
                    return QMainWindow::eventFilter(watched, event);
                }
            }
        }
    }
    else if (watched == ui->frame_hint) // *************** Drag window realisation ************************* //
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton)
            {
                mouseClickCoordinate = mouse_event->pos();
                clickPressedFlag = true;
                return QMainWindow::eventFilter(watched, event);
            }
        }
        else if ((event->type() == QEvent::MouseMove) && clickPressedFlag == true)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                if (this->isMaximized())
                {
                    on_expandWindow_clicked();
                }
                this->move(mouse_event->globalPos() - mouseClickCoordinate);
                return QMainWindow::eventFilter(watched, event);
            }
        }
        else if (event->type() == QEvent::MouseButtonDblClick)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                on_expandWindow_clicked();
                return QMainWindow::eventFilter(watched, event);
            }
        }
    }
    else if (watched == raiseThumb) // ************** Click thumb realisation ************** //
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton)
            {
                on_actionAdd_clicked();
                return QMainWindow::eventFilter(watched, event);
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) //************* Drag & Drop *************//
{
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event)
{
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent* event)
{
    event->accept();
}

void MainWindow::dropEvent(QDropEvent* event)
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

void MainWindow::setTheme(int &ind_theme)   //*********** Set theme ***********//
{
    QFile file;
    switch (ind_theme)
    {
        case 0: {
            file.setFileName(":/resources/css/style_0.css");
        }; break;
        case 1: {
            file.setFileName(":/resources/css/style_1.css");
        }; break;
        case 2: {
            file.setFileName(":/resources/css/style_2.css");
        }; break;
        case 3: {
            file.setFileName(":/resources/css/style_3.css");
        }; break;
    }
    file.open(QFile::ReadOnly);
    QString list = QString::fromUtf8(file.readAll());
    qApp->setStyleSheet(styleCreator(list));
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
}

QString MainWindow::styleCreator(const QString &list)
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

void MainWindow::on_buttonSortDown_clicked()
{
    ui->tableWidget->sortByColumn(columnIndex::FILENAME, Qt::DescendingOrder);
}

void MainWindow::on_buttonSortUp_clicked()
{
    ui->tableWidget->sortByColumn(columnIndex::FILENAME, Qt::AscendingOrder);
}

void MainWindow::on_buttonRightWindow_clicked()
{
    if (ui->frameRight->isHidden()) {
        ui->frameRight->show();
        QIcon icon_right;
        icon_right.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-arrow-right-.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->buttonRightWindow->setIcon(icon_right);
    } else {
        ui->frameRight->hide();
        QIcon icon_left;
        icon_left.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-arrow-left-.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->buttonRightWindow->setIcon(icon_left);
    }
}

void MainWindow::on_buttonLeftWindow_clicked()
{
    if (ui->frameLeft->isHidden()) {
        ui->frameLeft->show();
        QIcon icon_left;
        icon_left.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-arrow-left-.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->buttonLeftWindow->setIcon(icon_left);
    } else {
        ui->frameLeft->hide();
        QIcon icon_right;
        icon_right.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-arrow-right-.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui->buttonLeftWindow->setIcon(icon_right);
    }
}

void MainWindow::on_buttonCloseRightWindow_clicked()
{
    ui->frameRight->hide();
    QIcon icon_left;
    icon_left.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-arrow-left-.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->buttonRightWindow->setIcon(icon_left);
}

void MainWindow::on_buttonCloseLeftWindow_clicked()
{
    ui->frameLeft->hide();
    QIcon icon_right;
    icon_right.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-arrow-right-.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->buttonLeftWindow->setIcon(icon_right);
}

void MainWindow::on_lineEditTitleVideo_editingFinished()
{
    if (_row != -1) {
        QString videoTitle = ui->lineEditTitleVideo->text();
        QTableWidgetItem *newItem_videoTitle = new QTableWidgetItem(videoTitle);
        ui->tableWidget->setItem(_row, columnIndex::T_VIDEOTITLE, newItem_videoTitle);
        _videoMetadata[VIDEO_TITLE] = ui->tableWidget->item(_row, columnIndex::T_VIDEOTITLE)->text();
    }
}

void MainWindow::on_lineEditAuthorVideo_editingFinished()
{
    if (_row != -1) {
        QString videoAuthor = ui->lineEditAuthorVideo->text();
        QTableWidgetItem *newItem_videoAuthor = new QTableWidgetItem(videoAuthor);
        ui->tableWidget->setItem(_row, columnIndex::T_VIDEOAUTHOR, newItem_videoAuthor);
        _videoMetadata[VIDEO_AUTHOR] = ui->tableWidget->item(_row, columnIndex::T_VIDEOAUTHOR)->text();
    }
}

void MainWindow::on_lineEditYearVideo_editingFinished()
{
    if (_row != -1) {
        QString videoYear = ui->lineEditYearVideo->text();
        QTableWidgetItem *newItem_videoYear = new QTableWidgetItem(videoYear);
        ui->tableWidget->setItem(_row, columnIndex::T_VIDEOYEAR, newItem_videoYear);
        _videoMetadata[VIDEO_YEAR] = ui->tableWidget->item(_row, columnIndex::T_VIDEOYEAR)->text();
    }
}

void MainWindow::on_lineEditPerfVideo_editingFinished()
{
    if (_row != -1) {
        QString videoPerf = ui->lineEditPerfVideo->text();
        QTableWidgetItem *newItem_videoPerf = new QTableWidgetItem(videoPerf);
        ui->tableWidget->setItem(_row, columnIndex::T_VIDEOPERF, newItem_videoPerf);
        _videoMetadata[VIDEO_PERFORMER] = ui->tableWidget->item(_row, columnIndex::T_VIDEOPERF)->text();
    }
}

void MainWindow::on_lineEditMovieNameVideo_editingFinished()
{
    if (_row != -1) {
        QString videoMovieName = ui->lineEditMovieNameVideo->text();
        QTableWidgetItem *newItem_videoMovieName = new QTableWidgetItem(videoMovieName);
        ui->tableWidget->setItem(_row, columnIndex::T_VIDEOMOVIENAME, newItem_videoMovieName);
        _videoMetadata[VIDEO_MOVIENAME] = ui->tableWidget->item(_row, columnIndex::T_VIDEOMOVIENAME)->text();
    }
}

void MainWindow::on_lineEditDescriptionVideo_editingFinished()
{
    if (_row != -1) {
        QString videoDescription = ui->lineEditDescriptionVideo->text();
        QTableWidgetItem *newItem_videoDescription = new QTableWidgetItem(videoDescription);
        ui->tableWidget->setItem(_row, columnIndex::T_VIDEODESCR, newItem_videoDescription);
        _videoMetadata[VIDEO_DESCRIPTION] = ui->tableWidget->item(_row, columnIndex::T_VIDEODESCR)->text();
    }
}

void MainWindow::on_checkBoxAudio_1_clicked()
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

void MainWindow::on_checkBoxAudio_2_clicked()
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

void MainWindow::on_checkBoxAudio_3_clicked()
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

void MainWindow::on_checkBoxAudio_4_clicked()
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

void MainWindow::on_checkBoxAudio_5_clicked()
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

void MainWindow::on_checkBoxAudio_6_clicked()
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

void MainWindow::on_checkBoxAudio_7_clicked()
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

void MainWindow::on_checkBoxAudio_8_clicked()
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

void MainWindow::on_checkBoxAudio_9_clicked()
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

void MainWindow::on_lineEditLangAudio_1_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_1->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_1, newItem_langAudio);
        _audioLang[0] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_1)->text();
    }
}

void MainWindow::on_lineEditLangAudio_2_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_2->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_2, newItem_langAudio);
        _audioLang[1] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_2)->text();
    }
}

void MainWindow::on_lineEditLangAudio_3_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_3->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_3, newItem_langAudio);
        _audioLang[2] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_3)->text();
    }
}

void MainWindow::on_lineEditLangAudio_4_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_4->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_4, newItem_langAudio);
        _audioLang[3] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_4)->text();
    }
}

void MainWindow::on_lineEditLangAudio_5_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_5->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_5, newItem_langAudio);
        _audioLang[4] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_5)->text();
    }
}

void MainWindow::on_lineEditLangAudio_6_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_6->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_6, newItem_langAudio);
        _audioLang[5] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_6)->text();
    }
}

void MainWindow::on_lineEditLangAudio_7_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_7->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_7, newItem_langAudio);
        _audioLang[6] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_7)->text();
    }
}

void MainWindow::on_lineEditLangAudio_8_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_8->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_8, newItem_langAudio);
        _audioLang[7] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_8)->text();
    }
}

void MainWindow::on_lineEditLangAudio_9_editingFinished()
{
    if (_row != -1) {
        QString langAudio = ui->lineEditLangAudio_9->text();
        QTableWidgetItem *newItem_langAudio = new QTableWidgetItem(langAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOLANG_9, newItem_langAudio);
        _audioLang[8] = ui->tableWidget->item(_row, columnIndex::T_AUDIOLANG_9)->text();
    }
}

void MainWindow::on_lineEditTitleAudio_1_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_1->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_1, newItem_titleAudio);
        _audioTitle[0] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_1)->text();
    }
}

void MainWindow::on_lineEditTitleAudio_2_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_2->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_2, newItem_titleAudio);
        _audioTitle[1] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_2)->text();
    }
}

void MainWindow::on_lineEditTitleAudio_3_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_3->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_3, newItem_titleAudio);
        _audioTitle[2] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_3)->text();
    }
}

void MainWindow::on_lineEditTitleAudio_4_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_4->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_4, newItem_titleAudio);
        _audioTitle[3] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_4)->text();
    }
}

void MainWindow::on_lineEditTitleAudio_5_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_5->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_5, newItem_titleAudio);
        _audioTitle[4] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_5)->text();
    }
}

void MainWindow::on_lineEditTitleAudio_6_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_6->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_6, newItem_titleAudio);
        _audioTitle[5] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_6)->text();
    }
}

void MainWindow::on_lineEditTitleAudio_7_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_7->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_7, newItem_titleAudio);
        _audioTitle[6] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_7)->text();
    }
}

void MainWindow::on_lineEditTitleAudio_8_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_8->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_8, newItem_titleAudio);
        _audioTitle[7] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_8)->text();
    }
}

void MainWindow::on_lineEditTitleAudio_9_editingFinished()
{
    if (_row != -1) {
        QString titleAudio = ui->lineEditTitleAudio_9->text();
        QTableWidgetItem *newItem_titleAudio = new QTableWidgetItem(titleAudio);
        ui->tableWidget->setItem(_row, columnIndex::T_AUDIOTITLE_9, newItem_titleAudio);
        _audioTitle[8] = ui->tableWidget->item(_row, columnIndex::T_AUDIOTITLE_9)->text();
    }
}

void MainWindow::on_checkBoxSubtitle_1_clicked()
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

void MainWindow::on_checkBoxSubtitle_2_clicked()
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

void MainWindow::on_checkBoxSubtitle_3_clicked()
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

void MainWindow::on_checkBoxSubtitle_4_clicked()
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

void MainWindow::on_checkBoxSubtitle_5_clicked()
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

void MainWindow::on_checkBoxSubtitle_6_clicked()
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

void MainWindow::on_checkBoxSubtitle_7_clicked()
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

void MainWindow::on_checkBoxSubtitle_8_clicked()
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

void MainWindow::on_checkBoxSubtitle_9_clicked()
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

void MainWindow::on_lineEditLangSubtitle_1_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_1->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_1, newItem_langSubtitle);
        _subtitleLang[0] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_1)->text();
    }
}

void MainWindow::on_lineEditLangSubtitle_2_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_2->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_2, newItem_langSubtitle);
        _subtitleLang[1] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_2)->text();
    }
}

void MainWindow::on_lineEditLangSubtitle_3_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_3->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_3, newItem_langSubtitle);
        _subtitleLang[2] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_3)->text();
    }
}

void MainWindow::on_lineEditLangSubtitle_4_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_4->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_4, newItem_langSubtitle);
        _subtitleLang[3] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_4)->text();
    }
}

void MainWindow::on_lineEditLangSubtitle_5_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_5->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_5, newItem_langSubtitle);
        _subtitleLang[4] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_5)->text();
    }
}

void MainWindow::on_lineEditLangSubtitle_6_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_6->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_6, newItem_langSubtitle);
        _subtitleLang[5] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_6)->text();
    }
}

void MainWindow::on_lineEditLangSubtitle_7_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_7->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_7, newItem_langSubtitle);
        _subtitleLang[6] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_7)->text();
    }
}

void MainWindow::on_lineEditLangSubtitle_8_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_8->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_8, newItem_langSubtitle);
        _subtitleLang[7] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_8)->text();
    }
}

void MainWindow::on_lineEditLangSubtitle_9_editingFinished()
{
    if (_row != -1) {
        QString langSubtitle = ui->lineEditLangSubtitle_9->text();
        QTableWidgetItem *newItem_langSubtitle = new QTableWidgetItem(langSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_SUBLANG_9, newItem_langSubtitle);
        _subtitleLang[8] = ui->tableWidget->item(_row, columnIndex::T_SUBLANG_9)->text();
    }
}

void MainWindow::on_lineEditTitleSubtitle_1_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_1->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_1, newItem_titleSubtitle);
        _subtitleTitle[0] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_1)->text();
    }
}

void MainWindow::on_lineEditTitleSubtitle_2_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_2->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_2, newItem_titleSubtitle);
        _subtitleTitle[1] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_2)->text();
    }
}

void MainWindow::on_lineEditTitleSubtitle_3_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_3->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_3, newItem_titleSubtitle);
        _subtitleTitle[2] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_3)->text();
    }
}

void MainWindow::on_lineEditTitleSubtitle_4_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_4->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_4, newItem_titleSubtitle);
        _subtitleTitle[3] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_4)->text();
    }
}

void MainWindow::on_lineEditTitleSubtitle_5_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_5->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_5, newItem_titleSubtitle);
        _subtitleTitle[4] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_5)->text();
    }
}

void MainWindow::on_lineEditTitleSubtitle_6_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_6->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_6, newItem_titleSubtitle);
        _subtitleTitle[5] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_6)->text();
    }
}

void MainWindow::on_lineEditTitleSubtitle_7_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_7->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_7, newItem_titleSubtitle);
        _subtitleTitle[6] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_7)->text();
    }
}

void MainWindow::on_lineEditTitleSubtitle_8_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_8->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_8, newItem_titleSubtitle);
        _subtitleTitle[7] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_8)->text();
    }
}

void MainWindow::on_lineEditTitleSubtitle_9_editingFinished()
{
    if (_row != -1) {
        QString titleSubtitle = ui->lineEditTitleSubtitle_9->text();
        QTableWidgetItem *newItem_titleSubtitle = new QTableWidgetItem(titleSubtitle);
        ui->tableWidget->setItem(_row, columnIndex::T_TITLESUB_9, newItem_titleSubtitle);
        _subtitleTitle[8] = ui->tableWidget->item(_row, columnIndex::T_TITLESUB_9)->text();
    }
}

void MainWindow::on_horizontalSlider_valueChanged(int value)
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

void MainWindow::on_buttonFramePrevious_clicked()
{
    int value = ui->horizontalSlider->value();
    if (value > 0) {
        ui->horizontalSlider->setValue(value - 1);
    }
}

void MainWindow::on_buttonFrameNext_clicked()
{
    int value = ui->horizontalSlider->value();
    if (value < _fr_count) {
        ui->horizontalSlider->setValue(value + 1);
    }
}

void MainWindow::on_buttonSetStartTime_clicked()
{
    _startTime = _curTime;
    if (_startTime > _endTime && _endTime != 0)
    {
        _startTime = _endTime;
    }
    ui->lineEditStartTime->setText(timeConverter(_startTime));
    QTableWidgetItem *newItem_startTime = new QTableWidgetItem(QString::number(_startTime, 'f', 3));
    ui->tableWidget->setItem(_row, columnIndex::T_STARTTIME, newItem_startTime);
}

void MainWindow::on_buttonSetEndTime_clicked()
{
    _endTime = _curTime;
    if (_endTime < _startTime)
    {
        _endTime =_startTime;
    }
    ui->lineEditEndTime->setText(timeConverter(_endTime));
    QTableWidgetItem *newItem_endTime = new QTableWidgetItem(QString::number(_endTime, 'f', 3));
    ui->tableWidget->setItem(_row, columnIndex::T_ENDTIME, newItem_endTime);
}

void MainWindow::showMetadataEditor()
{
    if (ui->frameRight->isHidden()) {
        on_buttonRightWindow_clicked();
    }
    ui->tabWidgetRight->setCurrentIndex(0);
}

void MainWindow::showAudioStreamsSelection()
{
    if (ui->frameRight->isHidden()) {
        on_buttonRightWindow_clicked();
    }
    ui->tabWidgetRight->setCurrentIndex(1);
}

void MainWindow::showSubtitlesSelection()
{
    if (ui->frameRight->isHidden()) {
        on_buttonRightWindow_clicked();
    }
    ui->tabWidgetRight->setCurrentIndex(2);
}

void MainWindow::showSplitVideo()
{
    if (ui->frameRight->isHidden()) {
        on_buttonRightWindow_clicked();
    }
    ui->tabWidgetRight->setCurrentIndex(3);
}

QString MainWindow::timeConverter(double &time)
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

QString MainWindow::timeConverter(float &time)
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

void MainWindow::setThumbnail(QString curFilename, double time, QString quality)
{
    QString qualityParam = "-vf scale=144:-1";
    if (quality == "low")
    {
        qualityParam = "-vf scale=144:-1,format=pal8,dctdnoiz=4.5";
    }
    const QString time_qstr = QString::number(time, 'f', 3);
    const QString tmb_name = curFilename.replace(".", "_").replace(" ", "_") + time_qstr;
    const QString tmb_file = _thumb_path + QString("/") + tmb_name + QString(".png");
    QFile tmb;
    tmb.setFileName(tmb_file);
    if (!tmb.exists()) {
        std::cout<< "Thumbnail file not exist and created..." << std::endl;
        QStringList cmd;
        cmd << "-hide_banner" << "-ss" << time_qstr << "-i" << _input_file
            << qualityParam.split(" ") << "-vframes" << "1" << "-q:v" << "3" << "-y" << tmb_file;
        process_5->start("ffmpeg", cmd);
        process_5->waitForFinished();
    }
    ui->labelThumb->setPixmap(QPixmap(tmb_file));
}

void MainWindow::repeatHandler_Type_2()  //************* Repeat handler *****************//
{
    std::cout << "Call by timer... " << std::endl;
    if (_row != -1) {
        setThumbnail(_curFilename, _curTime, "high");
    }
}
