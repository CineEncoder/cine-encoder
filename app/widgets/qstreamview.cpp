#include "qstreamview.h"
#include <QHBoxLayout>
#include <QAction>
#include <QLineEdit>
#include <QCheckBox>
#include <QLabel>

#define ROW_HEIGHT 22


QStreamView::QStreamView(QWidget *parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_pLayout = new QVBoxLayout(this);
    m_pLayout->setContentsMargins(0,0,0,0);
    m_pLayout->setSpacing(6);
    setLayout(m_pLayout);
}

QStreamView::~QStreamView()
{

}

void QStreamView::setTypeOfView(TypeOfView type)
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
    int count = 0;
    if (m_type == TypeOfView::Audio) {
        count = data.audioFormats.size();
    } else
    if (m_type == TypeOfView::Subtitle) {
        count = data.subtFormats.size();
    }

    for (int i = 0; i < count; i++) {
        QWidget *cell = nullptr;
        if (m_type == TypeOfView::Audio) {
            cell = createCell(data.audioChecks[i],
                              data.audioFormats[i],
                              data.audioLangs[i],
                              data.audioTitles[i]);
        } else
        if (m_type == TypeOfView::Subtitle) {
            cell = createCell(data.subtChecks[i],
                              data.subtFormats[i],
                              data.subtLangs[i],
                              data.subtTitles[i]);
        }
        if (cell)
            m_pLayout->addWidget(cell);
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

QWidget *QStreamView::createCell(bool &state,
                                 const QString &format,
                                 QString &lang,
                                 QString &title)
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
    cell->setObjectName("Cell");
    cell->setMaximumWidth(350);
    cell->setMinimumHeight(34);
    QHBoxLayout *lut = new QHBoxLayout(cell);
    lut->setContentsMargins(6,6,6,6);
    lut->setSpacing(6);
    cell->setLayout(lut);

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
    lut->addWidget(chkBox);

    // Lang
    QLineEdit *line = new QLineEdit(cell);
    line->setObjectName(QString::fromUtf8("lineLang"));
    line->setEnabled(true);
    line->setText(lang);
    line->setCursorPosition(0);
    line->setMaximumSize(QSize(30, ROW_HEIGHT));
    connect(line, &QLineEdit::editingFinished, this, [line, &lang](){
        if (line->isModified()) {
            line->setModified(false);
            lang = line->text();
        }
    });
    connectAction(line, true);
    lut->addWidget(line);

    // Label
    QLabel *label = new QLabel(cell);
    label->setObjectName(QString::fromUtf8("labelTitle"));
    label->setText(tr("Title:"));
    label->setEnabled(true);
    label->setMinimumSize(QSize(0, ROW_HEIGHT));
    label->setMaximumSize(QSize(35, ROW_HEIGHT));
    label->setAutoFillBackground(false);
    label->setFrameShadow(QFrame::Plain);
    lut->addWidget(label);

    // Title
    QLineEdit *line_1 = new QLineEdit(cell);
    line_1->setObjectName(QString::fromUtf8("lineTitle"));
    line_1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    line_1->setMaximumSize(QSize(500, ROW_HEIGHT));
    line_1->setClearButtonEnabled(true);
    line_1->setText(title);
    line_1->setCursorPosition(0);
    connect(line_1, &QLineEdit::editingFinished, this, [line_1, &title](){
        if (line_1->isModified()) {
            line_1->setModified(false);
            title = line_1->text();
        }
    });
    connectAction(line_1, true);
    lut->addWidget(line_1);

    return cell;
}

