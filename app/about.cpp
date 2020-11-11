#include <QDesktopServices>
#include <QUrl>
#include "about.h"
#include "ui_about.h"


About::About(QWidget *parent) :
    QDialog(parent),
    ui_about(new Ui::About)
{
    ui_about->setupUi(this);
}

About::~About()
{
    delete ui_about;
}


void About::on_toolButton_6_clicked()   // Close window
{
    this->close();
}

void About::setParameters()
{
    ui_about->label_11->setText("<a href=\"https://github.com/CineEncoder/cine-encoder\">https://github.com/CineEncoder/cine-encoder</a>");
    ui_about->label_11->setOpenExternalLinks(true);
    ui_about->label_12->setText("<a href=\"https://github.com/CineEncoder/cine-encoder/blob/master/LICENSE\">License: GNU Lesser General Public License Version 3 </a>");
    ui_about->label_12->setOpenExternalLinks(true);
}

void About::on_toolButton_7_clicked()   // Open browser PayPal
{
    QDesktopServices::openUrl(QUrl("https://paypal.me/KozhukharenkoOleg?locale.x=ru_RU", QUrl::TolerantMode));
}

void About::on_toolButton_8_clicked()   // Open browser Bitcoin
{
    QDesktopServices::openUrl(QUrl("https://blockchain.com/btc/payment_request?address=14ukiWCK2f8vFNPP3qKbo2vfaSGRMN63qL&amount=0.00005448&message=Cine Encoder donation", QUrl::TolerantMode));
}
