#ifndef OPENINGFILES_H
#define OPENINGFILES_H

#include <QDialog>
#include <math.h>

namespace Ui
{
    class OpeningFiles;
}

class OpeningFiles : public QDialog
{
    Q_OBJECT

public:

    explicit OpeningFiles(QWidget *parent = nullptr);

    ~OpeningFiles();

    void setParameters(bool &show, QPoint &position);

    void setText(QString &text);

    void setPercent(int &percent);

private:

    Ui::OpeningFiles *ui;
};

#endif // OPENINGFILES_H
