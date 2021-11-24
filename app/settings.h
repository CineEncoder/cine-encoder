/***********************************************************************

                          C I N E   E N C O D E R
                                JULY, 2020
                            COPYRIGHT (C) 2020

 FILE: settings.h
 MODIFIED: November, 2021
 COMMENT:
 LICENSE: GNU General Public License v3.0

***********************************************************************/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QFileDialog>
#include <QCloseEvent>
#include <QMouseEvent>
#include <QHoverEvent>
#include <QListView>
#include <QMap>
#include <QFontDatabase>
#include <QStringListModel>



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

    void setParameters(QByteArray *ptr_settingsWindowGeometry, QString *ptr_output_folder,
                       QString *ptr_temp_folder, bool *ptr_protection, bool *ptr_showHDR_mode,
                       int *ptr_timer_interval, int *ptr_theme, QString *ptr_prefixName,
                       QString *ptr_suffixName, int *ptr_prefxType, int *ptr_suffixType,
                       bool *ptr_hideInTrayFlag, QString *ptr_language, const QString &_desktopEnv,
                       int *ptr_fontSize, QString *ptr_font);
private slots:

    void closeEvent(QCloseEvent *event);

    void on_closeWindow_clicked();

    void on_buttonCancel_clicked();

    void on_buttonApply_clicked();

    void on_buttonReset_clicked();

    void on_buttonOutputPath_clicked();

    void on_buttonTempPath_clicked();

    void on_checkBox_protection_clicked();

    void on_comboBoxPrefixType_currentIndexChanged(int index);

    void on_comboBoxSuffixType_currentIndexChanged(int index);

    void on_comboBox_font_currentIndexChanged(const QString &arg1);

    bool eventFilter(QObject *watched, QEvent *event);

    void on_buttonTab_1_clicked();

    void on_buttonTab_2_clicked();

private:

    Ui::Settings *ui;

    QString desktopEnv;

    QString *_ptr_output_folder;

    QString *_ptr_temp_folder;

    QString *_ptr_prefixName;

    QString *_ptr_suffixName;

    QString *_ptr_language;

    QString *_ptr_font;

    int *_ptr_fontSize;

    int *_ptr_prefxType;

    int *_ptr_suffixType;

    int *_ptr_timer_interval;

    int *_ptr_theme;

    bool *_ptr_showHDR_mode;

    bool *_ptr_protection;

    bool *_ptr_hideInTrayFlag;

    /**************** Geometry **************************/

    enum Resize {
        LEFT,   LEFT_TOP,   LEFT_BOTTOM,   TOP,
        RIGHT,  RIGHT_TOP,  RIGHT_BOTTOM,  BOTTOM
    };

    QByteArray *_ptr_settingsWindowGeometry;

    bool _expandWindowsState;

    bool clickPressedFlag;

    QVector<bool> clickPressedToResizeFlag;

    QPoint mouseClickCoordinate;

    int oldWidth;

    int oldHeight;

    void on_expandWindow_clicked();

    QString callFileDialog(const QString title);
};

#endif // SETTINGS_H
