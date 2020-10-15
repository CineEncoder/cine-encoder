#ifndef SMARTSETTINGS_H
#define SMARTSETTINGS_H

#include <QDialog>

namespace Ui {
class SmartSettings;
}

class SmartSettings : public QDialog
{
    Q_OBJECT

public slots:
    //void make_preset();


public:
    explicit SmartSettings(QWidget *parent = nullptr);
    ~SmartSettings();

private slots:
    void on_pushButton_11_clicked();

    //void on_pushButton_10_clicked();

    //void on_comboBox_27_currentTextChanged();

    //void on_checkBox_stateChanged();

    void on_horizontalSlider_valueChanged();

    void on_pushButton_10_clicked();

private:
    Ui::SmartSettings *ui_smartsettings;

};

#endif // SMARTSETTINGS_H
