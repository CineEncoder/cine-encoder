#include "qstreamview.h"
#include "helper.h"
#include <QMouseEvent>
#include <QMenu>
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
        line->setFixedHeight(ROW_HEIGHT * Helper::scaling());
        QObject::connect(line, &QLineEdit::editingFinished, [line, &text]() {
            if (line->isModified()) {
                line->setModified(false);
                text = line->text();
            }
        });
        return line;
    }

    QRadioButton *createRadio(QWidget *parent, const char *name, const QString &text, bool checked)
    {
        QRadioButton *btn = new QRadioButton(parent);
        btn->setObjectName(QString::fromUtf8(name));
        btn->setAutoExclusive(false);
        if (!text.isEmpty())
            btn->setText(text);
        btn->setChecked(checked);
        return btn;
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
    m_pLayout->setSpacing(1 * Helper::scaling());
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
    //m_pData = nullptr;
    QLayoutItem *item;
    while ((item = m_pLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            delete item->widget();
            delete item;
        }
    }
}

void QStreamView::setList(QString extension, Data &data)
{
    clearList();
    m_pData = &data;
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
    hw->setDefaultSectionSize(132 * Helper::scaling());
    hw->setFixedHeight(28 * Helper::scaling());
    hw->setModel(model);
    hw->setSectionResizeMode(0, QHeaderView::Fixed);
    hw->setSectionResizeMode(1, QHeaderView::Stretch);
    hw->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_pLayout->addWidget(hw);

    if (m_type == Content::Audio) {
        bool stub = false;
        for (int i = 0; i < FIELDS(audioFormats).size(); i++) {
            QWidget *cell = createCell(CHECKS(audioChecks)[i],
                                       extension,
                                       FIELDS(audioFormats)[i],
                                       FIELDS(audioDuration)[i],
                                       FIELDS(audioLangs)[i],
                                       FIELDS(audioTitles)[i],
                                       FIELDS(audioChannels)[i],
                                       FIELDS(audioChLayouts)[i],
                                       "",
                                       CHECKS(audioDef)[i],
                                       stub);
            m_pLayout->addWidget(cell);
        }
        for (int i = 0; i < FIELDS(externAudioFormats).size(); i++) {
            QWidget *cell = createCell(CHECKS(externAudioChecks)[i],
                                       extension,
                                       FIELDS(externAudioFormats)[i],
                                       FIELDS(externAudioDuration)[i],
                                       FIELDS(externAudioLangs)[i],
                                       FIELDS(externAudioTitles)[i],
                                       FIELDS(externAudioChannels)[i],
                                       FIELDS(externAudioChLayouts)[i],
                                       FIELDS(externAudioPath)[i],
                                       CHECKS(externAudioDef)[i],
                                       stub,
                                       true);
            m_pLayout->addWidget(cell);
        }
    } else
    if (m_type == Content::Subtitle) {
        for (int i = 0; i < FIELDS(subtFormats).size(); i++) {
            QWidget *cell = createCell(CHECKS(subtChecks)[i],
                                       extension,
                                       FIELDS(subtFormats)[i],
                                       FIELDS(subtDuration)[i],
                                       FIELDS(subtLangs)[i],
                                       FIELDS(subtTitles)[i],
                                       "",
                                       "",
                                       "",
                                       CHECKS(subtDef)[i],
                                       CHECKS(subtBurn)[i]);
            m_pLayout->addWidget(cell);
        }
        for (int i = 0; i < FIELDS(externSubtFormats).size(); i++) {
            QWidget *cell = createCell(CHECKS(externSubtChecks)[i],
                                       extension,
                                       FIELDS(externSubtFormats)[i],
                                       FIELDS(externSubtDuration)[i],
                                       FIELDS(externSubtLangs)[i],
                                       FIELDS(externSubtTitles)[i],
                                       "",
                                       "",
                                       FIELDS(externSubtPath)[i],
                                       CHECKS(externSubtDef)[i],
                                       CHECKS(externSubtBurn)[i],
                                       true);
            m_pLayout->addWidget(cell);
        }
    }
}

void QStreamView::deselectTitles()
{
    auto lines = findChildren<QCheckBox*>("checkStream");
    foreach (auto line, lines) {
        line->setChecked(false);
    }
    auto lines2 = findChildren<QRadioButton*>("burnInto");
    foreach (auto line, lines2) {
        line->setChecked(false);
    }
    auto lines3 = findChildren<QRadioButton*>("defaultStream");
    foreach (auto line, lines3) {
        line->setChecked(false);
    }
    if (m_type == Content::Audio) {
        m_pData->checks[Data::audioChecks].fill(false);
        m_pData->checks[Data::externAudioChecks].fill(false);
        m_pData->checks[Data::audioDef].fill(false);
    } else
    if (m_type == Content::Subtitle) {
        m_pData->checks[Data::subtChecks].fill(false);
        m_pData->checks[Data::externSubtChecks].fill(false);
        m_pData->checks[Data::subtDef].fill(false);
        m_pData->checks[Data::externSubtDef].fill(false);
        m_pData->checks[Data::externSubtBurn].fill(false);
    }
    setFocus();
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
    case QEvent::MouseButtonDblClick: {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->buttons() & Qt::LeftButton) {
            QWidget *cell = qobject_cast<QWidget*>(obj);
            QPushButton *btn = cell->findChild<QPushButton*>("expandBtn");
            if (btn)
                btn->click();
        }
        break;
    }
    case QEvent::MouseButtonPress: {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->buttons() & Qt::RightButton) {
            QWidget *cell = qobject_cast<QWidget*>(obj);
            QPushButton *btn = cell->findChild<QPushButton*>("expandBtn");
            bool expanded = false;
            if (btn)
                expanded = btn->property("expanded").toBool();
            QCheckBox *chkBox = cell->findChild<QCheckBox*>("checkStream");
            bool checked = false;
            if (chkBox)
                checked = (chkBox->checkState() == 2) ? true : false;
            QRadioButton *rbtn = cell->findChild<QRadioButton*>("defaultStream");
            bool deflt = false;
            if (rbtn)
                deflt = rbtn->isChecked();

            QMenu *streamMenu = new QMenu(cell);
            QAction *pActExpand = new QAction(expanded ? tr("Collapse") : tr("Expand"), streamMenu);
            QAction *pActCheck = new QAction(checked ? tr("Uncheck") : tr("Check"), streamMenu);
            QAction *pActSetDef = new QAction(tr("Set as default track"), streamMenu);
            QAction *pActDeselectAll = new QAction(tr("Deselect all"), streamMenu);
            connect(pActExpand, &QAction::triggered, this, [btn]() {
                if (btn)
                    btn->click();
            });
            connect(pActCheck, &QAction::triggered, this, [chkBox]() {
                if (chkBox)
                    chkBox->click();
            });
            connect(pActSetDef, &QAction::triggered, this, [rbtn, deflt]() {
                if (rbtn && !deflt)
                    rbtn->click();
            });
            connect(pActDeselectAll, &QAction::triggered, this, [this]() {
                deselectTitles();
            });
            QAction *pActExtract = nullptr;
            const bool external = obj->property("External").toBool();
            const int num = obj->property("Number").toInt();
            if (!external) {
                pActExtract = new QAction(tr("Extract track"), streamMenu);
                connect(pActExtract, &QAction::triggered, this, [this, num]() {
                    emit onExtractTrack(m_type, num);
                });
            }
            streamMenu->addAction(pActExpand);
            streamMenu->addSeparator();
            streamMenu->addAction(pActCheck);
            streamMenu->addAction(pActDeselectAll);
            streamMenu->addAction(pActSetDef);
            if (!external) {
                streamMenu->addSeparator();
                streamMenu->addAction(pActExtract);
            }
            const QPoint globPos = cell->mapToGlobal(mouse_event->pos() + QPoint(0, 10));
            streamMenu->exec(globPos);
            streamMenu->deleteLater();
            QStreamViewPrivate::onRowHovered(obj, false);
        }
        break;
    }
    default:
        break;
    }
    return QWidget::eventFilter(obj, event);
}

void QStreamView::resetCheckFlags(const int ind)
{
    Q_LOOP(i, 1, m_pLayout->count()) {
        if (i != ind) {
            QLayoutItem *item = m_pLayout->itemAt(i);
            if (item && item->widget()) {
                QCheckBox *chkBox = item->widget()->findChild<QCheckBox*>("checkStream");
                if (chkBox)
                    chkBox->setChecked(false);
            }
        }
    }

    if (m_type == Content::Audio) {
        m_pData->checks[Data::audioChecks].fill(false);
        m_pData->checks[Data::externAudioChecks].fill(false);
    } else
    if (m_type == Content::Subtitle) {
        m_pData->checks[Data::subtChecks].fill(false);
        m_pData->checks[Data::externSubtChecks].fill(false);
    }
}

void QStreamView::resetDefFlags(const int ind)
{
    Q_LOOP(i, 1, m_pLayout->count()) {
        if (i != ind) {
            QLayoutItem *item = m_pLayout->itemAt(i);
            if (item && item->widget()) {
                QRadioButton *rbtn = item->widget()->findChild<QRadioButton*>("defaultStream", Qt::FindDirectChildrenOnly);
                if (rbtn)
                    rbtn->setChecked(false);
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

void QStreamView::resetBurnFlags(const int ind)
{
    Q_LOOP(i, 1, m_pLayout->count()) {
        if (i != ind) {
            QLayoutItem *item = m_pLayout->itemAt(i);
            if (item && item->widget()) {
                QRadioButton *rbtn = item->widget()->findChild<QRadioButton*>("burnInto");
                if (rbtn)
                    rbtn->setChecked(false);
            }
        }
    }

    m_pData->checks[Data::subtBurn].fill(false);
    m_pData->checks[Data::externSubtBurn].fill(false);
}

QWidget *QStreamView::createCell(bool &state,
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
                                 bool externFlag
                                 )
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
    cell->setProperty("Number", m_pLayout->count() - 1);
    cell->setProperty("External", externFlag);
    cell->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    cell->setMinimumHeight(46 * Helper::scaling());
    QGridLayout *lut = new QGridLayout(cell);
    lut->setContentsMargins(6,2,6,4);
    lut->setHorizontalSpacing(6 * Helper::scaling());
    lut->setVerticalSpacing(4 * Helper::scaling());
    cell->setLayout(lut);

    // Radio button 'Default stream'
    QRadioButton *rbtn = QStreamViewPrivate::createRadio(cell, "defaultStream", "", deflt);
    rbtn->setFixedSize(QSize(12,12) * Helper::scaling());
    rbtn->setToolTip(tr("Default"));
    connect(rbtn, &QRadioButton::clicked, this, [this, cell, &burn, &deflt, &state](bool checked) {
        resetBurnFlags(m_pLayout->indexOf(cell));
        resetDefFlags(m_pLayout->indexOf(cell));
        deflt = checked;
        QLayoutItem *item = m_pLayout->itemAt(m_pLayout->indexOf(cell));
        if (item && item->widget()) {
            if (deflt) {
                QCheckBox *chkBox = item->widget()->findChild<QCheckBox*>("checkStream");
                if (chkBox && !chkBox->isChecked()) {
                    chkBox->setChecked(true);
                    state = true;
                }
            } else {
                QRadioButton *brn_rbtn = item->widget()->findChild<QRadioButton*>("burnInto");
                if (brn_rbtn && brn_rbtn->isChecked()) {
                    brn_rbtn->setChecked(false);
                    burn = false;
                }
            }
        }
    });
    lut->addWidget(rbtn, 0, 0, Qt::AlignLeft);

    // External Audio Label
    QFont fnt;
    fnt.setPointSize(8);
    QLabel *tit = QStreamViewPrivate::createLabel(cell, "extAudioLabel", "");
    tit->setFont(fnt);
    tit->setEnabled(false);
    tit->setMinimumSize(QSize(0, 12) * Helper::scaling());
    tit->setMaximumSize(QSize(150, 12) * Helper::scaling());
    lut->addWidget(tit, 0, 1, 1, 2, Qt::AlignLeft);
    if (externFlag)
        tit->setText(tr("external") + " ");

    QWidget *info = new QWidget(cell);
    info->setObjectName("infoWidget");
    info->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    info->hide();
    lut->addWidget(info, 2, 0, 3, 0);
    QGridLayout *infoLut = new QGridLayout(info);
    infoLut->setContentsMargins(6,6,6,6);
    infoLut->setHorizontalSpacing(6 * Helper::scaling());
    infoLut->setVerticalSpacing(2 * Helper::scaling());
    info->setLayout(infoLut);

    double duration_double = 0.001 * dur.toDouble();
    QString durationTime = Helper::timeConverter(static_cast<float>(duration_double));
    QLabel *labDuration = QStreamViewPrivate::createLabel(info, "labelDuration",
                                                    QString("%1: %2").arg(tr("Duration"),
                                                                          durationTime));
    labDuration->setEnabled(true);
    labDuration->setFixedHeight(ROW_HEIGHT * Helper::scaling());
    infoLut->addWidget(labDuration, 0, 0);

    bool burn_only = false;
    // Label channels
    if (m_type == Content::Audio) {
        if (!Helper::isAudioSupported(extension, format))
            tit->setText(tit->text() + tr("unsupported"));
        if (chLayouts.isEmpty())
            chLayouts = tr("No layouts");

        QLabel *labChLayouts = QStreamViewPrivate::createLabel(info, "labelChLayouts",
                                                        QString("%1: %2").arg(tr("Layouts"), chLayouts));
        labChLayouts->setEnabled(true);
        labChLayouts->setFixedHeight(ROW_HEIGHT);
        infoLut->addWidget(labChLayouts, 1, 1);

        QLabel *labCh = QStreamViewPrivate::createLabel(info, "labelChannels",
                                                        QString("%1: %2")
                                                        .arg(tr("Channels"),
                                                             Helper::recalcChannels(channels)));
        labCh->setEnabled(true);
        labCh->setFixedHeight(ROW_HEIGHT * Helper::scaling());
        infoLut->addWidget(labCh, 0, 1);
    } else
    if (m_type == Content::Subtitle) {
        if (!Helper::isSubtitleSupported(extension, format)) {
            tit->setText(tit->text() + tr("Hard-burn only"));
            burn_only = true;
            state = false;
        }
        QRadioButton *brn_rbtn = QStreamViewPrivate::createRadio(info, "burnInto", tr("Burn into video"), burn);
        brn_rbtn->setFixedHeight(12 * Helper::scaling());
        brn_rbtn->setToolTip(tr("Burn into video"));
        // Stream can only be burnt for target.
        if (burn_only) {
            brn_rbtn->setChecked(true);
        }
        connect(brn_rbtn, &QRadioButton::clicked, this, [this, cell, &burn, &deflt, &state, &burn_only](bool checked) {
            resetBurnFlags(m_pLayout->indexOf(cell));
            resetDefFlags(m_pLayout->indexOf(cell));
            resetCheckFlags(m_pLayout->indexOf(cell));
            burn = checked || burn_only;
            if (burn) {
                QLayoutItem *item = m_pLayout->itemAt(m_pLayout->indexOf(cell));
                if (item && item->widget()) {
                    // Cannot copy stream if burn is set.
                    QCheckBox *chkBox = item->widget()->findChild<QCheckBox*>("checkStream");
                    if (chkBox && chkBox->isChecked()) {
                        chkBox->setChecked(false);
                        state = false;
                    }
                    QRadioButton *rbtn = item->widget()->findChild<QRadioButton *>("defaultStream",
                                                                                   Qt::FindDirectChildrenOnly);
                    // Don't force this here - the default button is the way that the hard-burn is enabled.
                    if (!burn_only) {
                        if (rbtn && !rbtn->isChecked()) {
                            rbtn->setChecked(true);
                            deflt = true;
                        }
                    }
                }
            }
        });
        infoLut->addWidget(brn_rbtn, 0, 1, Qt::AlignLeft);
    }

    // Label external path
    if (externFlag) {
        const QString elidedPath = Helper::elideText(this, path, Qt::ElideMiddle);
        QLabel *labPath = QStreamViewPrivate::createLabel(info, "labelPath",
                                                          QString("%1: %2").arg(tr("Path"), elidedPath));
        labPath->setEnabled(true);
        labPath->setFixedHeight(ROW_HEIGHT * Helper::scaling());
        infoLut->addWidget(labPath, 2, 0, 1, 2);
    }

    QSpacerItem *sp_bottom = new QSpacerItem(5, 5, QSizePolicy::Fixed, QSizePolicy::Expanding);
    infoLut->addItem(sp_bottom, 5, 0);

    // Expand button
    QPushButton *btn = new QPushButton(cell);
    btn->setObjectName(QString::fromUtf8("expandBtn"));
    btn->setFixedSize(QSize(12, 12) * Helper::scaling());
    connect(btn, &QPushButton::clicked, this, [cell, btn, info]() {
        if (cell->minimumHeight() == 46 * Helper::scaling()) {
            QStreamViewPrivate::onRowResize(cell, 46 * Helper::scaling(), 120 * Helper::scaling());
            btn->setProperty("expanded", true);
            btn->style()->polish(btn);
            info->show();
        } else {
            QStreamViewPrivate::onRowResize(cell, 120 * Helper::scaling(), 46 * Helper::scaling());
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
    num->setMinimumWidth(14 * Helper::scaling());
    lut->addWidget(num, 1, 0, Qt::AlignLeft);

    // Check
    QCheckBox *chkBox = new QCheckBox(cell);
    chkBox->setObjectName(QString::fromUtf8("checkStream"));
    chkBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    chkBox->setFixedWidth(100 * Helper::scaling());
    chkBox->setText(format);
    if (burn_only && (m_type == Content::Subtitle)) {
        chkBox->setEnabled(false);
        chkBox->setChecked(false);
    }
    else
    {
        chkBox->setEnabled(true);
        chkBox->setChecked(state);
    }
    // Burn is whether the user selected to burn; burn_only is when only burning is an option.
    // Default marks the default stream, which triggers burn. A stream cannot be burnt if it is not default.
    connect(chkBox, &QCheckBox::clicked, this, [this, cell, chkBox, &burn, &burn_only, &state, &deflt](){
        state = (chkBox->checkState() == 2) ? true : false;
        // Burn-only prohibits the copy of subtitle streams (target format cannot support the stream).
        if (burn_only)
        {
            state = false;
        }
        if (!state) {
            QLayoutItem *item = m_pLayout->itemAt(m_pLayout->indexOf(cell));
            if (item && item->widget()) {
                QRadioButton *rbtn = item->widget()->findChild<QRadioButton*>("defaultStream", Qt::FindDirectChildrenOnly);
                if (rbtn && rbtn->isChecked()) {
                    rbtn->setChecked(false);
                    deflt = false;
                }
                QRadioButton *brn_rbtn = item->widget()->findChild<QRadioButton*>("burnInto");
                if (brn_rbtn /* && brn_rbtn->isChecked()*/ ) {
                    burn = false;
                    if (burn_only)
                    {
                        burn = true;
                    }
                    brn_rbtn->setChecked(burn);
                }
            }
        }
    });
    lut->addWidget(chkBox, 1, 1);

    // Title
    QLineEdit *line_1 = QStreamViewPrivate::createLine(cell, "lineTitle", title);
    line_1->setClearButtonEnabled(true);
    connectAction(line_1, true);
    lut->addWidget(line_1, 1, 2);

    // Lang
    QLineEdit *line = QStreamViewPrivate::createLine(cell, "lineLang", lang);
    line->setMaximumWidth(30 * Helper::scaling());
    connectAction(line, true);
    lut->addWidget(line, 1, 3);

    return cell;
}
