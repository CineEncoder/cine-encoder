#include <QtGlobal>
#include <QMenu>
#include <QMessageBox>
#include <iostream>
#include "preset.h"
#include "selectpreset.h"
#include "ui_selectpreset.h"

extern QString _cur_preset_name;
extern int p1, p2, p3, p8, p11, p12, p13, p18, p21, p22, pos_top, pos_cld;
extern QString p4, p5, p6, p9, p10, p14, p15, p16, p17, p19, p20;
extern bool p7;
extern QVector <QVector <QString> > _preset_table;

SelectPreset::SelectPreset(QWidget *parent) :
    QDialog(parent),
    ui_selectpreset(new Ui::SelectPreset)
{
    ui_selectpreset->setupUi(this);
    ui_selectpreset->treeWidget->clear();
    int n =  _preset_table[0].size();
    int m = _preset_table.size();
    QString type;
    QColor color, color_child;
    color.setRgba(qRgb(10, 180, 255));
    color_child.setRgba(qRgb(210, 210, 210));
    QFont font, font_1;
    font.setBold(true);
    font_1.setLetterSpacing(QFont::PercentageSpacing, 99);
    for (int i = 0; i < n; i++) {
        type = _preset_table[23][i];
        if (type == "TopLewelItem") {
            QTreeWidgetItem *root = new QTreeWidgetItem();
            root->setText(0, _preset_table[0][i]);
            ui_selectpreset->treeWidget->addTopLevelItem(root);
            ui_selectpreset->treeWidget->setCurrentItem(root);
            root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
            root->setFont(0, font);
            root->setForeground(0, color);
        };
        if (type == "ChildItem") {
            QTreeWidgetItem *item = ui_selectpreset->treeWidget->currentItem();
            QTreeWidgetItem *child = new QTreeWidgetItem();
            for (int j = 0; j < 23; j++) {
                child->setText(j, _preset_table[j][i]);
            };
            child->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
            child->setFont(0, font_1);
            child->setForeground(0, color_child);
            item->addChild(child);
        };
    };
    QTreeWidgetItem *item = ui_selectpreset->treeWidget->topLevelItem(pos_top)->child(pos_cld);
    ui_selectpreset->treeWidget->setCurrentItem(item);
    std::cout << n << " x " << m << std::endl; // Table size
    for (int i=1; i<=24; i++) {
        ui_selectpreset->treeWidget->hideColumn(i);
    };
    QAction *addsection = new QAction(tr("Add section"), this);
    QAction *addpreset = new QAction(tr("Add new preset"), this);
    connect(addsection, &QAction::triggered, this, &SelectPreset::add_section);
    connect(addpreset, &QAction::triggered, this, &SelectPreset::add_preset);
    QMenu* menu = new QMenu(this);
    menu->addAction(addsection);
    menu->addAction(addpreset);
    menu->setStyleSheet("QMenu {background-color: rgb(5, 20, 25);} "
                        "QMenu::item {color: rgb(255, 255, 255); background-color: transparent;} "
                        "QMenu::item:selected {background-color: rgb(5, 40, 45);}");
//    QIcon icon;
//    icon.addFile(QString::fromUtf8(":/16x16/icons/16x16/cil-cloud-download.png"), QSize(), QIcon::Normal, QIcon::Off);
//    addpreset->setIcon(icon);
    ui_selectpreset->actionAdd_preset->setMenu(menu);
}

SelectPreset::~SelectPreset()
{
    delete ui_selectpreset;
}

void SelectPreset::add_section()  //******************************************* Add section *****************//
{
    QColor color;
    color.setRgba(qRgb(10, 180, 255));
    QFont font;
    font.setBold(true);
    QTreeWidgetItem *root = new QTreeWidgetItem();
    root->setText(0, "New section");
    ui_selectpreset->treeWidget->addTopLevelItem(root);
    ui_selectpreset->treeWidget->setCurrentItem(root);
    root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    root->setFont(0, font);
    root->setForeground(0, color);
}

void SelectPreset::add_preset()  //******************************************** Add preset ******************//
{
    int index = ui_selectpreset->treeWidget->currentIndex().row();
    if (index < 0) {
        std::cout << "Negative index..." << std::endl;
        return;
    };
    QColor color_child;
    color_child.setRgba(qRgb(210, 210, 210));
    QFont font_1;
    font_1.setLetterSpacing(QFont::PercentageSpacing, 99);
    QTreeWidgetItem *item = ui_selectpreset->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    QTreeWidgetItem *child = new QTreeWidgetItem();
    if (parentItem != nullptr) {
        // Item is child...
        child->setText(0, "New preset");
        child->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        child->setFont(0, font_1);
        child->setForeground(0, color_child);
        parentItem->addChild(child);
    } else {
        // Item is parent...
        child->setText(0, "New preset");
        child->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
        child->setFont(0, font_1);
        child->setForeground(0, color_child);
        item->addChild(child);
        ui_selectpreset->treeWidget->expandItem(item);
    };
}

void SelectPreset::on_actionRemove_preset_clicked()  //*********************** Remove preset ****************//
{
    int index = ui_selectpreset->treeWidget->currentIndex().row();
    if (index < 0) {
        std::cout << "Negative index..." << std::endl;
        return;
    };
    QTreeWidgetItem *item = ui_selectpreset->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem != nullptr) {
        // Item is child...
        QMessageBox msgBox;
        msgBox.setStyleSheet("background-color: rgb(5, 30, 35);");
        msgBox.setIcon(QMessageBox::Question);
        msgBox.setWindowTitle("Preset");
        msgBox.setText("Delete?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int confirm = msgBox.exec();
        if (confirm == QMessageBox::Yes) {
            int index_child = parentItem->indexOfChild(item);
            QTreeWidgetItem *takenItem = parentItem->takeChild(index_child);
            Q_ASSERT(takenItem==item);
            delete takenItem;
        };
    } else {
        // Item is parent...
        int count_child = item->childCount();
        if (count_child == 0) {
            int index_top = ui_selectpreset->treeWidget->indexOfTopLevelItem(item);
            QTreeWidgetItem *takenItem = ui_selectpreset->treeWidget->takeTopLevelItem(index_top);
            Q_ASSERT(takenItem==item);
            delete takenItem;
        } else {
            QMessageBox msgBox;
            msgBox.setStyleSheet("background-color: rgb(5, 30, 35);");
            msgBox.setIcon(QMessageBox::Information);
            msgBox.setWindowTitle("Preset");
            msgBox.setText("Delete presets first! ");
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.exec();
        };
    };
}

void SelectPreset::on_actionEdit_preset_clicked()  //************************** Edit preset *****************//
{
    int index = ui_selectpreset->treeWidget->currentIndex().row();
    if (index < 0) {
        std::cout << "Negative index..." << std::endl;
        QMessageBox msgBox;
        msgBox.setStyleSheet("background-color: rgb(5, 30, 35);");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("Preset");
        msgBox.setText("Select preset first! ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    };
    QTreeWidgetItem *item = ui_selectpreset->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem != nullptr) {
        // Item is child...
        _cur_preset_name = item->text(0);
        p1 = (item->text(1)).toInt();
        p2 = (item->text(2)).toInt();
        p3 = (item->text(3)).toInt();
        p4 = item->text(4);
        p5 = item->text(5);
        p6 = item->text(6);
        p7 = bool((item->text(7)).toInt());
        p8 = (item->text(8)).toInt();
        p9 = item->text(9);
        p10 = item->text(10);
        p11 = (item->text(11)).toInt();
        p12 = (item->text(12)).toInt();
        p13 = (item->text(13)).toInt();
        p14 = item->text(14);
        p15 = item->text(15);
        p16 = item->text(16);
        p17 = item->text(17);
        p18 = (item->text(18)).toInt();
        p19 = item->text(19);
        p20 = item->text(20);
        p21 = (item->text(21)).toInt();
        p22 = (item->text(22)).toInt();
        Preset preset;
        preset.setModal(true);
        preset.exec();  //******************************** Go to Preset and wait for return ***************************//
        item->setText(0, _cur_preset_name);
        item->setText(1, QString::number(p1));
        item->setText(2, QString::number(p2));
        item->setText(3, QString::number(p3));
        item->setText(4, p4);
        item->setText(5, p5);
        item->setText(6, p6);
        item->setText(7, QString::number(p7));
        item->setText(8, QString::number(p8));
        item->setText(9, p9);
        item->setText(10, p10);
        item->setText(11, QString::number(p11));
        item->setText(12, QString::number(p12));
        item->setText(13, QString::number(p13));
        item->setText(14, p14);
        item->setText(15, p15);
        item->setText(16, p16);
        item->setText(17, p17);
        item->setText(18, QString::number(p18));
        item->setText(19, p19);
        item->setText(20, p20);
        item->setText(21, QString::number(p21));
        item->setText(22, QString::number(p22));

    } else {
        // Item is parent...
        QMessageBox msgBox;
        msgBox.setStyleSheet("background-color: rgb(5, 30, 35);");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("Preset");
        msgBox.setText("Select preset first! ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    };
}

void SelectPreset::on_pushButton_7_clicked()  //*********************** Close select preset window **********//
{
    this->close();
}

void SelectPreset::on_pushButton_6_clicked()  //****************************** Apply preset *****************//
{
    int index = ui_selectpreset->treeWidget->currentIndex().row();
    if (index < 0) {
        std::cout << "Negative index..." << std::endl;
        QMessageBox msgBox;
        msgBox.setStyleSheet("background-color: rgb(5, 30, 35);");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("Preset");
        msgBox.setText("Select preset first! ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    };
    QTreeWidgetItem *item = ui_selectpreset->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem != nullptr) {
        // Item is child...
        _cur_preset_name = item->text(0);
        p1 = (item->text(1)).toInt();
        p2 = (item->text(2)).toInt();
        p3 = (item->text(3)).toInt();
        p4 = item->text(4);
        p5 = item->text(5);
        p6 = item->text(6);
        p7 = bool((item->text(7)).toInt());
        p8 = (item->text(8)).toInt();
        p9 = item->text(9);
        p10 = item->text(10);
        p11 = (item->text(11)).toInt();
        p12 = (item->text(12)).toInt();
        p13 = (item->text(13)).toInt();
        p14 = item->text(14);
        p15 = item->text(15);
        p16 = item->text(16);
        p17 = item->text(17);
        p18 = (item->text(18)).toInt();
        p19 = item->text(19);
        p20 = item->text(20);
        p21 = (item->text(21)).toInt();
        p22 = (item->text(22)).toInt();

    } else {
        // Item is parent...
        QMessageBox msgBox;
        msgBox.setStyleSheet("background-color: rgb(5, 30, 35);");
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setWindowTitle("Preset");
        msgBox.setText("Select preset first! ");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
        return;
    };

    int n1 = ui_selectpreset->treeWidget->topLevelItemCount();
    int n2 = 0;
    for(int i = 0; i < n1; i++) {
        n2 += ui_selectpreset->treeWidget->topLevelItem(i)->childCount();
    };
    int n = n1 + n2;  // Count of all rows...
    for (int i = 0; i < 24; i++) {
      _preset_table[i].resize(n);
    };
    int p = 0;
    for (int i = 0; i < n1; i++) {
        _preset_table[0][p] = ui_selectpreset->treeWidget->topLevelItem(i)->text(0);
        _preset_table[23][p] = "TopLewelItem";
        n2 = ui_selectpreset->treeWidget->topLevelItem(i)->childCount();
        for (int j = 0; j < n2; j++) {
            p++;
            for (int k = 0; k < 23; k++) {
                _preset_table[k][p] = ui_selectpreset->treeWidget->topLevelItem(i)->child(j)->text(k);
            };
            _preset_table[23][p] = "ChildItem";
        };
        p++;
    };
    std::cout << _preset_table[0].size() << " x " << _preset_table.size() << std::endl; // Table size
    pos_top = ui_selectpreset->treeWidget->indexOfTopLevelItem(parentItem);
    pos_cld = parentItem->indexOfChild(item);
    std::cout << "Pos_top: " << pos_top << std::endl;  // Current section pos
    std::cout << "Pos_cld: " << pos_cld << std::endl;  // Current preset pos
    this->close();
}

