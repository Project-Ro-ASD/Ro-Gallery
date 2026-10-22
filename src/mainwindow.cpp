#include "mainwindow.h"

#include <QCloseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QMenu>
#include <QScrollBar>
#include <QUrl>
#include <QVBoxLayout>

// ------------------------------------------------------------ ContentPage

ContentPage::ContentPage(ThumbCache* thumbs, QWidget* parent)
    : QWidget(parent)
{
    grid = new PhotoGrid(thumbs, this);
    scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setWidget(grid);
    grid->setScrollArea(scroll);

    topbar = new TopBar(this);
    status = new QLabel(this);
    status->setObjectName(QStringLiteral("statusPill"));
    status->hide();

    empty = new QWidget(this);
    auto* ev = new QVBoxLayout(empty);
    ev->setContentsMargins(24, 24, 24, 24);
    ev->setSpacing(8);

    emptyTitle_ = new QLabel(empty);
    emptyTitle_->setObjectName(QStringLiteral("emptyTitle"));
    emptyTitle_->setAlignment(Qt::AlignCenter);
    emptyTitle_->setWordWrap(true);

    emptyText_ = new QLabel(empty);
    emptyText_->setObjectName(QStringLiteral("emptyText"));
    emptyText_->setAlignment(Qt::AlignCenter);
    emptyText_->setWordWrap(true);

    emptyButton_ = new QPushButton(QStringLiteral("Klasör Ekle"), empty);
    emptyButton_->setObjectName(QStringLiteral("accentButton"));
    emptyButton_->setCursor(Qt::PointingHandCursor);
    connect(emptyButton_, &QPushButton::clicked, this, &ContentPage::emptyActionRequested);

    ev->addStretch(1);
    ev->addWidget(emptyTitle_);
    ev->addWidget(emptyText_);
    ev->addSpacing(10);
    ev->addWidget(emptyButton_, 0, Qt::AlignHCenter);
    ev->addStretch(1);
    empty->hide();

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->addWidget(scroll);
}

void ContentPage::setEmptyState(const QString& title, const QString& text, bool showButton)
{
    emptyTitle_->setText(title);
    emptyText_->setText(text);
    emptyButton_->setVisible(showButton);
    empty->show();
    empty->raise();
}

void ContentPage::hideEmptyState()
{
    empty->hide();
}

void ContentPage::setStatus(const QString& text, bool visible)
{
    status->setText(text);
    status->setVisible(visible);
    status->setMinimumWidth(0);
    status->adjustSize();
    // QLabel sizeHint QSS padding'i hesaba katmaz; telafi et
    status->setMinimumWidth(status->sizeHint().width() + 36);
    positionOverlays();
}

void ContentPage::positionOverlays()
{
    topbar->adjustSize();
    topbar->move(qMax(12, (width() - topbar->width()) / 2), 14);
    topbar->raise();

    status->adjustSize();
    status->setMinimumWidth(status->sizeHint().width() + 36);
    status->move(qMax(12, width() - status->width() - 20), height() - status->height() - 16);
    status->raise();

    empty->resize(qMin(560, width() - 80), 240);
    empty->move((width() - empty->width()) / 2, (height() - empty->height()) / 2);
    empty->raise();
}

void ContentPage::resizeEvent(QResizeEvent* e)
{
    positionOverlays();
    QWidget::resizeEvent(e);
}

// -------------------------------------------------------------- MainWindow

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Ro-Gallery"));
    const QByteArray geo = settings_.geometry();
    if (!geo.isEmpty())
        restoreGeometry(geo);
    else
        resize(1380, 880);

    thumbs_ = new ThumbCache(this);
    scanner_ = new LibraryScanner(this);

    auto* central = new QWidget(this);
    setCentralWidget(central);
    auto* root = new QVBoxLayout(central);
    root->setContentsMargins(12, 12, 12, 12);

    auto* appFrame = new QFrame(central);
    auto* hl = new QHBoxLayout(appFrame);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(0);

    sidebar_ = new Sidebar(thumbs_, appFrame);
    content_ = new ContentPage(thumbs_, appFrame);
    viewer_ = new PhotoViewer(thumbs_, appFrame);

    stack_ = new QStackedWidget(appFrame);
    stack_->setObjectName(QStringLiteral("content"));
    stack_->addWidget(content_);
    stack_->addWidget(viewer_);

    hl->addWidget(sidebar_);
    hl->addWidget(stack_, 1);
    root->addWidget(appFrame);

    // ---- tarayıcı
    connect(scanner_, &LibraryScanner::finished, this, [this](const QVector<Photo>& photos) {
        allPhotos_ = photos;
        updateAlbums();
        refreshView();
    });
    connect(scanner_, &LibraryScanner::progress, this,
            [this](const QString& text) { updateStatus(text); });

    // ---- kenar çubuğu
    connect(sidebar_, &Sidebar::filterSelected, this,
            [this](const QString& kind, const QString& value) {
                filterKind_ = kind;
                filterValue_ = value;
                refreshView();
                content_->scroll->verticalScrollBar()->setValue(0);
            });
    connect(sidebar_, &Sidebar::addFolderRequested, this, &MainWindow::addFolder);
    connect(sidebar_, &Sidebar::removeFolderRequested, this, [this](const QString& path) {
        settings_.removeFolder(path);
        rescan();
    });
    connect(sidebar_, &Sidebar::openFolderRequested, this, [](const QString& path) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    });

    // ---- ızgara
    connect(content_->grid, &PhotoGrid::photoActivated, this, &MainWindow::openViewer);
    connect(content_->grid, &PhotoGrid::contextMenuRequested, this, &MainWindow::showContextMenu);

    // ---- üst araç çubuğu
    connect(content_->topbar, &TopBar::groupingChanged, this, [this](const QString& g) {
        settings_.setGrouping(g);
        refreshView();
    });
    connect(content_->topbar, &TopBar::zoomChanged, this, [this](int rowHeight) {
        settings_.setRowHeight(rowHeight);
        content_->grid->setRowHeight(rowHeight);
    });
    connect(content_->topbar, &TopBar::uploadRequested, this, &MainWindow::importPhotos);
    connect(content_->topbar, &TopBar::searchChanged, this, [this](const QString& text) {
        searchText_ = text;
        refreshView();
    });
    connect(content_->topbar, &TopBar::clearRequested, this,
            [this]() { content_->topbar->setSearchText(QString()); });
    connect(content_, &ContentPage::emptyActionRequested, this, &MainWindow::addFolder);

    // ---- görüntüleyici
    connect(viewer_, &PhotoViewer::closed, this, &MainWindow::closeViewer);
    connect(viewer_, &PhotoViewer::navigateRequested, this, &MainWindow::navigate);
    connect(viewer_, &PhotoViewer::favoriteToggled, this, [this](const QString& path) {
        settings_.toggleFavorite(path);
        afterPhotoMutation();
    });
    connect(viewer_, &PhotoViewer::deleteRequested, this, [this](const QString& path) {
        settings_.softDelete(path);
        afterPhotoMutation();
    });
    connect(viewer_, &PhotoViewer::restoreRequested, this, [this](const QString& path) {
        settings_.restore(path);
        afterPhotoMutation();
    });
    connect(viewer_, &PhotoViewer::openExternalRequested, this, [](const QString& path) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    });

    // ---- ilk yükleme
    sidebar_->setSources(settings_.folders());
    content_->topbar->setGrouping(settings_.grouping());
    content_->topbar->setZoom(settings_.rowHeight());
    rescan();
}

void MainWindow::closeEvent(QCloseEvent* e)
{
    settings_.setGeometry(saveGeometry());
    QMainWindow::closeEvent(e);
}

void MainWindow::rescan()
{
    const QStringList folders = settings_.folders();
    sidebar_->setSources(folders);
    if (folders.isEmpty()) {
        allPhotos_.clear();
        updateAlbums();
        refreshView();
        return;
    }
    updateStatus(QStringLiteral("Taranıyor…"));
    scanner_->scan(folders);
}

void MainWindow::refreshView()
{
    const QStringList favorites = settings_.favorites();
    const QStringList deleted = settings_.deleted();
    const QSet<QString> fav(favorites.begin(), favorites.end());
    const QSet<QString> del(deleted.begin(), deleted.end());

    viewPhotos_ = library::applyFilter(allPhotos_, filterKind_, filterValue_, fav, del);

    if (!searchText_.isEmpty()) {
        const QString s = searchText_.toLower();
        QVector<Photo> filtered;
        filtered.reserve(viewPhotos_.size());
        for (const Photo& p : viewPhotos_) {
            if (p.name.toLower().contains(s) || p.folder.toLower().contains(s))
                filtered.append(p);
        }
        viewPhotos_ = filtered;
    }

    content_->grid->setRowHeight(settings_.rowHeight());
    content_->grid->setGroups(library::groupPhotos(viewPhotos_, settings_.grouping()));
    content_->topbar->setGrouping(settings_.grouping());
    updateEmptyState();
    updateStatus();
}

void MainWindow::updateEmptyState()
{
    if (!viewPhotos_.isEmpty()) {
        content_->hideEmptyState();
        return;
    }

    QString title;
    QString text;
    bool button = false;

    if (settings_.folders().isEmpty()) {
        title = QStringLiteral("Fotoğraf kitaplığınız boş");
        text = QStringLiteral("Başlamak için soldaki Sources bölümünden + düğmesiyle "
                              "taranacak klasörleri seçin.");
        button = true;
    } else if (filterKind_ == QLatin1String("videos") || filterKind_ == QLatin1String("util:live")
               || filterKind_ == QLatin1String("util:360")) {
        title = QStringLiteral("2. adımda gelecek");
        text = QStringLiteral("Video desteği sonraki adımda eklenecek.");
    } else if (filterKind_ == QLatin1String("deleted")) {
        title = QStringLiteral("Çöp kutusu boş");
        text = QStringLiteral("Sildiğiniz fotoğraflar burada görünür.");
    } else if (!searchText_.isEmpty()) {
        title = QStringLiteral("Sonuç yok");
        text = QStringLiteral("“%1” için eşleşen fotoğraf bulunamadı.").arg(searchText_);
    } else {
        title = QStringLiteral("Fotoğraf bulunamadı");
        text = QStringLiteral("Bu görünümde gösterilecek fotoğraf yok.");
    }
    content_->setEmptyState(title, text, button);
}

void MainWindow::updateStatus(const QString& scanningText)
{
    if (!scanningText.isEmpty()) {
        content_->setStatus(scanningText, true);
        return;
    }
    if (allPhotos_.isEmpty()) {
        content_->setStatus(QString(), false);
        return;
    }
    const QLocale loc;
    content_->setStatus(QStringLiteral("%1 fotoğraf · %2 klasör")
                            .arg(loc.toString(viewPhotos_.size()))
                            .arg(settings_.folders().size()),
                        true);
}

void MainWindow::updateAlbums()
{
    QVector<Sidebar::AlbumInfo> albums;
    for (const QString& folder : settings_.folders()) {
        Sidebar::AlbumInfo a;
        a.name = QFileInfo(folder).fileName();
        if (a.name.isEmpty())
            a.name = folder;
        a.folder = folder;
        for (const Photo& p : allPhotos_) {
            if (p.root == folder) {
                a.cover = p;
                a.hasCover = true;
                break;
            }
        }
        albums.append(a);
    }
    sidebar_->setAlbums(albums);
}

void MainWindow::openViewer(int index)
{
    if (index < 0 || index >= viewPhotos_.size())
        return;
    viewerIndex_ = index;
    const Photo& p = viewPhotos_.at(index);
    viewer_->showPhoto(p, index + 1, viewPhotos_.size(), settings_.isFavorite(p.path),
                       filterKind_ == QLatin1String("deleted"));
    stack_->setCurrentWidget(viewer_);
    viewer_->setFocus();
}

void MainWindow::closeViewer()
{
    stack_->setCurrentWidget(content_);
    content_->grid->setFocus();
    content_->grid->scrollToIndex(viewerIndex_);
}

void MainWindow::navigate(int delta)
{
    if (viewPhotos_.isEmpty()) {
        closeViewer();
        return;
    }
    int next = viewerIndex_ + delta;
    next = qBound(0, next, viewPhotos_.size() - 1);
    openViewer(next);
}

void MainWindow::afterPhotoMutation()
{
    const bool viewing = (stack_->currentWidget() == viewer_);
    const int idx = viewerIndex_;
    refreshView();
    if (!viewing)
        return;
    if (viewPhotos_.isEmpty()) {
        closeViewer();
        return;
    }
    openViewer(qBound(0, idx, viewPhotos_.size() - 1));
}

void MainWindow::addFolder()
{
    const QString dir = QFileDialog::getExistingDirectory(
        this, QStringLiteral("Taranacak Klasörü Seç"), QDir::homePath());
    if (dir.isEmpty())
        return;
    settings_.addFolder(dir);
    rescan();
}

void MainWindow::importPhotos()
{
    if (settings_.folders().isEmpty())
        addFolder();
    if (settings_.folders().isEmpty())
        return;

    const QString dest = settings_.folders().first();
    const QStringList files = QFileDialog::getOpenFileNames(
        this, QStringLiteral("Fotoğrafları İçe Aktar"), QDir::homePath(),
        QStringLiteral("Görseller (*.jpg *.jpeg *.png *.gif *.webp *.bmp *.tif *.tiff "
                       "*.heic *.heif *.avif *.jxl *.raw *.dng)"));
    if (files.isEmpty())
        return;

    int copied = 0;
    for (const QString& f : files) {
        const QFileInfo fi(f);
        QString target = dest + QLatin1Char('/') + fi.fileName();
        int i = 1;
        while (QFile::exists(target)) {
            target = dest + QLatin1Char('/') + fi.completeBaseName() + QLatin1Char('-')
                     + QString::number(i++) + QLatin1Char('.') + fi.suffix();
        }
        if (QFile::copy(f, target))
            ++copied;
    }
    if (copied > 0)
        rescan();
}

void MainWindow::showContextMenu(int index, const QPoint& globalPos)
{
    if (index < 0 || index >= viewPhotos_.size())
        return;
    const Photo p = viewPhotos_.at(index);

    QMenu menu(this);
    menu.addAction(QStringLiteral("Aç"), [this, index]() { openViewer(index); });
    menu.addAction(settings_.isFavorite(p.path) ? QStringLiteral("Favorilerden Çıkar")
                                                : QStringLiteral("Favorilere Ekle"),
                   [this, p]() {
                       settings_.toggleFavorite(p.path);
                       refreshView();
                   });
    menu.addAction(QStringLiteral("Dosyayı Göster"),
                   [p]() { QDesktopServices::openUrl(QUrl::fromLocalFile(p.folder)); });
    menu.addSeparator();
    if (filterKind_ == QLatin1String("deleted")) {
        menu.addAction(QStringLiteral("Geri Yükle"), [this, p]() {
            settings_.restore(p.path);
            afterPhotoMutation();
        });
    } else {
        menu.addAction(QStringLiteral("Sil"), [this, p]() {
            settings_.softDelete(p.path);
            afterPhotoMutation();
        });
    }
    menu.exec(globalPos);
}

void MainWindow::debugOpenFirstPhoto()
{
    if (!viewPhotos_.isEmpty())
        openViewer(0);
}
