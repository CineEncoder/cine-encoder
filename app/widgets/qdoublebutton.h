#ifndef QDOUBLEBUTTON_H
#define QDOUBLEBUTTON_H

#include <QWidget>
#include <QPushButton>


class QDoubleButton : public QWidget
{
    Q_OBJECT
public:
    QDoubleButton(QWidget *parent);
    ~QDoubleButton();
    void setIcons(const QIcon &lIcon,
                  const QIcon &rIcon);
    uchar currentIndex();
    void setCurrentIndex(uchar index);
    void setToolTips(const QString &lTip,
                     const QString &rTip);

signals:
    void indexChanged(uchar index);

private:
    void onLeft();
    void onRight();
    void switchButtons();
    QPushButton *m_pLButton,
                *m_pRButton;
    uchar m_index;
};

#endif // QDOUBLEBUTTON_H
