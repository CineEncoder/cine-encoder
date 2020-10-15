#include "taskcomplete.h"
#include "ui_taskcomplete.h"

extern QString _message;


Taskcomplete::Taskcomplete(QWidget *parent) :
    QDialog(parent),
    ui_taskcomplete(new Ui::Taskcomplete)
{
    ui_taskcomplete->setupUi(this);
    ui_taskcomplete->textBrowser->clear();
    ui_taskcomplete->textBrowser->append(_message);
}

Taskcomplete::~Taskcomplete()
{
    delete ui_taskcomplete;
}


void Taskcomplete::on_pushButton_4_clicked()
{
    close();
}
