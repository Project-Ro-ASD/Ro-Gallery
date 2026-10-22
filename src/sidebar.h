#pragma once
#include <QFrame>
#include <QImage>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVector>

#include "library.h"
#include "thumbs.h"

// Kenar çubuğundaki albüm kapağı.
class AlbumCover : public QWidget
{
    Q_OBJECT
public:
    AlbumCover(const QString& name, const QString& folder, QWidget* parent = nullptr);
    void setCoverImage(const QImage& img);
    QString folder() const { return folder_; }

signals:
    void clicked(const QString& folder);

protected:
    void paintEvent(QPaintEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;

private:
    QString name_;
    QString folder_;
    QImage cover_;
};

// Uygulamanın sol kenar çubuğu: gezinme, kaynaklar, araçlar, albümler.
class Sidebar : public QFrame
{
    Q_OBJECT
public:
    struct AlbumInfo {
        QString name;
        QString folder;
        Photo cover;
        bool hasCover = false;
    };

    explicit Sidebar(ThumbCache* thumbs, QWidget* parent = nullptr);

    void setSources(const QStringList& folders);
    void setAlbums(const QVector<AlbumInfo>& albums);
    void setActive(const QString& kind, const QString& value = QString());

signals:
    void filterSelected(const QString& kind, const QString& value);
    void addFolderRequested();
    void removeFolderRequested(const QString& path);
    void openFolderRequested(const QString& path);

private:
    struct NavItem {
        QPushButton* btn;
        QString kind;
        QString value;
    };

    QPushButton* addNav(QBoxLayout* layout, const QString& iconName, const QString& text,
                        const QString& kind, const QString& value = QString());
    void addSectionHeader(QBoxLayout* layout, const QString& title, const QString& buttonIcon);
    void refreshActive();
    void onThumbUpdated(const QString& path, int key);

    ThumbCache* thumbs_ = nullptr;
    QVector<NavItem> navs_;
    QVBoxLayout* sourcesBox_ = nullptr;
    QVBoxLayout* albumsBox_ = nullptr;
    QString activeKind_ = QStringLiteral("library");
    QString activeValue_;
    QVector<AlbumCover*> covers_;
    QVector<Photo> coverPhotos_;
};