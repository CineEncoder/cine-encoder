#ifndef SELECTPRESET_H
#define SELECTPRESET_H

#include <QDialog>

namespace Ui {
class SelectPreset;
}

class SelectPreset : public QDialog
{
    Q_OBJECT

public:
    explicit SelectPreset(QWidget *parent = nullptr);
    ~SelectPreset();

private slots:

    void on_pushButton_7_clicked();

    void on_pushButton_6_clicked();

    void on_actionRemove_preset_clicked();

    void on_actionEdit_preset_clicked();

    void add_section();

    void add_preset();

private:
    Ui::SelectPreset *ui_selectpreset;
};

#endif // SELECTPRESET_H
