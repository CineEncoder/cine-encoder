#include "preset.h"
#include "selectpreset.h"
#include "ui_selectpreset.h"
#include "taskcomplete.h"
#include "dialog.h"



SelectPreset::SelectPreset(QWidget *parent) :
    QDialog(parent),
    ui_selectpreset(new Ui::SelectPreset)
{
    ui_selectpreset->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);
}

SelectPreset::~SelectPreset()
{
    delete ui_selectpreset;
}

void SelectPreset::on_pushButton_7_clicked()  //*********************** Close select preset window ***********//
{
    this->close();
}

void SelectPreset::on_closeWindow_2_clicked()
{
    this->close();
}

void SelectPreset::on_expandWindow_2_clicked()
{
    if (_expandWindowsState == false)
    {
        this->showMaximized();
        _expandWindowsState = true;
        QIcon iconShowMax;
        iconShowMax.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-clone.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui_selectpreset->expandWindow_2->setIcon(iconShowMax);
    } else {
        this->showNormal();
        _expandWindowsState = false;
        QIcon iconShowNorm;
        iconShowNorm.addFile(QString::fromUtf8(":/resources/icons/16x16/cil-media-stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        ui_selectpreset->expandWindow_2->setIcon(iconShowNorm);
    }
}

void SelectPreset::on_hideWindow_2_clicked()
{
    this->showMinimized();
}

void SelectPreset::setParameters(int *ptr_pos_top, int *ptr_pos_cld)    //***** Set paremeters ***************//
{
    this->resize(950, 600);
    ui_selectpreset->frame_hint->installEventFilter(this);
    ui_selectpreset->widget->installEventFilter(this);
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    _ptr_pos_top = ptr_pos_top;
    _ptr_pos_cld = ptr_pos_cld;

    ui_selectpreset->treeWidget->clear();
    int n = _preset_table[0].size();
    int m = _preset_table.size();
    QString type;
    //QColor color;
    QColor color_child;
    //color.setRgba(qRgb(10, 180, 255));
    color_child.setRgba(qRgb(150, 150, 150));
    QFont font, font_1;
    font.setBold(true);
    font_1.setLetterSpacing(QFont::PercentageSpacing, 99);
    //font_1.setItalic(true);
    for (int i = 0; i < n; i++) {
        type = _preset_table[23][i];
        if (type == "TopLewelItem") {
            QTreeWidgetItem *root = new QTreeWidgetItem();
            root->setText(0, _preset_table[0][i]);
            ui_selectpreset->treeWidget->addTopLevelItem(root);
            ui_selectpreset->treeWidget->setCurrentItem(root);
            root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
            root->setFont(0, font);
            //root->setForeground(0, color);
        }
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
        }
    }
    QTreeWidgetItem *item = ui_selectpreset->treeWidget->topLevelItem(*_ptr_pos_top)->child(*_ptr_pos_cld);
    ui_selectpreset->treeWidget->setCurrentItem(item);
    std::cout << n << " x " << m << std::endl; // Table size
    for (int i=1; i<=24; i++) {
        ui_selectpreset->treeWidget->hideColumn(i);
    }

    // ***************************** Top menu actions ***********************************//

    QAction *addsection = new QAction(tr("Add section"), this);
    QAction *addpreset = new QAction(tr("Add new preset"), this);
    connect(addsection, &QAction::triggered, this, &SelectPreset::add_section);
    connect(addpreset, &QAction::triggered, this, &SelectPreset::add_preset);
    QMenu* menu = new QMenu(this);
    menu->addAction(addsection);
    menu->addAction(addpreset);
    // QIcon icon;
    // icon.addFile(QString::fromUtf8(":/16x16/icons/16x16/cil-cloud-download.png"), QSize(), QIcon::Normal, QIcon::Off);
    // addpreset->setIcon(icon);
    ui_selectpreset->actionAdd_preset->setMenu(menu);
}

void SelectPreset::add_section()  //******************************************** Add section *****************//
{
    //QColor color;
    //color.setRgba(qRgb(10, 180, 255));
    QFont font;
    font.setBold(true);
    QTreeWidgetItem *root = new QTreeWidgetItem();
    root->setText(0, "New section");
    ui_selectpreset->treeWidget->addTopLevelItem(root);
    ui_selectpreset->treeWidget->setCurrentItem(root);
    root->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    root->setFont(0, font);
    //root->setForeground(0, color);
}

void SelectPreset::add_preset()  //********************************************* Add preset ******************//
{
    int index = ui_selectpreset->treeWidget->currentIndex().row();
    if (index < 0) {
        std::cout << "Negative index..." << std::endl;
        return;
    };
    QColor color_child;
    color_child.setRgba(qRgb(150, 150, 150));
    QFont font_1;
    font_1.setLetterSpacing(QFont::PercentageSpacing, 99);
    //font_1.setItalic(true);
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
    }
}

void SelectPreset::on_actionRemove_preset_clicked()  //*********************** Remove preset *****************//
{
    int index = ui_selectpreset->treeWidget->currentIndex().row();
    if (index < 0) {
        std::cout << "Negative index..." << std::endl;
        return;
    }
    QTreeWidgetItem *item = ui_selectpreset->treeWidget->currentItem();
    QTreeWidgetItem *parentItem = item->parent();
    if (parentItem != nullptr) {
        // Item is child...
        _message = "Delete?";
        bool confirm = call_dialog(_message);
        if (confirm == true)
        {
            int index_child = parentItem->indexOfChild(item);
            QTreeWidgetItem *takenItem = parentItem->takeChild(index_child);
            Q_ASSERT(takenItem==item);
            delete takenItem;
        }
    } else {
        // Item is parent...
        int count_child = item->childCount();
        if (count_child == 0) {
            int index_top = ui_selectpreset->treeWidget->indexOfTopLevelItem(item);
            QTreeWidgetItem *takenItem = ui_selectpreset->treeWidget->takeTopLevelItem(index_top);
            Q_ASSERT(takenItem==item);
            delete takenItem;
        } else {
            _message = "Delete presets first!\n";
            call_task_complete(_message, false);
        }
    }
}

void SelectPreset::on_actionEdit_preset_clicked()  //************************** Edit preset ******************//
{
    int index = ui_selectpreset->treeWidget->currentIndex().row();
    if (index < 0) {
        std::cout << "Negative index..." << std::endl;
        _message = "Select preset first!\n";
        call_task_complete(_message, false);
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
        preset.setParameters();
        preset.setModal(true);
        preset.exec();  //******************************** Go to Preset and wait for return ***************************//
        for (int k = 0; k < 23; k++) {
            item->setText(k, _cur_param[k]);
        };

    } else {
        // Item is parent...
        _message = "Select preset first!\n";
        call_task_complete(_message, false);
    };
}

void SelectPreset::on_pushButton_6_clicked()  //****************************** Apply preset ******************//
{
    int index = ui_selectpreset->treeWidget->currentIndex().row();
    if (index < 0) {
        std::cout << "Negative index..." << std::endl;
        _message = "Select preset first!\n";
        call_task_complete(_message, false);
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
        _message = "Select preset first!\n";
        call_task_complete(_message, false);
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

bool SelectPreset::call_dialog(const QString &_message)  // Call dialog ******//
{
    bool acceptFlag = false;
    Dialog dialog(this);
    dialog.setMessage(_message, &acceptFlag);
    dialog.setModal(true);
    dialog.exec();
    return acceptFlag;
}

void SelectPreset::call_task_complete(const QString &_message, const bool &_timer_mode)   // Call task complete
{
    Taskcomplete taskcomplete(this);
    taskcomplete.setMessage(_message, _timer_mode);
    taskcomplete.setModal(true);
    taskcomplete.exec();
}

bool SelectPreset::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease) // *************** Reset ************************* //
    {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->button() == Qt::LeftButton)
        {
            QGuiApplication::restoreOverrideCursor();
            clickPressedFlag = false;
            clickPressed_Left_ResizeFlag = false;
            clickPressed_Left_Top_ResizeFlag = false;
            clickPressed_Top_ResizeFlag = false;
            clickPressed_Right_Top_ResizeFlag = false;
            clickPressed_Right_ResizeFlag = false;
            clickPressed_Right_Bottom_ResizeFlag = false;
            clickPressed_Bottom_ResizeFlag = false;
            clickPressed_Left_Bottom_ResizeFlag = false;
            return QDialog::eventFilter(watched, event);
        }
    }

    if (watched == ui_selectpreset->widget) // *************** Resize window realisation ************************* //
    {
        if (!this->isMaximized())
        {
            if (event->type() == QEvent::MouseButtonPress)
            {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->button() == Qt::LeftButton)
                {
                    _posX = this->pos().x();
                    _posY = this->pos().y();
                    _width = this->width();
                    _height = this->height();
                    mouseClickCoordinate = mouse_event->pos();
                    if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() > 62) && (mouseClickCoordinate.y() < (_height-12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                        clickPressed_Left_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() < 12))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                        clickPressed_Left_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > 12) && (mouseClickCoordinate.x() < (_width - 120)) && (mouseClickCoordinate.y() < 3))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                        clickPressed_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (_width - 12)) && (mouseClickCoordinate.y() < 12))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                        clickPressed_Right_Top_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (_width - 12)) && (mouseClickCoordinate.y() > 62) && (mouseClickCoordinate.y() < (_height-12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));
                        clickPressed_Right_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > (_width - 12)) && (mouseClickCoordinate.y() > (_height - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeFDiagCursor));
                        clickPressed_Right_Bottom_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() > 12) && (mouseClickCoordinate.x() < (_width - 12)) && (mouseClickCoordinate.y() > (_height - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeVerCursor));
                        clickPressed_Bottom_ResizeFlag = true;
                    }
                    else if ((mouseClickCoordinate.x() < 12) && (mouseClickCoordinate.y() > (_height - 12)))
                    {
                        QGuiApplication::setOverrideCursor(QCursor(Qt::SizeBDiagCursor));
                        clickPressed_Left_Bottom_ResizeFlag = true;
                    }
                    return QDialog::eventFilter(watched, event);
                }
            }
            else if (event->type() == QEvent::MouseMove)
            {
                QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
                if (mouse_event->buttons() & Qt::LeftButton)
                {
                    int deltaX = mouse_event->globalPos().x() - mouseClickCoordinate.x();
                    int deltaY = mouse_event->globalPos().y() - mouseClickCoordinate.y();
                    int deltaWidth = static_cast<int>(mouse_event->localPos().x()) - mouseClickCoordinate.x();
                    int deltaHeight = static_cast<int>(mouse_event->localPos().y()) - mouseClickCoordinate.y();
                    if (clickPressed_Left_ResizeFlag == true)
                    {
                        this->setGeometry(deltaX, _posY, this->width() - deltaWidth, _height);
                    }
                    else if (clickPressed_Left_Top_ResizeFlag == true)
                    {
                        this->setGeometry(deltaX, deltaY, this->width() - deltaWidth, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Top_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, deltaY, _width, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Right_Top_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, deltaY, _width + deltaWidth, this->height() - deltaHeight);
                    }
                    else if (clickPressed_Right_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, _posY, _width + deltaWidth, _height);
                    }
                    else if (clickPressed_Right_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, _posY, _width + deltaWidth, _height + deltaHeight);
                    }
                    else if (clickPressed_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(_posX, _posY, _width, _height + deltaHeight);
                    }
                    else if (clickPressed_Left_Bottom_ResizeFlag == true)
                    {
                        this->setGeometry(deltaX, _posY, this->width() - deltaWidth, _height + deltaHeight);
                    }
                    return QDialog::eventFilter(watched, event);
                }
            }
        }
    }
    else if (watched == ui_selectpreset->frame_hint) // *************** Drag window realisation ************************* //
    {
        if (event->type() == QEvent::MouseButtonPress)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->button() == Qt::LeftButton)
            {
                mouseClickCoordinate = mouse_event->pos();
                clickPressedFlag = true;
                return QDialog::eventFilter(watched, event);
            }
        }
        else if ((event->type() == QEvent::MouseMove) && clickPressedFlag == true)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                if (this->isMaximized())
                {
                    on_expandWindow_2_clicked();
                }
                this->move(mouse_event->globalPos() - mouseClickCoordinate);
                return QDialog::eventFilter(watched, event);
            }
        }
        else if (event->type() == QEvent::MouseButtonDblClick)
        {
            QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
            if (mouse_event->buttons() & Qt::LeftButton)
            {
                on_expandWindow_2_clicked();
                return QDialog::eventFilter(watched, event);
            }
        }
    }
    return QDialog::eventFilter(watched, event);
}

