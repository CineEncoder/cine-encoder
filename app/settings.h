#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QFileDialog>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QListView>
#include <QMap>
//#include <iostream>
//#include <QDebug>

namespace Ui
{
class Settings;
}


class Settings : public QDialog
{
    Q_OBJECT

public:

    explicit Settings(QWidget *parent = nullptr);

    ~Settings();

    void setParameters(QByteArray *ptr_settingsWindowGeometry, QFile *ptr_stn_file,
                       QString *ptr_output_folder, QString *ptr_temp_folder,
                       bool *ptr_protection, bool *ptr_showHDR_mode, int *ptr_timer_interval,
                       int *ptr_theme, QString *ptr_prefixName, QString *ptr_suffixName,
                       int *ptr_prefxType, int *ptr_suffixType, bool *ptr_hideInTrayFlag, QString *ptr_language);

private slots:

    void on_closeWindow_clicked();

    void on_buttonCancel_clicked();

    void on_buttonApply_clicked();

    void on_buttonReset_clicked();

    void on_buttonOutputPath_clicked();

    void on_buttonTempPath_clicked();

    void on_checkBox_2_clicked();

    void on_checkBox_3_clicked();

    void on_checkBox_tray_clicked();

    void on_comboBoxPrefixType_currentIndexChanged(int index);

    void on_comboBoxSuffixType_currentIndexChanged(int index);

    void on_comboBox_lang_currentIndexChanged(int index);

    void closeEvent(QCloseEvent *event);

    bool eventFilter(QObject *watched, QEvent *event);

    void on_buttonTab_1_clicked();

    void on_buttonTab_2_clicked();

private:

    Ui::Settings *ui_settings;

    QString _message;

    QString _curr_output_folder;

    QString _curr_temp_folder;

    QString _curr_prefixName;

    QString _curr_suffixName;

    QString _curr_language;

    int _curr_prefxType;

    int _curr_suffixType;

    int _curr_theme;

    int _curr_timer_interval;

    bool _curr_showHDR_mode;

    bool _curr_protection;

    bool _curr_hideInTrayFlag;

    bool _flag_save;

    QFile *_ptr_stn_file;

    QString *_ptr_output_folder;

    QString *_ptr_temp_folder;

    QString *_ptr_prefixName;

    QString *_ptr_suffixName;

    QString *_ptr_language;

    int *_ptr_prefxType;

    int *_ptr_suffixType;

    int *_ptr_timer_interval;

    int *_ptr_theme;

    bool *_ptr_showHDR_mode;

    bool *_ptr_protection;

    bool *_ptr_hideInTrayFlag;

    /**************** Geometry **************************/

    QByteArray *_ptr_settingsWindowGeometry;

    bool _expandWindowsState = false;

    bool clickPressedFlag = false;

    bool clickPressed_Left_ResizeFlag = false;

    bool clickPressed_Left_Top_ResizeFlag = false;

    bool clickPressed_Top_ResizeFlag = false;

    bool clickPressed_Right_Top_ResizeFlag = false;

    bool clickPressed_Right_ResizeFlag = false;

    bool clickPressed_Right_Bottom_ResizeFlag = false;

    bool clickPressed_Bottom_ResizeFlag = false;

    bool clickPressed_Left_Bottom_ResizeFlag = false;

    QPoint mouseClickCoordinate;

    QPoint mouseCoordinate;

    int oldWidth;

    int oldHeight;

    int curWidth;

    int curHeight;

    void call_task_complete(const QString &_message, const bool &timer_mode);

    QString callFileDialog(const QString title);

    void on_expandWindow_clicked();
};

#endif // SETTINGS_H
