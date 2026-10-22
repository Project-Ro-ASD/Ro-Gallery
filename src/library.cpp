#include "library.h"
#include "exifdate.h"

#include <QDateTime>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QImageReader>
#include <QSet>
#include <QThread>
#include <algorithm>

namespace library {

bool isImageFile(const QString& path)
{
    static const QSet<QString> exts = {
        QStringLiteral("jpg"),  QStringLiteral("jpeg"), QStringLiteral("png"),
        QStringLiteral("gif"),  QStringLiteral("bmp"),  QStringLiteral("webp"),
        QStringLiteral("tif"),  QStringLiteral("tiff"), QStringLiteral("heic"),
        QStringLiteral("heif"), QStringLiteral("avif"), QStringLiteral("jxl"),
        QStringLiteral("raw"),  QStringLiteral("dng"),  QStringLiteral("ico"),
        QStringLiteral("svg"),
    };
    const QString suffix = QFileInfo(path).suffix().toLower();
    return exts.contains(suffix);
}

QVector<Photo> scanFolders(const QStringList& folders,
                           const std::function<void(const QString&)>& progress)
{
    QVector<Photo> out;
    for (const QString& root : folders) {
        QDir rootDir(root);
        if (!rootDir.exists())
            continue;
        QDirIterator it(root, QDir::Files | QDir::Readable | QDir::NoDotAndDotDot,
                        QDirIterator::Subdirectories);
        while (it.hasNext()) {
            const QString path = it.next();
            // Gizli dizinleri (.cache vb.) atla
            if (path.contains(QLatin1String("/.")))
                continue;
            if (!isImageFile(path))
                continue;

            const QFileInfo fi = it.fileInfo();
            Photo p;
            p.path = path;
            p.root = root;
            p.folder = fi.absolutePath();
            p.name = fi.fileName();
            p.size = fi.size();
            p.dt = exifDate(path);
            if (!p.dt.isValid())
                p.dt = fi.lastModified();

            QImageReader reader(path);
            reader.setAutoTransform(true);
            const QSize s = reader.size();
            p.width = s.width() > 0 ? s.width() : 4;
            p.height = s.height() > 0 ? s.height() : 3;

            out.append(p);
            if (progress && (out.size() % 20 == 0))
                progress(QStringLiteral("%1 fotoğraf bulundu…").arg(out.size()));
        }
    }

    std::sort(out.begin(), out.end(), [](const Photo& a, const Photo& b) {
        if (a.dt != b.dt)
            return a.dt > b.dt;
        return a.path < b.path;
    });
    return out;
}

QVector<Group> groupPhotos(const QVector<Photo>& photos, const QString& mode)
{
    static const char* months[12] = {
        "January", "February", "March",     "April",   "May",      "June",
        "July",    "August",   "September", "October", "November", "December"};

    auto labelOf = [&](const QDate& d) -> QString {
        const QString mName = QString::fromLatin1(months[d.month() - 1]);
        if (mode == QLatin1String("years"))
            return QString::number(d.year());
        if (mode == QLatin1String("days"))
            return QStringLiteral("%1 %2, %3").arg(mName).arg(d.day()).arg(d.year());
        return QStringLiteral("%1 %2").arg(mName).arg(d.year());
    };

    QVector<Group> groups;
    for (const Photo& p : photos) {
        const QString label = labelOf(p.dt.date());
        if (groups.isEmpty() || groups.last().label != label) {
            Group g;
            g.label = label;
            groups.append(g);
        }
        groups.last().photos.append(p);
    }
    for (Group& g : groups) {
        const int n = g.photos.size();
        g.subtitle = QStringLiteral("%1 photo%2").arg(n).arg(n == 1 ? QString() : QStringLiteral("s"));
    }
    return groups;
}

QVector<Photo> applyFilter(const QVector<Photo>& photos, const QString& kind,
                           const QString& value, const QSet<QString>& favorites,
                           const QSet<QString>& deleted)
{
    QVector<Photo> out;
    out.reserve(photos.size());
    for (const Photo& p : photos) {
        const bool isDel = deleted.contains(p.path);
        if (kind == QLatin1String("deleted")) {
            if (isDel)
                out.append(p);
            continue;
        }
        if (isDel)
            continue;

        if (kind == QLatin1String("favorites")) {
            if (favorites.contains(p.path))
                out.append(p);
            continue;
        }
        // 2. adım: videolar ve bu araçlar şimdilik boş
        if (kind == QLatin1String("videos") || kind == QLatin1String("util:live")
            || kind == QLatin1String("util:360"))
            continue;

        if (kind == QLatin1String("source") || kind == QLatin1String("album")) {
            if (p.root == value)
                out.append(p);
            continue;
        }
        if (kind == QLatin1String("util:panoramas")) {
            if (p.aspect() >= 2.0)
                out.append(p);
            continue;
        }
        if (kind == QLatin1String("util:screenshots")) {
            const QString n = p.name.toLower();
            if (n.contains(QLatin1String("screenshot"))
                || n.contains(QLatin1String("ekran")))
                out.append(p);
            continue;
        }
        out.append(p); // library / photos
    }
    return out;
}

} // namespace library

LibraryScanner::LibraryScanner(QObject* parent)
    : QObject(parent)
{
}

void LibraryScanner::scan(const QStringList& folders)
{
    const int token = ++token_;
    QThread* t = QThread::create([this, folders, token]() {
        auto progressCb = [this, token](const QString& text) {
            if (token == token_.load())
                emit progress(text);
        };
        const QVector<Photo> photos = library::scanFolders(folders, progressCb);
        if (token == token_.load())
            emit finished(photos);
    });
    connect(t, &QThread::finished, t, &QThread::deleteLater);
    t->start();
}