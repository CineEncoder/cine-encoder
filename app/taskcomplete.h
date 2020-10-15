#ifndef TASKCOMPLETE_H
#define TASKCOMPLETE_H

#include <QDialog>

namespace Ui {
class Taskcomplete;
}

class Taskcomplete : public QDialog
{
    Q_OBJECT

public:
    explicit Taskcomplete(QWidget *parent = nullptr);
    ~Taskcomplete();

private slots:
    void on_pushButton_4_clicked();

private:
    Ui::Taskcomplete *ui_taskcomplete;
};

#endif // TASKCOMPLETE_H
