#pragma once
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVector>

#include "grid.h"
#include "library.h"
#include "settings.h"
#include "sidebar.h"
#include "thumbs.h"
#include "topbar.h"
#include "viewer.h"

// Ana içerik sayfası: yüzen araç çubuğu + fotoğraf ızgarası + durum/boş ekran.
class ContentPage : public QWidget
{
    Q_OBJECT
public:
    explicit ContentPage(ThumbCache* thumbs, QWidget* parent = nullptr);

    PhotoGrid* grid = nullptr;
    TopBar* topbar = nullptr;
    QScrollArea* scroll = nullptr;
    QLabel* status = nullptr;
    QWidget* empty = nullptr;

    void setEmptyState(const QString& title, const QString& text, bool showButton);
    void hideEmptyState();
    void setStatus(const QString& text, bool visible);

signals:
    void emptyActionRequested();

protected:
    void resizeEvent(QResizeEvent* e) override;

private:
    void positionOverlays();
    QLabel* emptyTitle_ = nullptr;
    QLabel* emptyText_ = nullptr;
    QPushButton* emptyButton_ = nullptr;
};

// Uygulamanın ana penceresi.
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

    void debugOpenFirstPhoto();

protected:
    void closeEvent(QCloseEvent* e) override;

private:
    void rescan();
    void refreshView();
    void updateEmptyState();
    void updateStatus(const QString& scanningText = QString());
    void updateAlbums();
    void openViewer(int index);
    void closeViewer();
    void navigate(int delta);
    void addFolder();
    void importPhotos();
    void showContextMenu(int index, const QPoint& globalPos);
    void afterPhotoMutation();

    Settings settings_;
    ThumbCache* thumbs_ = nullptr;
    LibraryScanner* scanner_ = nullptr;
    Sidebar* sidebar_ = nullptr;
    ContentPage* content_ = nullptr;
    PhotoViewer* viewer_ = nullptr;
    QStackedWidget* stack_ = nullptr;

    QVector<Photo> allPhotos_;
    QVector<Photo> viewPhotos_;
    QString filterKind_ = QStringLiteral("library");
    QString filterValue_;
    QString searchText_;
    int viewerIndex_ = -1;
};