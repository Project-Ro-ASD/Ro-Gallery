#pragma once
#include <QByteArray>
#include <QSet>
#include <QSettings>
#include <QString>
#include <QStringList>

// Kalıcı uygulama ayarları (~/.config/Ro-ASD/ro-gallery.ini).
class Settings
{
public:
    Settings();

    // Taranacak klasörler (home altındaki seçili klasörler)
    QStringList folders() const;
    void setFolders(const QStringList& folders);
    void addFolder(const QString& path);
    void removeFolder(const QString& path);

    // Favoriler / çöp kutusu (dosya yolları)
    bool isFavorite(const QString& path) const;
    void toggleFavorite(const QString& path);
    bool isDeleted(const QString& path) const;
    void softDelete(const QString& path);
    void restore(const QString& path);
    void emptyTrash();
    QStringList favorites() const;
    QStringList deleted() const;

    // Görünüm
    int rowHeight() const;
    void setRowHeight(int h);
    QString grouping() const;   // "years" | "months" | "days"
    void setGrouping(const QString& g);

    // Pencere
    QByteArray geometry() const;
    void setGeometry(const QByteArray& g);

private:
    QSettings s_;
    QSet<QString> fav_;
    QSet<QString> del_;
    void loadSets();
    void saveSets();
};