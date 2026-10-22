#pragma once
#include <QHash>
#include <QImage>
#include <QObject>
#include <QQueue>
#include <QRunnable>
#include <QSet>
#include <QString>

#include "library.h"

// Küçük resim önbelleği: bellekte LRU + disk önbelleği, iş parçacığında yükler.
// key > 0  → o yükseklikte küçük resim
// key == 0 → tam resim (görüntüleyici için, uzun kenar en fazla 4096px)
class ThumbCache : public QObject
{
    Q_OBJECT
public:
    explicit ThumbCache(QObject* parent = nullptr);

    QImage get(const QString& path, qint64 size, int key) const;
    bool has(const QString& path, qint64 size, int key) const;
    void request(const Photo& photo, int key);
    void requestFull(const QString& path);

    static QString diskPath(const QString& path, qint64 size, int key);

signals:
    void updated(const QString& path, int key);
    void fullReady(const QString& path, const QImage& image);

private:
    friend class ThumbJob;
    void store(const QString& key, const QImage& img);
    QString memKey(const QString& path, qint64 size, int key) const;

    QHash<QString, QImage> mem_;
    QQueue<QString> order_;
    QSet<QString> inflight_;
    qint64 memBytes_ = 0;
};

// İş parçacığında çalışan yükleme görevi.
class ThumbJob : public QObject, public QRunnable
{
    Q_OBJECT
public:
    ThumbJob(const QString& path, qint64 size, int key);
    void run() override;

signals:
    void done(const QString& path, const QString& memKey, int key, const QImage& image);

private:
    QString path_;
    qint64 size_;
    int key_;
    QString memKey_;
};