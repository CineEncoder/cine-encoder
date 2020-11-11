#include <QtGlobal>
#include <QMenu>
#include <QMessageBox>
#include <iostream>
#include "preset.h"
#include "selectpreset.h"
#include "ui_selectpreset.h"


extern QVector <QVector <QString> > _preset_table;
extern QString _cur_param[23];

SelectPreset::SelectPreset(QWidget *parent) :
    QDialog(parent),
    ui_selectpreset(new Ui::SelectPreset)
{
    ui_selectpreset->setupUi(this);
}

SelectPreset::~SelectPreset()
{
    delete ui_selectpreset;
}

void SelectPreset::set_param(int *ptr_pos_top, int *ptr_pos_cld)
{
    _ptr_pos_top = ptr_pos_top;
    _ptr_pos_cld = ptr_pos_cld;

    ui_selectpreset->treeWidget->clear();
    int n = _preset_table[0].size();
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
    QTreeWidgetItem *item = ui_selectpreset->treeWidget->topLevelItem(*_ptr_pos_top)->child(*_ptr_pos_cld);
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
        QMessageBox msgBox(this);
        msgBox.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(5, 30, 35);");
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
            QMessageBox msgBox(this);
            msgBox.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(5, 30, 35);");
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
        QMessageBox msgBox(this);
        msgBox.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(5, 30, 35);");
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
        for (int k = 0; k < 23; k++) {
            _cur_param[k] = item->text(k);
        };
        Preset preset(this);
        preset.set_param();
        preset.setModal(true);
        preset.exec();  //******************************** Go to Preset and wait for return ***************************//
        for (int k = 0; k < 23; k++) {
            item->setText(k, _cur_param[k]);
        };

    } else {
        // Item is parent...
        QMessageBox msgBox(this);
        msgBox.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(5, 30, 35);");
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
        QMessageBox msgBox(this);
        msgBox.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(5, 30, 35);");
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
        for (int k = 0; k < 23; k++) {
            _cur_param[k] = item->text(k);
        };

    } else {
        // Item is parent...
        QMessageBox msgBox(this);
        msgBox.setStyleSheet("color: rgb(255, 255, 255); background-color: rgb(5, 30, 35);");
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
    *_ptr_pos_top = ui_selectpreset->treeWidget->indexOfTopLevelItem(parentItem);
    *_ptr_pos_cld = parentItem->indexOfChild(item);
    std::cout << "Pos_top: " << *_ptr_pos_top << std::endl;  // Current section pos
    std::cout << "Pos_cld: " << *_ptr_pos_cld << std::endl;  // Current preset pos
    this->close();
}

