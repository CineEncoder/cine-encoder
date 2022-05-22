#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include <QString>

enum class DesktopEnv : uint8_t {
    GNOME, OTHER, UNDEF
};

enum class FileDialogType : uint8_t {
    OPENVFILES, SELECTFOLDER
};

class Helper : public QObject
{
public:
    explicit Helper();
    ~Helper();

    static void detectEnv();
    static DesktopEnv getEnv();
    static QString getParsedCss(const QString &list);
    static void openFileDialog(FileDialogType dialogType,
                               const QString  &title,
                               const QString  &path,
                               QStringList    &result);
    static QString timeConverter(const float time);
    static QString timeConverter(const double time);

private:
    static DesktopEnv m_desktopEnv;
};

#endif // HELPER_H
