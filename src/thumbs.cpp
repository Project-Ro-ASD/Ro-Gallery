#include "thumbs.h"

#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QImageReader>
#include <QThreadPool>

namespace {

QString cacheRoot()
{
    const QByteArray xdg = qgetenv("XDG_CACHE_HOME");
    const QString base = xdg.isEmpty() ? QDir::homePath() + QStringLiteral("/.cache")
                                       : QString::fromLocal8Bit(xdg);
    return base + QStringLiteral("/ro-gallery/thumbs");
}

const int kMemLimit = 400;            // en fazla küçük resim sayısı
const qint64 kByteBudget = 64 * 1024 * 1024; // bellek bütçesi (64 MB)

} // namespace

ThumbCache::ThumbCache(QObject* parent)
    : QObject(parent)
{
    QDir().mkpath(cacheRoot());
}

QString ThumbCache::memKey(const QString& path, qint64 size, int key) const
{
    return QStringLiteral("%1|%2|%3").arg(path).arg(size).arg(key);
}

QString ThumbCache::diskPath(const QString& path, qint64 size, int key)
{
    const QByteArray hash =
        QCryptographicHash::hash(QStringLiteral("%1|%2|%3").arg(path).arg(size).arg(key).toUtf8(),
                                 QCryptographicHash::Md5)
            .toHex();
    return cacheRoot() + QLatin1Char('/') + QString::fromLatin1(hash) + QStringLiteral(".jpg");
}

QImage ThumbCache::get(const QString& path, qint64 size, int key) const
{
    return mem_.value(memKey(path, size, key));
}

bool ThumbCache::has(const QString& path, qint64 size, int key) const
{
    return mem_.contains(memKey(path, size, key));
}

void ThumbCache::store(const QString& key, const QImage& img)
{
    if (mem_.contains(key)) {
        memBytes_ -= mem_.value(key).sizeInBytes();
        mem_[key] = img;
        memBytes_ += img.sizeInBytes();
        return;
    }
    mem_.insert(key, img);
    memBytes_ += img.sizeInBytes();
    order_.enqueue(key);
    while (order_.size() > kMemLimit || memBytes_ > kByteBudget) {
        if (order_.isEmpty())
            break;
        const QString old = order_.dequeue();
        memBytes_ -= mem_.value(old).sizeInBytes();
        mem_.remove(old);
    }
}

void ThumbCache::request(const Photo& photo, int key)
{
    const QString mKey = memKey(photo.path, photo.size, key);
    if (mem_.contains(mKey) || inflight_.contains(mKey))
        return;
    inflight_.insert(mKey);
    ThumbJob* job = new ThumbJob(photo.path, photo.size, key);
    connect(job, &ThumbJob::done, this,
            [this](const QString& path, const QString& mKey, int key, const QImage& image) {
                inflight_.remove(mKey);
                if (!image.isNull())
                    store(mKey, image);
                emit updated(path, key);
            },
            Qt::QueuedConnection);
    QThreadPool::globalInstance()->start(job);
}

void ThumbCache::requestFull(const QString& path)
{
    ThumbJob* job = new ThumbJob(path, 0, 0);
    connect(job, &ThumbJob::done, this,
            [this](const QString& path, const QString&, int, const QImage& image) {
                if (!image.isNull())
                    emit fullReady(path, image);
            },
            Qt::QueuedConnection);
    QThreadPool::globalInstance()->start(job);
}

// ---------------------------------------------------------------- ThumbJob

ThumbJob::ThumbJob(const QString& path, qint64 size, int key)
    : path_(path), size_(size), key_(key)
{
    memKey_ = QStringLiteral("%1|%2|%3").arg(path).arg(size).arg(key);
    setAutoDelete(true);
}

void ThumbJob::run()
{
    QImage img;

    // 1) disk önbelleği (yalnız küçük resimler)
    if (key_ > 0) {
        const QString disk = ThumbCache::diskPath(path_, size_, key_);
        if (QFile::exists(disk)) {
            img.load(disk);
            if (!img.isNull()) {
                emit done(path_, memKey_, key_, img);
                return;
            }
        }
    }

    // 2) çöz
    QImageReader reader(path_);
    reader.setAutoTransform(true);
    const QSize orig = reader.size();
    if (key_ > 0 && orig.isValid() && orig.height() > 0) {
        QSize target = orig;
        const double scale = double(key_) / double(orig.height());
        target.setWidth(qMax(1, int(orig.width() * scale)));
        target.setHeight(qMax(1, key_));
        reader.setScaledSize(target);
    }
    img = reader.read();

    if (!img.isNull()) {
        if (key_ == 0) {
            // görüntüleyici için boyut sınırı (bellek tasarrufu)
            if (img.width() > 4096 || img.height() > 4096)
                img = img.scaled(4096, 4096, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
            const QString disk = ThumbCache::diskPath(path_, size_, key_);
            img.save(disk, "JPG", 82);
        }
    }

    emit done(path_, memKey_, key_, img);
}