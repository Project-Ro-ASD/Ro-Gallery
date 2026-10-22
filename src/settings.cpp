#include "settings.h"

#include <QDir>

Settings::Settings()
    : s_(QSettings::IniFormat, QSettings::UserScope,
         QStringLiteral("Ro-ASD"), QStringLiteral("ro-gallery"))
{
    loadSets();
}

void Settings::loadSets()
{
    const QStringList fav = s_.value(QStringLiteral("photos/favorites")).toStringList();
    const QStringList del = s_.value(QStringLiteral("photos/deleted")).toStringList();
    fav_ = QSet<QString>(fav.begin(), fav.end());
    del_ = QSet<QString>(del.begin(), del.end());
}

void Settings::saveSets()
{
    const QStringList fav(fav_.begin(), fav_.end());
    const QStringList del(del_.begin(), del_.end());
    s_.setValue(QStringLiteral("photos/favorites"), fav);
    s_.setValue(QStringLiteral("photos/deleted"), del);
}

QStringList Settings::folders() const
{
    QStringList f = s_.value(QStringLiteral("library/folders")).toStringList();
    f.removeAll(QString());
    return f;
}

void Settings::setFolders(const QStringList& folders)
{
    s_.setValue(QStringLiteral("library/folders"), folders);
}

void Settings::addFolder(const QString& path)
{
    QStringList f = folders();
    const QString clean = QDir::cleanPath(path);
    if (!f.contains(clean)) {
        f.append(clean);
        setFolders(f);
    }
}

void Settings::removeFolder(const QString& path)
{
    QStringList f = folders();
    f.removeAll(QDir::cleanPath(path));
    setFolders(f);
}

bool Settings::isFavorite(const QString& path) const
{
    return fav_.contains(path);
}

void Settings::toggleFavorite(const QString& path)
{
    if (fav_.contains(path))
        fav_.remove(path);
    else
        fav_.insert(path);
    saveSets();
}

bool Settings::isDeleted(const QString& path) const
{
    return del_.contains(path);
}

void Settings::softDelete(const QString& path)
{
    del_.insert(path);
    saveSets();
}

void Settings::restore(const QString& path)
{
    del_.remove(path);
    saveSets();
}

void Settings::emptyTrash()
{
    del_.clear();
    saveSets();
}

QStringList Settings::favorites() const
{
    const QStringList f(fav_.begin(), fav_.end());
    return f;
}

QStringList Settings::deleted() const
{
    const QStringList d(del_.begin(), del_.end());
    return d;
}

int Settings::rowHeight() const
{
    return s_.value(QStringLiteral("view/rowHeight"), 190).toInt();
}

void Settings::setRowHeight(int h)
{
    s_.setValue(QStringLiteral("view/rowHeight"), h);
}

QString Settings::grouping() const
{
    return s_.value(QStringLiteral("view/grouping"), QStringLiteral("months")).toString();
}

void Settings::setGrouping(const QString& g)
{
    s_.setValue(QStringLiteral("view/grouping"), g);
}

QByteArray Settings::geometry() const
{
    return s_.value(QStringLiteral("window/geometry")).toByteArray();
}

void Settings::setGeometry(const QByteArray& g)
{
    s_.setValue(QStringLiteral("window/geometry"), g);
}