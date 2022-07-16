#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include <QString>
#include <QVector>


class Helper : public QObject
{
public:
    explicit Helper();
    ~Helper();

    enum class FileDialogType : uint8_t {
        OPENVFILES, OPENAFILES, OPENSFILES, SELECTFOLDER
    };

    enum class DesktopEnv : uint8_t {
        GNOME, OTHER, UNDEF
    };

    static void detectEnv();
    static DesktopEnv getEnv();
    static QString getParsedCss(const QString &list);
    static void openFileDialog(FileDialogType dialogType,
                               const QString  &title,
                               const QString  &path,
                               QStringList    &result);
    static QString timeConverter(const float time);
    static QString timeConverter(const double time);
    static QString getSysLanguage();
    static QString elideText(QWidget *w,
                             const QString &text,
                             const Qt::TextElideMode elide);
    static QString recalcChannels(const QString &ch);
    static bool isSupported(const QString &format);
    template<class T>
    static void eraseRow(QVector<T> &v, const int row);
    template<class T>
    static void reorder(QVector<T> &v, QVector<int> const &order);

private:
    static DesktopEnv m_desktopEnv;
};

template<class T>
void Helper::eraseRow(QVector<T> &v, const int row)
{
    auto it = v.begin();
    std::advance(it, row);
    v.erase(it);
}

template<class T>
void Helper::reorder(QVector<T> &v, QVector<int> const &order)
{
    for (int s = 1, d; s < order.size(); ++s) {
        for (d = order[s]; d < s; d = order[d]);
        if (d == s)
            while (d = order[d], d != s)
                std::swap(v[s], v[d]);

    }
}

#endif // HELPER_H
