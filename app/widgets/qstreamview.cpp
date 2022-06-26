#include "qstreamview.h"
#include <QPropertyAnimation>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QEvent>
#include <QStyle>
#include <QLayout>
#include <QAction>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <iostream>

#define ROW_HEIGHT 22

namespace QStreamViewPrivate {
    QLabel *createLabel(QWidget *parent, const char *name, const QString &text)
    {
        QLabel *label = new QLabel(parent);
        label->setObjectName(QString::fromUtf8(name));
        label->setText(text);
        label->setAutoFillBackground(false);
        label->setFrameShadow(QFrame::Plain);
        return label;
    }

    QLineEdit *createLine(QWidget *parent, const char *name, QString &text)
    {
        QLineEdit *line = new QLineEdit(parent);
        line->setObjectName(QString::fromUtf8(name));
        line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        line->setEnabled(true);
        line->setText(text);
        line->setCursorPosition(0);
        line->setFixedHeight(ROW_HEIGHT);
        QObject::connect(line, &QLineEdit::editingFinished, [line, &text]() {
            if (line->isModified()) {
                line->setModified(false);
                text = line->text();
            }
        });
        return line;
    }

    void onRowHovered(QObject *obj, bool flag)
    {
        QWidget *wgt = dynamic_cast<QWidget*>(obj);
        if (wgt) {
            wgt->setProperty("hover", flag);
            wgt->style()->polish(wgt);
        }
    }

    void onRowResize(QWidget *wgt, int start, int end)
    {
        QPropertyAnimation *animation = new QPropertyAnimation(wgt, "minimumHeight");
        animation->setDuration(200);
        animation->setStartValue(start);
        animation->setEndValue(end);
        animation->start(QPropertyAnimation::DeleteWhenStopped);
    }
}

QStreamView::QStreamView(QWidget *parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_pLayout = new QVBoxLayout(this);
    m_pLayout->setContentsMargins(0,0,0,0);
    m_pLayout->setSpacing(1);
    m_pLayout->setSizeConstraint(QLayout::SetMinimumSize);
    setLayout(m_pLayout);
}

QStreamView::~QStreamView()
{

}

void QStreamView::setContentType(Content type)
{
    m_type = type;
}

void QStreamView::clearList()
{
    QLayoutItem *item;
    while ((item = m_pLayout->takeAt(0)) != nullptr) {
        if (item->widget())
            item->widget()->deleteLater();
    }
}

void QStreamView::setList(Data &data)
{
    clearList();
    const QString columns[] = {
        tr("Format"), tr("Title"), tr("Language")
    };
    QStandardItemModel *model = new QStandardItemModel(this);
    Q_LOOP(i, 0, 3) {
        QStandardItem *__item = new QStandardItem(columns[i]);
        model->setHorizontalHeaderItem(i, __item);
    }
    QHeaderView *hw = new QHeaderView(Qt::Horizontal, this);
    QFont fnt = hw->font();
    fnt.setItalic(true);
    fnt.setBold(true);
    hw->setFont(fnt);
    hw->setDefaultSectionSize(132);
    hw->setFixedHeight(28);
    hw->setModel(model);
    hw->setSectionResizeMode(0, QHeaderView::Fixed);
    hw->setSectionResizeMode(1, QHeaderView::Stretch);
    hw->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_pLayout->addWidget(hw);

    if (m_type == Content::Audio) {
        for (int i = 0; i < data.audioFormats.size(); i++) {
            QWidget *cell = createCell(data.audioChecks[i],
                                       data.audioFormats[i],
                                       data.audioLangs[i],
                                       data.audioTitles[i],
                                       data.audioChannels[i],
                                       data.audioChLayouts[i],
                                       "");
            m_pLayout->addWidget(cell);
        }
        for (int i = 0; i < data.externAudioFormats.size(); i++) {
            QWidget *cell = createCell(data.externAudioChecks[i],
                                       data.externAudioFormats[i],
                                       data.externAudioLangs[i],
                                       data.externAudioTitles[i],
                                       data.externAudioChannels[i],
                                       data.externAudioChLayouts[i],
                                       data.externAudioPath[i],
                                       true);
            m_pLayout->addWidget(cell);
        }
    } else
    if (m_type == Content::Subtitle) {
        for (int i = 0; i < data.subtFormats.size(); i++) {
            QWidget *cell = createCell(data.subtChecks[i],
                                       data.subtFormats[i],
                                       data.subtLangs[i],
                                       data.subtTitles[i],
                                       "", "", "");
            m_pLayout->addWidget(cell);
        }
    }
}

void QStreamView::clearTitles()
{
    auto lines = findChildren<QLineEdit*>("lineTitle");
    foreach (auto line, lines) {
        line->clear();
        line->insert("");
        line->setFocus();
        line->setModified(true);
    }
    setFocus();
}

void QStreamView::undoTitles()
{
    auto lines = findChildren<QLineEdit*>("lineTitle");
    foreach (auto line, lines) {
        line->undo();
        if (line->text() != "") {
            line->setFocus();
            line->setCursorPosition(0);
            line->setModified(true);
        }
    }
    setFocus();
}

bool QStreamView::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type()) {
    case QEvent::HoverEnter:
        QStreamViewPrivate::onRowHovered(obj, true);
        break;
    case QEvent::HoverLeave:
        QStreamViewPrivate::onRowHovered(obj, false);
        break;
    case QEvent::MouseButtonPress:
        Dump(obj->objectName().toStdString());
        break;
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}

QWidget *QStreamView::createCell(bool &state,
                                 const QString &format,
                                 QString &lang,
                                 QString &title,
                                 const QString &channels,
                                 QString chLayouts,
                                 const QString &path,
                                 bool externFlag)
{
    auto connectAction = [this](QLineEdit* line, bool isVisible)->void {
        auto actionList = line->findChildren<QAction*>();
        if (!actionList.isEmpty()) {
            connect(actionList.first(), &QAction::triggered, this, [this, line]() {
                line->clear();
                line->insert("");
                line->setModified(true);
                setFocus();
            });
        }
        if (!isVisible) {
            line->setEnabled(true);
            line->setVisible(false);
        }
    };

    QWidget *cell = new QWidget(this);
    cell->setAttribute(Qt::WA_Hover);
    cell->installEventFilter(this);
    cell->setObjectName("Cell");
    cell->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    cell->setMinimumHeight(46);
    QGridLayout *lut = new QGridLayout(cell);
    lut->setContentsMargins(6,3,6,4);
    lut->setHorizontalSpacing(6);
    lut->setVerticalSpacing(4);
    cell->setLayout(lut);

    // External Audio Label
    QFont fnt;
    fnt.setPointSize(8);
    if (externFlag) {
        QLabel *tit = QStreamViewPrivate::createLabel(cell, "extAudioLabel", tr("external"));
        tit->setFont(fnt);
        tit->setEnabled(false);
        tit->setMinimumSize(QSize(0, 10));
        tit->setMaximumSize(QSize(100, 10));
        lut->addWidget(tit, 0, 1, Qt::AlignLeft);
    }

    QWidget *info = new QWidget(cell);
    info->setObjectName("infoWidget");
    info->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    info->hide();
    lut->addWidget(info, 2, 0, 3, 0);
    QGridLayout *infoLut = new QGridLayout(info);
    infoLut->setContentsMargins(6,6,6,6);
    infoLut->setHorizontalSpacing(6);
    infoLut->setVerticalSpacing(2);
    info->setLayout(infoLut);

    // Label channels
    if (m_type == Content::Audio) {
        QLabel *labCh = QStreamViewPrivate::createLabel(info, "labelChannels",
                                                        QString("%1: %2").arg(tr("Channels"), channels));
        labCh->setEnabled(true);
        labCh->setFixedHeight(ROW_HEIGHT);
        infoLut->addWidget(labCh, 0, 0);

        /*QSpacerItem *sp = new QSpacerItem(5, 5, QSizePolicy::Expanding, QSizePolicy::Fixed);
        infoLut->addItem(sp, 0, 1);*/

        if (chLayouts.isEmpty())
            chLayouts = tr("No layouts");

        QLabel *labChLayouts = QStreamViewPrivate::createLabel(info, "labelChLayouts",
                                                        QString("%1: %2").arg(tr("Layouts"), chLayouts));
        labChLayouts->setEnabled(true);
        labChLayouts->setFixedHeight(ROW_HEIGHT);
        infoLut->addWidget(labChLayouts, 0, 1);
    }

    if (externFlag) {
        QLabel *labPath = QStreamViewPrivate::createLabel(info, "labelPath",
                                                          QString("%1: %2").arg(tr("Path"), path));
        labPath->setEnabled(true);
        labPath->setFixedHeight(ROW_HEIGHT);
        infoLut->addWidget(labPath, 1, 0, 0, 2);
    }

    QSpacerItem *sp_bottom = new QSpacerItem(5, 5, QSizePolicy::Fixed, QSizePolicy::Expanding);
    infoLut->addItem(sp_bottom, 10, 0);

    // Button
    QPushButton *btn = new QPushButton(cell);
    btn->setObjectName(QString::fromUtf8("audioExpandBtn"));
    btn->setFixedSize(QSize(12, 12));
    connect(btn, &QPushButton::clicked, this, [cell, btn, info]() {
        if (cell->minimumHeight() == 46) {
            QStreamViewPrivate::onRowResize(cell, 46, 120);
            btn->setProperty("expanded", true);
            btn->style()->polish(btn);
            info->show();
        } else {
            QStreamViewPrivate::onRowResize(cell, 120, 46);
            btn->setProperty("expanded", false);
            btn->style()->polish(btn);
            info->hide();
        }
    });
    lut->addWidget(btn, 0, 3, Qt::AlignRight);

    // Number
    QLabel *num = QStreamViewPrivate::createLabel(cell, "numAudioLabel",
                                                  QString::number(m_pLayout->count()) + ".");
    num->setFont(fnt);
    num->setEnabled(false);
    num->setMinimumWidth(14);
    lut->addWidget(num, 1, 0, Qt::AlignLeft);

    // Check
    QCheckBox *chkBox = new QCheckBox(cell);
    chkBox->setObjectName(QString::fromUtf8("checkBox"));
    chkBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    chkBox->setFixedWidth(100);
    chkBox->setText(format);
    chkBox->setEnabled(true);
    chkBox->setChecked(state);
    connect(chkBox, &QCheckBox::clicked, this, [chkBox, &state](){
        state = (chkBox->checkState() == 2) ? true : false;
    });
    lut->addWidget(chkBox, 1, 1);

    // Title
    QLineEdit *line_1 = QStreamViewPrivate::createLine(cell, "lineTitle", title);
    line_1->setClearButtonEnabled(true);
    connectAction(line_1, true);
    lut->addWidget(line_1, 1, 2);

    // Lang
    QLineEdit *line = QStreamViewPrivate::createLine(cell, "lineLang", lang);
    line->setMaximumWidth(30);
    connectAction(line, true);
    lut->addWidget(line, 1, 3);

    // Label
    /*QLabel *label = new QLabel(cell);
    label->setObjectName(QString::fromUtf8("labelTitle"));
    label->setText(tr("Title:"));
    label->setEnabled(true);
    label->setMaximumWidth(35);
    label->setFixedHeight(ROW_HEIGHT);
    label->setAutoFillBackground(false);
    label->setFrameShadow(QFrame::Plain);
    lut->addWidget(label, 1, 3);*/

    return cell;
}
