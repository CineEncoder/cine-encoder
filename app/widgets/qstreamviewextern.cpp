#include "qstreamviewextern.h"
#include <QHBoxLayout>
#include <QAction>
#include <QLineEdit>
#include <QCheckBox>

#define ROW_HEIGHT 22


QStreamViewExtern::QStreamViewExtern(QWidget *parent) :
    QWidget(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    QGridLayout *lut = new QGridLayout(this);
    lut->setContentsMargins(0,0,0,0);
    lut->setSpacing(6);
    setLayout(lut);

    QFont fnt;
    fnt.setPointSize(10);
    //fnt.setItalic(true);
    fnt.setBold(true);

    QSpacerItem *sps = new QSpacerItem(1, 2, QSizePolicy::Fixed, QSizePolicy::Fixed);
    lut->addItem(sps, 0, 0, 1, 1);

    m_label = new QLabel(this);
    m_label->setObjectName(QString::fromUtf8("extAudioLabel"));
    m_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_label->setFont(fnt);
    m_label->setText(tr("External audio"));
    //m_label->setEnabled(false);
    m_label->setVisible(false);
    m_label->setMinimumHeight(ROW_HEIGHT);
    m_label->setMaximumHeight(ROW_HEIGHT);
    m_label->setAutoFillBackground(false);
    m_label->setFrameShadow(QFrame::Plain);
    lut->addWidget(m_label, 0, 1, 1, 1, Qt::AlignLeft);

    m_pLayout = new QVBoxLayout(this);
    m_pLayout->setContentsMargins(0,0,0,0);
    m_pLayout->setSpacing(6);
    lut->addLayout(m_pLayout, 1, 0, 1, 2);
}

QStreamViewExtern::~QStreamViewExtern()
{

}

void QStreamViewExtern::clearList()
{
    m_label->setVisible(false);
    QLayoutItem *item;
    while ((item = m_pLayout->takeAt(0)) != nullptr) {
        if (item->widget())
            item->widget()->deleteLater();
    }
}

void QStreamViewExtern::setList(Data &data)
{
    clearList();
    const int cnt = data.externAudioFormats.size();
    for (int i = 0; i < cnt; i++) {
        QWidget *cell = createCell(data.externAudioChecks[i],
                                   data.externAudioFormats[i],
                                   data.externAudioLangs[i],
                                   data.externAudioTitles[i]);
        m_pLayout->addWidget(cell);
    }
    if (cnt > 0)
        m_label->setVisible(true);
}

QWidget *QStreamViewExtern::createCell(bool &state,
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
