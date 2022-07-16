#include "qstreamview.h"
#include "helper.h"
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
#include <QRadioButton>
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
    QWidget(parent),
    m_pData(nullptr)
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
        if (item->widget()) {
            delete item->widget();
            delete item;
        }
    }
}

void QStreamView::setList(Data &data)
{
    m_pData = &data;
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
        for (int i = 0; i < FIELDS(audioFormats).size(); i++) {
            QWidget *cell = createCell(CHECKS(audioChecks)[i],
                                       FIELDS(audioFormats)[i],
                                       FIELDS(audioDuration)[i],
                                       FIELDS(audioLangs)[i],
                                       FIELDS(audioTitles)[i],
                                       FIELDS(audioChannels)[i],
                                       FIELDS(audioChLayouts)[i],
                                       "",
                                       CHECKS(audioDef)[i]);
            m_pLayout->addWidget(cell);
        }
        for (int i = 0; i < FIELDS(externAudioFormats).size(); i++) {
            QWidget *cell = createCell(CHECKS(externAudioChecks)[i],
                                       FIELDS(externAudioFormats)[i],
                                       FIELDS(externAudioDuration)[i],
                                       FIELDS(externAudioLangs)[i],
                                       FIELDS(externAudioTitles)[i],
                                       FIELDS(externAudioChannels)[i],
                                       FIELDS(externAudioChLayouts)[i],
                                       FIELDS(externAudioPath)[i],
                                       CHECKS(externAudioDef)[i],
                                       true);
            m_pLayout->addWidget(cell);
        }
    } else
    if (m_type == Content::Subtitle) {
        for (int i = 0; i < FIELDS(subtFormats).size(); i++) {
            QWidget *cell = createCell(CHECKS(subtChecks)[i],
                                       FIELDS(subtFormats)[i],
                                       FIELDS(subtDuration)[i],
                                       FIELDS(subtLangs)[i],
                                       FIELDS(subtTitles)[i],
                                       "",
                                       "",
                                       "",
                                       CHECKS(subtDef)[i]);
            m_pLayout->addWidget(cell);
        }
        for (int i = 0; i < FIELDS(externSubtFormats).size(); i++) {
            QWidget *cell = createCell(CHECKS(externSubtChecks)[i],
                                       FIELDS(externSubtFormats)[i],
                                       FIELDS(externSubtDuration)[i],
                                       FIELDS(externSubtLangs)[i],
                                       FIELDS(externSubtTitles)[i],
                                       "",
                                       "",
                                       FIELDS(externSubtPath)[i],
                                       CHECKS(externSubtDef)[i],
                                       true);
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
        Print(obj->objectName().toStdString());
        break;
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}

void QStreamView::resetDefFlags(const int ind)
{
    Q_LOOP(i, 1, m_pLayout->count()) {
        if (i != ind) {
            QLayoutItem *item = m_pLayout->itemAt(i);
            if (item) {
                if (item->widget()) {
                    QRadioButton *rbtn = item->widget()->findChild<QRadioButton*>();
                    if (rbtn)
                        rbtn->setChecked(false);
                }
            }
        }
    }

    if (m_type == Content::Audio) {
        m_pData->checks[Data::audioDef].fill(false);
        m_pData->checks[Data::externAudioDef].fill(false);
    } else
    if (m_type == Content::Subtitle) {
        m_pData->checks[Data::subtDef].fill(false);
        m_pData->checks[Data::externSubtDef].fill(false);
    }
}

QWidget *QStreamView::createCell(bool &state,
                                 const QString &format,
                                 const QString &dur,
                                 QString &lang,
                                 QString &title,
                                 const QString &channels,
                                 QString chLayouts,
                                 const QString &path,
                                 bool &deflt,
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
    lut->setContentsMargins(6,2,6,4);
    lut->setHorizontalSpacing(6);
    lut->setVerticalSpacing(4);
    cell->setLayout(lut);

    QRadioButton *rbtn = new QRadioButton(cell);
    rbtn->setChecked(deflt);
    rbtn->setFixedSize(QSize(12,12));
    rbtn->setToolTip(tr("Default"));
    connect(rbtn, &QRadioButton::clicked, this, [this, cell, rbtn, &deflt]() {
        resetDefFlags(m_pLayout->indexOf(cell));
        deflt = rbtn->isChecked();
    });
    lut->addWidget(rbtn, 0, 0, Qt::AlignLeft);

    // External Audio Label
    QFont fnt;
    fnt.setPointSize(8);
    QLabel *tit = QStreamViewPrivate::createLabel(cell, "extAudioLabel", "");
    tit->setFont(fnt);
    tit->setEnabled(false);
    tit->setMinimumSize(QSize(0, 12));
    tit->setMaximumSize(QSize(150, 12));
    lut->addWidget(tit, 0, 1, 1, 2, Qt::AlignLeft);
    if (externFlag)
        tit->setText(tr("external") + " ");

    if (!Helper::isSupported(format))
        tit->setText(tit->text() + tr("unsupported"));

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

    double duration_double = 0.001 * dur.toDouble();
    QString durationTime = Helper::timeConverter(static_cast<float>(duration_double));
    QLabel *labDuration = QStreamViewPrivate::createLabel(info, "labelDuration",
                                                    QString("%1: %2").arg(tr("Duration"),
                                                                          durationTime));
    labDuration->setEnabled(true);
    labDuration->setFixedHeight(ROW_HEIGHT);
    infoLut->addWidget(labDuration, 0, 0);

    // Label channels
    if (m_type == Content::Audio) {
        /*if (chLayouts.isEmpty())
            chLayouts = tr("No layouts");

        QLabel *labChLayouts = QStreamViewPrivate::createLabel(info, "labelChLayouts",
                                                        QString("%1: %2").arg(tr("Duration"), dur));
        labChLayouts->setEnabled(true);
        labChLayouts->setFixedHeight(ROW_HEIGHT);
        infoLut->addWidget(labChLayouts, 0, 0);*/

        QLabel *labCh = QStreamViewPrivate::createLabel(info, "labelChannels",
                                                        QString("%1: %2")
                                                        .arg(tr("Channels"),
                                                             Helper::recalcChannels(channels)));
        labCh->setEnabled(true);
        labCh->setFixedHeight(ROW_HEIGHT);
        infoLut->addWidget(labCh, 0, 1);
    }

    if (externFlag) {
        QLabel *labPath = QStreamViewPrivate::createLabel(info, "labelPath",
                                                          QString("%1: %2").arg(tr("Path"), path));
        labPath->setEnabled(true);
        labPath->setFixedHeight(ROW_HEIGHT);
        infoLut->addWidget(labPath, 1, 0, 1, 2);
    }

    QSpacerItem *sp_bottom = new QSpacerItem(5, 5, QSizePolicy::Fixed, QSizePolicy::Expanding);
    infoLut->addItem(sp_bottom, 5, 0);

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

    return cell;
}
