#include "sidebar.h"
#include "icons.h"
#include "theme.h"

#include <QFileInfo>
#include <QHBoxLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScrollArea>
#include <QStyle>

// ------------------------------------------------------------- AlbumCover

AlbumCover::AlbumCover(const QString& name, const QString& folder, QWidget* parent)
    : QWidget(parent), name_(name), folder_(folder)
{
    setFixedHeight(76);
    setCursor(Qt::PointingHandCursor);
}

void AlbumCover::setCoverImage(const QImage& img)
{
    cover_ = img;
    update();
}

void AlbumCover::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    QPainterPath clip;
    clip.addRoundedRect(QRectF(rect()).adjusted(0, 0, -0.5, -0.5), 10, 10);
    p.setClipPath(clip);

    if (!cover_.isNull()) {
        const QPixmap px = QPixmap::fromImage(cover_);
        QSize target = px.size().scaled(size(), Qt::KeepAspectRatioByExpanding);
        const QRect r(QPoint((width() - target.width()) / 2, (height() - target.height()) / 2),
                      target);
        p.drawPixmap(r, px);
    } else {
        p.fillRect(rect(), QColor(QStringLiteral("#1c1c22")));
    }

    QLinearGradient g(0, height() * 0.35, 0, height());
    g.setColorAt(0, QColor(0, 0, 0, 0));
    g.setColorAt(1, QColor(0, 0, 0, 200));
    p.fillRect(rect(), g);

    QFont f = font();
    f.setPixelSize(12);
    f.setWeight(QFont::DemiBold);
    p.setFont(f);
    p.setPen(Qt::white);
    p.drawText(QRectF(rect()).adjusted(11, 0, -11, -8),
               Qt::AlignLeft | Qt::AlignBottom, name_);
}

void AlbumCover::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
        emit clicked(folder_);
    QWidget::mousePressEvent(e);
}

// ----------------------------------------------------------------- Sidebar

Sidebar::Sidebar(ThumbCache* thumbs, QWidget* parent)
    : QFrame(parent), thumbs_(thumbs)
{
    setObjectName(QStringLiteral("sidebar"));
    setFixedWidth(252);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setFrameShape(QFrame::NoFrame);

    auto* content = new QWidget;
    content->setObjectName(QStringLiteral("sidebarContent"));
    auto* box = new QVBoxLayout(content);
    box->setContentsMargins(14, 22, 14, 18);
    box->setSpacing(2);

    auto* title = new QLabel(QStringLiteral("Photos"), content);
    title->setObjectName(QStringLiteral("appTitle"));
    box->addWidget(title);
    box->addSpacing(12);

    // gezinme
    addNav(box, QStringLiteral("library"), QStringLiteral("Library"), QStringLiteral("library"));
    addNav(box, QStringLiteral("albums"), QStringLiteral("Albums"), QStringLiteral("photos"));
    addNav(box, QStringLiteral("heart"), QStringLiteral("Favorites"), QStringLiteral("favorites"));
    addNav(box, QStringLiteral("photo"), QStringLiteral("Photos"), QStringLiteral("photos"));
    addNav(box, QStringLiteral("video"), QStringLiteral("Videos"), QStringLiteral("videos"));
    addNav(box, QStringLiteral("trash"), QStringLiteral("Deleted"), QStringLiteral("deleted"));

    box->addSpacing(14);
    addSectionHeader(box, QStringLiteral("Sources"), QStringLiteral("plus"));
    sourcesBox_ = new QVBoxLayout;
    sourcesBox_->setContentsMargins(0, 0, 0, 0);
    sourcesBox_->setSpacing(2);
    box->addLayout(sourcesBox_);

    box->addSpacing(14);
    addSectionHeader(box, QStringLiteral("Utilities"), QString());
    addNav(box, QStringLiteral("spark"), QStringLiteral("Live Photos"), QStringLiteral("util:live"));
    addNav(box, QStringLiteral("panorama"), QStringLiteral("Panoramas"), QStringLiteral("util:panoramas"));
    addNav(box, QStringLiteral("screenshot"), QStringLiteral("Screenshots"), QStringLiteral("util:screenshots"));
    addNav(box, QStringLiteral("360"), QStringLiteral("360°"), QStringLiteral("util:360"));

    box->addSpacing(14);
    addSectionHeader(box, QStringLiteral("Albums"), QStringLiteral("chevron"));
    albumsBox_ = new QVBoxLayout;
    albumsBox_->setContentsMargins(0, 0, 0, 0);
    albumsBox_->setSpacing(8);
    box->addLayout(albumsBox_);

    box->addStretch(1);

    scroll->setWidget(content);
    auto* wrap = new QVBoxLayout(this);
    wrap->setContentsMargins(0, 0, 0, 0);
    wrap->addWidget(scroll);

    connect(thumbs_, &ThumbCache::updated, this, &Sidebar::onThumbUpdated);
    refreshActive();
}

void Sidebar::addSectionHeader(QBoxLayout* layout, const QString& title, const QString& buttonIcon)
{
    auto* row = new QWidget(this);
    auto* hb = new QHBoxLayout(row);
    hb->setContentsMargins(10, 4, 6, 4);
    hb->setSpacing(4);

    auto* lbl = new QLabel(title, row);
    lbl->setObjectName(QStringLiteral("sectionLabel"));
    hb->addWidget(lbl);
    hb->addStretch(1);

    if (!buttonIcon.isEmpty()) {
        auto* btn = new QPushButton(row);
        btn->setObjectName(QStringLiteral("sectionButton"));
        btn->setFixedSize(24, 24);
        btn->setIcon(icons::icon(buttonIcon, 13, QColor(theme::TEXT_SECTION)));
        btn->setIconSize(QSize(13, 13));
        if (buttonIcon == QLatin1String("plus")) {
            btn->setToolTip(QStringLiteral("Klasör ekle"));
            connect(btn, &QPushButton::clicked, this, &Sidebar::addFolderRequested);
        } else {
            btn->setToolTip(QStringLiteral("Albümleri göster/gizle"));
            connect(btn, &QPushButton::clicked, this, [this]() {
                const bool show = (albumsBox_->count() == 0) ? true : !albumsBox_->itemAt(0)->widget()->isVisible();
                for (int i = 0; i < albumsBox_->count(); ++i) {
                    if (QWidget* w = albumsBox_->itemAt(i)->widget())
                        w->setVisible(show);
                }
            });
        }
        hb->addWidget(btn);
    }
    layout->addWidget(row);
}

QPushButton* Sidebar::addNav(QBoxLayout* layout, const QString& iconName, const QString& text,
                             const QString& kind, const QString& value)
{
    auto* btn = new QPushButton(this);
    btn->setObjectName(QStringLiteral("navButton"));
    btn->setIcon(icons::icon(iconName, 16));
    btn->setIconSize(QSize(16, 16));
    btn->setText(text);
    btn->setFixedHeight(32);
    btn->setCursor(Qt::PointingHandCursor);
    layout->addWidget(btn);

    navs_.append({btn, kind, value});
    connect(btn, &QPushButton::clicked, this, [this, kind, value]() {
        activeKind_ = kind;
        activeValue_ = value;
        refreshActive();
        emit filterSelected(kind, value);
    });
    return btn;
}

void Sidebar::refreshActive()
{
    for (const NavItem& item : navs_) {
        const bool active = (item.kind == activeKind_ && item.value == activeValue_);
        item.btn->setProperty("active", active);
        item.btn->style()->unpolish(item.btn);
        item.btn->style()->polish(item.btn);
    }
}

void Sidebar::setActive(const QString& kind, const QString& value)
{
    activeKind_ = kind;
    activeValue_ = value;
    refreshActive();
}

void Sidebar::setSources(const QStringList& folders)
{
    // eski kaynak düğmelerini temizle
    for (int i = navs_.size() - 1; i >= 0; --i) {
        if (navs_[i].kind == QLatin1String("source"))
            navs_.removeAt(i);
    }
    while (QLayoutItem* it = sourcesBox_->takeAt(0)) {
        if (it->widget())
            it->widget()->deleteLater();
        delete it;
    }

    if (folders.isEmpty()) {
        auto* lbl = new QLabel(QStringLiteral("Henüz klasör eklenmedi"), this);
        lbl->setObjectName(QStringLiteral("sectionLabel"));
        lbl->setWordWrap(true);
        sourcesBox_->addWidget(lbl);
        return;
    }

    for (const QString& folder : folders) {
        QString name = QFileInfo(folder).fileName();
        if (name.isEmpty())
            name = folder;
        QPushButton* btn = addNav(sourcesBox_, QStringLiteral("folder"), name,
                                  QStringLiteral("source"), folder);
        btn->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(btn, &QWidget::customContextMenuRequested, this,
                [this, btn, folder](const QPoint& pos) {
                    QMenu menu(btn);
                    menu.addAction(icons::icon(QStringLiteral("open"), 14),
                                   QStringLiteral("Klasörü Aç"),
                                   [this, folder]() { emit openFolderRequested(folder); });
                    menu.addSeparator();
                    menu.addAction(icons::icon(QStringLiteral("close"), 14),
                                   QStringLiteral("Kütüphaneden Kaldır"),
                                   [this, folder]() { emit removeFolderRequested(folder); });
                    menu.exec(btn->mapToGlobal(pos));
                });
    }
    refreshActive();
}

void Sidebar::setAlbums(const QVector<AlbumInfo>& albums)
{
    while (QLayoutItem* it = albumsBox_->takeAt(0)) {
        if (it->widget())
            it->widget()->deleteLater();
        delete it;
    }
    covers_.clear();
    coverPhotos_.clear();

    for (const AlbumInfo& a : albums) {
        auto* cover = new AlbumCover(a.name, a.folder, this);
        connect(cover, &AlbumCover::clicked, this, [this](const QString& folder) {
            activeKind_ = QStringLiteral("album");
            activeValue_ = folder;
            refreshActive();
            emit filterSelected(QStringLiteral("album"), folder);
        });
        albumsBox_->addWidget(cover);
        covers_.append(cover);
        coverPhotos_.append(a.cover);
        if (a.hasCover) {
            const QImage img = thumbs_->get(a.cover.path, a.cover.size, 120);
            if (!img.isNull())
                cover->setCoverImage(img);
            else
                thumbs_->request(a.cover, 120);
        }
    }
}

void Sidebar::onThumbUpdated(const QString& path, int key)
{
    if (key != 120)
        return;
    for (int i = 0; i < covers_.size(); ++i) {
        if (i < coverPhotos_.size() && coverPhotos_[i].path == path) {
            const QImage img = thumbs_->get(path, coverPhotos_[i].size, 120);
            if (!img.isNull())
                covers_[i]->setCoverImage(img);
        }
    }
}