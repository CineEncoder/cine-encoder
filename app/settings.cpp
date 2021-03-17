#include "settings.h"
#include "ui_settings.h"
#include "taskcomplete.h"



Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui_settings(new Ui::Settings)
{
    ui_settings->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);
}

Settings::~Settings()
{
    delete ui_settings;
}

void Settings::on_closeWindow_clicked() // Close settings window
{
    this->close();
}

void Settings::on_pushButton_7_clicked() // Close settings window
{
    this->close();
}

void Settings::on_expandWindow_clicked()
{
    if (_expandWindowsState == false)
    {
        this->showMaximized();
        _expandWindowsState = true;
        QIcon iconShowMax;
        iconShowMax.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-clone.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui_settings->expandWindow->setIcon(iconShowMax);
    } else {
        this->showNormal();
        _expandWindowsState = false;
        QIcon iconShowNorm;
        iconShowNorm.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-media-stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui_settings->expandWindow->setIcon(iconShowNorm);
    }
}

void Settings::on_hideWindow_clicked()
{
    this->showMinimized();
}

void Settings::closeEvent(QCloseEvent *close_settings)  // Show prompt when close app
{
    close_settings->ignore();
    if (_flag_save == false)
    {
        *_ptr_output_folder = _curr_output_folder;
        *_ptr_temp_folder = _curr_temp_folder;
        *_ptr_batch_mode = _curr_batch_mode;
        *_ptr_showHDR_mode = _curr_showHDR_mode;
        *_ptr_protection = _curr_protection;
        *_ptr_timer_interval = _curr_timer_interval;
        *_ptr_theme = _curr_theme;
    };
    close_settings->accept();
}

void Settings::setParameters(bool *ptr_batch_mode, QFile *ptr_stn_file,
                             QString *ptr_output_folder, QString *ptr_temp_folder,
                             bool *ptr_protection, bool *ptr_showHDR_mode, int *ptr_timer_interval, int *ptr_theme)  // Set parameters
{
    ui_settings->frame_hint->installEventFilter(this);
    ui_settings->widget->installEventFilter(this);
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    _ptr_batch_mode = ptr_batch_mode;
    _ptr_showHDR_mode = ptr_showHDR_mode;
    _ptr_stn_file = ptr_stn_file;
    _ptr_output_folder = ptr_output_folder;
    _ptr_temp_folder = ptr_temp_folder;
    _ptr_protection = ptr_protection;
    _ptr_timer_interval = ptr_timer_interval;
    _ptr_theme = ptr_theme;

    ui_settings->lineEdit_9->setText(*_ptr_temp_folder);
    ui_settings->lineEdit_10->setText(*_ptr_output_folder);
    ui_settings->spinBox_3->setValue(*_ptr_timer_interval);
    if (*_ptr_batch_mode == true)
    {
        ui_settings->checkBox_1->setChecked(true);
    }
    if (*_ptr_showHDR_mode == true)
    {
        ui_settings->checkBox_2->setChecked(true);
    }
    if (*_ptr_protection == true)
    {
        ui_settings->checkBox_3->setChecked(true);
        ui_settings->spinBox_3->setEnabled(true);
    }
    ui_settings->comboBox_1->setCurrentIndex(*_ptr_theme);
    _curr_output_folder = *_ptr_output_folder;
    _curr_temp_folder = *_ptr_temp_folder;
    _curr_batch_mode = *_ptr_batch_mode;
    _curr_showHDR_mode = *_ptr_showHDR_mode;
    _curr_protection = *_ptr_protection;
    _curr_timer_interval = *_ptr_timer_interval;
    _curr_theme = *_ptr_theme;
    _flag_save = false;
}

void Settings::on_buttonReset_clicked() // Reset settings
{
    ui_settings->lineEdit_9->clear();
    ui_settings->lineEdit_10->clear();
    ui_settings->checkBox_1->setChecked(false);
    ui_settings->checkBox_2->setChecked(false);
    ui_settings->checkBox_3->setChecked(false);
    ui_settings->spinBox_3->setEnabled(false);
    ui_settings->comboBox_1->setCurrentIndex(0);
    *_ptr_temp_folder = "";
    *_ptr_output_folder = "";
    *_ptr_protection = false;
    *_ptr_showHDR_mode = false;
    *_ptr_batch_mode = false;
    *_ptr_theme = 0;
}

void Settings::on_pushButton_6_clicked() // Save settings
{
    if ((*_ptr_stn_file).open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QString line_0 = QString("temp_folder:") + *_ptr_temp_folder + QString("\n");
        (*_ptr_stn_file).write(line_0.toUtf8());
        QString line_1 = QString("output_folder:") + *_ptr_output_folder + QString("\n");
        (*_ptr_stn_file).write(line_1.toUtf8());
        if (*_ptr_batch_mode == true)
        {
            (*_ptr_stn_file).write("batch_mode:true\n");
        }
        else
        {
            (*_ptr_stn_file).write("batch_mode:false\n");
        }
        if (*_ptr_showHDR_mode == true)
        {
            (*_ptr_stn_file).write("show_hdr:true\n");
        }
        else
        {
            (*_ptr_stn_file).write("show_hdr:false\n");
        }
        if (*_ptr_protection == true)
        {
            (*_ptr_stn_file).write("protection:true\n");
        }
        else
        {
            (*_ptr_stn_file).write("protection:false\n");
        }
        *_ptr_timer_interval = ui_settings->spinBox_3->value();
        QString line_2 = QString("timer_interval:") + QString::number(*_ptr_timer_interval) + QString("\n");
        (*_ptr_stn_file).write(line_2.toUtf8());
        *_ptr_theme = ui_settings->comboBox_1->currentIndex();
        QString line_3 = QString("theme:") + QString::number(*_ptr_theme) + QString("\n");
        (*_ptr_stn_file).write(line_3.toUtf8());
        (*_ptr_stn_file).close();
        _flag_save = true;
        this->close();
    }
    else
    {
        _message = "Settings file not found!\n";
        call_task_complete(_message, false);
    };
}

void Settings::on_pushButton_5_clicked() // Select temp folder
{
    QString temp_folder_name = callFileDialog("Select temp folder");
    if (temp_folder_name.isEmpty()) {
        return;
    }
    ui_settings->lineEdit_9->setText(temp_folder_name);
    *_ptr_temp_folder = temp_folder_name;
}

void Settings::on_pushButton_4_clicked()  // Select output folder
{
    QString output_folder_name = callFileDialog("Select output folder");
    if (output_folder_name.isEmpty())
    {
        return;
    }
    ui_settings->lineEdit_10->setText(output_folder_name);
    *_ptr_output_folder = output_folder_name;
}

QString Settings::callFileDialog(const QString title)  // Call file dialog
{
    QFileDialog *selectFolderWindow = new QFileDialog(this);
    selectFolderWindow->setFileMode(QFileDialog::DirectoryOnly);
#ifdef Q_OS_WIN
    selectFolderWindow->setOptions(QFileDialog::ShowDirsOnly |
                                   QFileDialog::ReadOnly);
#else
    selectFolderWindow->setOptions(QFileDialog::ShowDirsOnly |
                                   QFileDialog::DontUseNativeDialog |
                                   QFileDialog::ReadOnly);
#endif
    selectFolderWindow->setStyleSheet("QWidget {color: rgb(10, 10, 10); background-color: "
                                      "rgb(120, 120, 120);} QHeaderView {color: rgb(10, 10, 10); "
                                      "background-color: transparent;} QHeaderView::section:horizontal "
                                      "{height: 20px; padding: 0px; border: 1px solid rgb(160, 160, 160); "
                                      "border-top-left-radius: 0px; border-top-right-radius: 0px; "
                                      "background-color: rgb(160, 160, 160);} QScrollBar {background-color: "
                                      "rgb(160, 160, 160);}");
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

void Settings::on_checkBox_1_clicked()  // Batch mode select
{
    int stts_1 = ui_settings->checkBox_1->checkState();
    if (stts_1 == 2)
    {
        *_ptr_batch_mode = true;
    }
    else
    {
        *_ptr_batch_mode = false;
    }
}

void Settings::on_checkBox_2_clicked()  // Show HDR info select
{
    int stts_2 = ui_settings->checkBox_2->checkState();
    if (stts_2 == 2)
    {
        *_ptr_showHDR_mode = true;
    }
    else
    {
        *_ptr_showHDR_mode = false;
    }
}

void Settings::on_checkBox_3_clicked()  // Protection mode select
{
    int stts_3 = ui_settings->checkBox_3->checkState();
    if (stts_3 == 2)
    {
        *_ptr_protection = true;
        ui_settings->spinBox_3->setEnabled(true);
    }
    else
    {
        *_ptr_protection = false;
        ui_settings->spinBox_3->setEnabled(false);
    };
}

void Settings::call_task_complete(const QString &_message, const bool &_timer_mode)   // Call task complete
{
    Taskcomplete taskcomplete(this);
    taskcomplete.setMessage(_message, _timer_mode);
    taskcomplete.setModal(true);
    taskcomplete.exec();
}

bool Settings::eventFilter(QObject *watched, QEvent *event)
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
            return QDialog::eventFilter(watched, event);
        }
    }

    if (watched == ui_settings->widget) // *************** Resize window realisation ************************* //
    {
        if (!this->isMaximized())
        {
            if (event->type() == QEvent::MouseButtonPress)
            {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->button() == Qt::LeftButton)
                {
                    _posX = this->pos().x();
                    _posY = this->pos().y();
                    _width = this->width();
                    _height = this->height();
                    mouseClickCoordinate = mouse_event->pos();
                    if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() > 62) && (mouseClickCoordinate.y() < (_height-12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                        clickPressed_Left_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() < 12))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                        clickPressed_Left_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > 12) && (mouseClickCoordinate.x() < (_width - 120)) && (mouseClickCoordinate.y() < 3))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                        clickPressed_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (_width - 12)) && (mouseClickCoordinate.y() < 12))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                        clickPressed_Right_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (_width - 12)) && (mouseClickCoordinate.y() > 62) && (mouseClickCoordinate.y() < (_height-12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                        clickPressed_Right_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (_width - 12)) && (mouseClickCoordinate.y() > (_height - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                        clickPressed_Right_Bottom_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > 12) && (mouseClickCoordinate.x() < (_width - 12)) && (mouseClickCoordinate.y() > (_height - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                        clickPressed_Bottom_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() > (_height - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                        clickPressed_Left_Bottom_ResizeFlag = true;
                    }
                    return QDialog::eventFilter(watched, event);
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
                        this->setGeometry(deltaX, _posY, this->width() - deltaWidth, _height);
                    }
                    else if (clickPressed_Left_Top_ResizeFlag == true)
                    {
                        this->setGeometry(deltaX, deltaY, this->width() - deltaWidth, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Top_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, deltaY, _width, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Right_Top_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, deltaY, _width + deltaWidth, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Right_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, _posY, _width + deltaWidth, _height);
                    }
                    else if (clickPressed_Right_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, _posY, _width + deltaWidth, _height + deltaHeight);
                    }
                    else if (clickPressed_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, _posY, _width, _height + deltaHeight);
                    }
                    else if (clickPressed_Left_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(deltaX, _posY, this->width() - deltaWidth, _height + deltaHeight);
                    }
                    return QDialog::eventFilter(watched, event);
                }
            }
        }
    }
    else if (watched == ui_settings->frame_hint) // *************** Drag window realisation ************************* //
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton)
            {
                mouseClickCoordinate = mouse_event->pos();
                clickPressedFlag = true;
                return QDialog::eventFilter(watched, event);
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
                return QDialog::eventFilter(watched, event);
            }
        }
        else if (event->type() == QEvent::MouseButtonDblClick)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                on_expandWindow_clicked();
                return QDialog::eventFilter(watched, event);
            }
        }
    }
    return QDialog::eventFilter(watched, event);
}

