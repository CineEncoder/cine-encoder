#include "donate.h"
#include "ui_donate.h"



Donate::Donate(QWidget *parent) :
    QDialog(parent),
    ui_donate(new Ui::Donate)
{
    ui_donate->setupUi(this);
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::SubWindow);
    this->setMouseTracking(true);
}

Donate::~Donate()
{
    delete ui_donate;
}

void Donate::on_toolButton_6_clicked()   // Close window
{
    this->close();
}

void Donate::on_closeWindow_clicked()
{
    this->close();
}

void Donate::setParameters()
{
    ui_donate->frame_hint->installEventFilter(this);
    mouseClickCoordinate.setX(0);
    mouseClickCoordinate.setY(0);
    QFile file;
    file.setFileName(":/resources/html/donate.html");
    file.open(QFile::ReadOnly);
    QString text = file.readAll();
    ui_donate->textBrowser->setHtml(text);
    ui_donate->textBrowser->setOpenExternalLinks(true);
}

void Donate::on_toolButton_7_clicked()   // Open browser PayPal
{
    QDesktopServices::openUrl(QUrl("https://paypal.me/KozhukharenkoOleg?locale.x=ru_RU", QUrl::TolerantMode));
}

void Donate::on_toolButton_8_clicked()   // Open browser Bitcoin
{
    QDesktopServices::openUrl(QUrl("https://blockchain.com/btc/payment_request?address=14ukiWCK2f8vFNPP3qKbo2vfaSGRMN63qL&amount=0.00005448&message=Cine Encoder donation", QUrl::TolerantMode));
}

bool Donate::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* mouse_event = dynamic_cast<QMouseEvent*>(event);
        if (mouse_event->button() == Qt::LeftButton)
        {
            clickPressedFlag = false;
            return QDialog::eventFilter(watched, event);
        }
    }

    if (watched == ui_donate->frame_hint)
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
                this->move(mouse_event->globalPos() - mouseClickCoordinate);
                return QDialog::eventFilter(watched, event);
            }
        }
    }
    return QDialog::eventFilter(watched, event);
}

