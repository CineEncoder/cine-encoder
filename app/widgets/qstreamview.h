#ifndef QSTREAMVIEW_H
#define QSTREAMVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include "constants.h"


class QStreamView : public QWidget
{
    Q_OBJECT
public:
    enum class TypeOfView {
        Audio, Subtitle
    };
    QStreamView(QWidget *parent);
    ~QStreamView();
    void setTypeOfView(TypeOfView type);
    void clearList();
    void setList(Data &data);
    void clearTitles();
    void undoTitles();

private:
    QWidget *createCell(bool &state,
                        const QString &format,
                        QString &lang,
                        QString &title);
    QVBoxLayout *m_pLayout;
    TypeOfView m_type;
};

#endif // QSTREAMVIEW_H
