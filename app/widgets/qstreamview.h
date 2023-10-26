#ifndef QSTREAMVIEW_H
#define QSTREAMVIEW_H

#include <QWidget>
#include <QVBoxLayout>
#include "constants.h"

using namespace Constants;

class QStreamView : public QWidget
{
    Q_OBJECT
public:
    enum class Content {
        Audio, Subtitle
    };
    enum class TrackType {
        Internal, External
    };
    QStreamView(QWidget *parent);
    ~QStreamView();
    void setContentType(Content type);
    void clearList();
    void setList(QString container, Data &data);
    void deselectTitles();
    void clearTitles();
    void undoTitles();

signals:
    void onExtractTrack(QStreamView::Content type, int track);

private:
    virtual bool eventFilter(QObject*, QEvent*) final;
    void resetCheckFlags(const int ind);
    void resetDefFlags(const int ind);
    void resetBurnFlags(const int ind);
    QWidget *createCell(bool &state,
                        QString &extension,
                        const QString &format,
                        const QString &dur,
                        QString &lang,
                        QString &title,
                        const QString &channels,
                        QString chLayouts,
                        const QString &path,
                        bool &deflt,
                        bool &burn,
                        bool externFlag = false);
    QVBoxLayout *m_pLayout;
    Content m_type;
    Data *m_pData;
};

#endif // QSTREAMVIEW_H
