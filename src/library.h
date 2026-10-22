#pragma once
#include <atomic>

#include <QDateTime>
#include <QMetaType>
#include <QObject>
#include <QSet>
#include <QString>
#include <QStringList>
#include <QVector>
#include <functional>

// Bir fotoğrafın temel bilgileri.
struct Photo {
    QString path;    // tam dosya yolu
    QString root;    // kaynak klasör (kullanıcının seçtiği klasör)
    QString folder;  // dosyanın bulunduğu dizin
    QString name;    // dosya adı
    qint64 size = 0;
    QDateTime dt;
    int width = 0;
    int height = 0;

    double aspect() const
    {
        return height > 0 ? double(width) / double(height) : 1.0;
    }
};

Q_DECLARE_METATYPE(Photo)
Q_DECLARE_METATYPE(QVector<Photo>)

namespace library {

// Tarih grupları ("July 2026" gibi).
struct Group {
    QString label;
    QString subtitle;
    QVector<Photo> photos;
};

bool isImageFile(const QString& path);

// Seçilen klasörleri (ve alt klasörlerini) tarar.
QVector<Photo> scanFolders(const QStringList& folders,
                           const std::function<void(const QString&)>& progress = {});

// mode: "years" | "months" | "days"
QVector<Group> groupPhotos(const QVector<Photo>& photos, const QString& mode);

// kind: library | photos | videos | favorites | deleted | source | album
//       | util:live | util:panoramas | util:screenshots | util:360
QVector<Photo> applyFilter(const QVector<Photo>& photos, const QString& kind,
                           const QString& value, const QSet<QString>& favorites,
                           const QSet<QString>& deleted);

} // namespace library

// Arka planda tarama yapan tarayıcı (arayüzü kilitlemez).
class LibraryScanner : public QObject
{
    Q_OBJECT
public:
    explicit LibraryScanner(QObject* parent = nullptr);
    void scan(const QStringList& folders);

signals:
    void progress(const QString& text);
    void finished(const QVector<Photo>& photos);

private:
    std::atomic<int> token_{0};
};