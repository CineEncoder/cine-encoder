#include "fileiconprovider.h"


FileIconProvider::FileIconProvider()
{

}

FileIconProvider::~FileIconProvider()
{

}

QIcon FileIconProvider::icon(const QFileInfo &info) const
{
    QString path(":/resources/icons/svg/sys_folder.svg");
    if (info.baseName() == QObject::tr("Computer")) {
        path = ":/resources/icons/svg/computer.svg";
    } else
    if (info.baseName() == QObject::tr("Desktop")) {
        path = ":/resources/icons/svg/desktop.svg";
    } else
    if (info.baseName() == QObject::tr("Drive")) {
        path = ":/resources/icons/svg/drive.svg";
    } else
    if (info.isFile()) {
        path = ":/resources/icons/svg/file.svg";
    } else
    if (info.baseName() == QObject::tr("Network")) {
        path = ":/resources/icons/svg/network.svg";
    } else
    if (info.baseName() == QObject::tr("Trashcan")) {

    }
    return QIcon(QPixmap(path));
}

QIcon FileIconProvider::icon(const QFileIconProvider::IconType ic) const
{
    QString path(":/resources/icons/svg/sys_folder.svg");
    switch (ic) {
    case QFileIconProvider::IconType::Computer:
        path = ":/resources/icons/svg/computer.svg";
        break;
    case QFileIconProvider::IconType::Desktop:
        path = ":/resources/icons/svg/desktop.svg";
        break;
    case QFileIconProvider::IconType::Drive:
        path = ":/resources/icons/svg/drive.svg";
        break;
    case QFileIconProvider::IconType::File:
        path = ":/resources/icons/svg/file.svg";
        break;
    case QFileIconProvider::IconType::Network:
        path = ":/resources/icons/svg/network.svg";
        break;
    case QFileIconProvider::IconType::Trashcan:

        break;
    default:
        break;
    }
    return QIcon(QPixmap(path));
}
