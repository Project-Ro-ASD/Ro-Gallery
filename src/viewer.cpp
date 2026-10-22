#include "viewer.h"
#include "icons.h"
#include "theme.h"

#include <QKeyEvent>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QtMath>

namespace {

QString humanSize(qint64 bytes)
{
    const char* units[] = {"B", "KB", "MB", "GB"};
    double v = double(bytes);
    int u = 0;
    while (v >= 1024.0 && u < 3) {
        v /= 1024.0;
        ++u;
    }
    return QStringLiteral("%1 %2").arg(v, 0, 'f', u == 0 ? 0 : 1)
        .arg(QString::fromLatin1(units[u]));
}

QString humanDate(const QDateTime& dt)
{
    static const char* months[12] = {
        "January", "February", "March",     "April",   "May",      "June",
        "July",    "August",   "September", "October", "November", "December"};
    return QStringLiteral("%1 %2, %3")
        .arg(QString::fromLatin1(months[dt.date().month() - 1]))
        .arg(dt.date().day())
        .arg(dt.date().year());
}

} // namespace

// -------------------------------------------------------------- ImageView

ImageView::ImageView(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(200, 200);
    setMouseTracking(true);
    setCursor(Qt::ArrowCursor);
}

void ImageView::setImage(const QImage& img)
{
    img_ = img;
    fitToWindow();
}

void ImageView::fitToWindow()
{
    if (img_.isNull() || width() < 10 || height() < 10)
        return;
    const double sx = double(width() - 60) / double(img_.width());
    const double sy = double(height() - 60) / double(img_.height());
    scale_ = qMin(1.0, qMin(sx, sy));
    offset_ = QPointF(0, 0);
    fitted_ = true;
    emit scaleChanged(scalePercent());
    update();
}

void ImageView::actualSize()
{
    scale_ = 1.0;
    offset_ = QPointF(0, 0);
    fitted_ = false;
    emit scaleChanged(scalePercent());
    update();
}

void ImageView::zoomIn()
{
    zoomAt(QPointF(width() / 2.0, height() / 2.0), 1.25);
}

void ImageView::zoomOut()
{
    zoomAt(QPointF(width() / 2.0, height() / 2.0), 0.8);
}

int ImageView::scalePercent() const
{
    return qRound(scale_ * 100.0);
}

void ImageView::zoomAt(const QPointF& pos, double factor)
{
    if (img_.isNull())
        return;
    const QPointF center(width() / 2.0 + offset_.x(), height() / 2.0 + offset_.y());
    const QPointF pImg = (pos - center) / scale_;
    scale_ = qBound(0.05, scale_ * factor, 16.0);
    const QPointF c2 = pos - pImg * scale_;
    offset_ = c2 - QPointF(width() / 2.0, height() / 2.0);
    fitted_ = false;
    emit scaleChanged(scalePercent());
    update();
}

void ImageView::paintEvent(QPaintEvent*)
{
    if (img_.isNull())
        return;
    QPainter p(this);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);
    const QPointF center(width() / 2.0 + offset_.x(), height() / 2.0 + offset_.y());
    const QSizeF sz(double(img_.width()) * scale_, double(img_.height()) * scale_);
    const QRectF target(center.x() - sz.width() / 2.0, center.y() - sz.height() / 2.0,
                        sz.width(), sz.height());
    p.drawImage(target, img_);
}

void ImageView::wheelEvent(QWheelEvent* e)
{
    if (img_.isNull())
        return;
    const double delta = e->angleDelta().y();
    zoomAt(e->position(), qPow(1.0016, delta));
    e->accept();
}

void ImageView::mousePressEvent(QMouseEvent* e)
{
    lastPos_ = e->position().toPoint();
    if (e->button() == Qt::LeftButton)
        setCursor(Qt::ClosedHandCursor);
    QWidget::mousePressEvent(e);
}

void ImageView::mouseMoveEvent(QMouseEvent* e)
{
    if (e->buttons() & Qt::LeftButton) {
        const QPoint cur = e->position().toPoint();
        offset_ += cur - lastPos_;
        lastPos_ = cur;
        fitted_ = false;
        update();
    }
    QWidget::mouseMoveEvent(e);
}

void ImageView::mouseDoubleClickEvent(QMouseEvent* e)
{
    if (img_.isNull())
        return;
    if (fitted_)
        actualSize();
    else
        fitToWindow();
    QWidget::mouseDoubleClickEvent(e);
}

void ImageView::resizeEvent(QResizeEvent* e)
{
    if (fitted_)
        fitToWindow();
    QWidget::resizeEvent(e);
}

// ------------------------------------------------------------ PhotoViewer

PhotoViewer::PhotoViewer(ThumbCache* thumbs, QWidget* parent)
    : QFrame(parent), thumbs_(thumbs)
{
    setFocusPolicy(Qt::StrongFocus);

    auto* top = new QHBoxLayout;
    top->setContentsMargins(22, 16, 18, 0);
    top->setSpacing(10);

    fileLabel_ = new QLabel(this);
    fileLabel_->setObjectName(QStringLiteral("viewerFile"));
    metaLabel_ = new QLabel(this);
    metaLabel_->setObjectName(QStringLiteral("viewerMeta"));
    top->addWidget(fileLabel_);
    top->addWidget(metaLabel_);
    top->addStretch(1);

    auto* close = new QPushButton(this);
    close->setObjectName(QStringLiteral("closeButton"));
    close->setFixedSize(34, 34);
    close->setIcon(icons::icon(QStringLiteral("close"), 12, QColor(theme::TEXT_DIM)));
    close->setIconSize(QSize(12, 12));
    close->setCursor(Qt::PointingHandCursor);
    connect(close, &QPushButton::clicked, this, &PhotoViewer::closed);
    top->addWidget(close);

    // orta: resim + oklar
    auto* mid = new QHBoxLayout;
    mid->setContentsMargins(10, 8, 10, 8);
    mid->setSpacing(8);

    auto* prev = new QPushButton(this);
    prev->setObjectName(QStringLiteral("navArrow"));
    prev->setFixedSize(42, 42);
    {
        // sola bakan ok
        QPixmap pm = icons::pixmap(QStringLiteral("chevron"), 14, QColor(theme::TEXT));
        QTransform tr;
        tr.rotate(180);
        prev->setIcon(QIcon(pm.transformed(tr)));
    }
    prev->setIconSize(QSize(14, 14));
    prev->setCursor(Qt::PointingHandCursor);
    connect(prev, &QPushButton::clicked, this, [this]() { emit navigateRequested(-1); });

    auto* next = new QPushButton(this);
    next->setObjectName(QStringLiteral("navArrow"));
    next->setFixedSize(42, 42);
    next->setIcon(icons::icon(QStringLiteral("chevron"), 14, QColor(theme::TEXT)));
    next->setIconSize(QSize(14, 14));
    next->setCursor(Qt::PointingHandCursor);
    connect(next, &QPushButton::clicked, this, [this]() { emit navigateRequested(1); });

    view_ = new ImageView(this);

    mid->addWidget(prev, 0, Qt::AlignVCenter);
    mid->addWidget(view_, 1);
    mid->addWidget(next, 0, Qt::AlignVCenter);

    // alt araç çubuğu
    bottomBar_ = new QFrame(this);
    bottomBar_->setObjectName(QStringLiteral("viewerBar"));
    auto* bar = new QHBoxLayout(bottomBar_);
    bar->setContentsMargins(8, 6, 8, 6);
    bar->setSpacing(4);

    auto toolBtn = [this](const QString& iconName, int iconSize = 15) {
        auto* b = new QPushButton(bottomBar_);
        b->setObjectName(QStringLiteral("iconButton"));
        b->setFixedSize(34, 34);
        b->setIcon(icons::icon(iconName, iconSize));
        b->setIconSize(QSize(iconSize, iconSize));
        b->setCursor(Qt::PointingHandCursor);
        return b;
    };

    QPushButton* zoomOut = toolBtn(QStringLiteral("minus"));
    zoomLabel_ = new QLabel(QStringLiteral("100%"), bottomBar_);
    zoomLabel_->setObjectName(QStringLiteral("viewerMeta"));
    zoomLabel_->setFixedWidth(48);
    zoomLabel_->setAlignment(Qt::AlignCenter);
    QPushButton* zoomIn = toolBtn(QStringLiteral("plus"));
    QPushButton* fit = new QPushButton(QStringLiteral("Sığdır"), bottomBar_);
    fit->setObjectName(QStringLiteral("iconButton"));
    fit->setFixedHeight(34);
    fit->setCursor(Qt::PointingHandCursor);
    favButton_ = toolBtn(QStringLiteral("heart"));
    favButton_->setToolTip(QStringLiteral("Favorilere ekle (F)"));
    deleteButton_ = new QPushButton(bottomBar_);
    deleteButton_->setObjectName(QStringLiteral("iconButton"));
    deleteButton_->setFixedHeight(34);
    deleteButton_->setCursor(Qt::PointingHandCursor);

    bar->addWidget(zoomOut);
    bar->addWidget(zoomLabel_);
    bar->addWidget(zoomIn);
    bar->addSpacing(6);
    bar->addWidget(fit);
    bar->addSpacing(6);
    bar->addWidget(favButton_);
    bar->addWidget(deleteButton_);

    connect(zoomOut, &QPushButton::clicked, view_, &ImageView::zoomOut);
    connect(zoomIn, &QPushButton::clicked, view_, &ImageView::zoomIn);
    connect(fit, &QPushButton::clicked, view_, &ImageView::fitToWindow);
    connect(view_, &ImageView::scaleChanged, this,
            [this](int p) { zoomLabel_->setText(QStringLiteral("%1%").arg(p)); });
    connect(favButton_, &QPushButton::clicked, this, [this]() {
        emit favoriteToggled(photo_.path);
    });
    connect(deleteButton_, &QPushButton::clicked, this, [this]() {
        if (deletedView_)
            emit restoreRequested(photo_.path);
        else
            emit deleteRequested(photo_.path);
    });

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);
    root->addLayout(top);
    root->addLayout(mid, 1);

    connect(thumbs_, &ThumbCache::fullReady, this,
            [this](const QString& path, const QImage& image) {
                if (path == photo_.path && !image.isNull())
                    view_->setImage(image);
            });
}

void PhotoViewer::resizeEvent(QResizeEvent* e)
{
    bottomBar_->adjustSize();
    bottomBar_->move((width() - bottomBar_->width()) / 2,
                     height() - bottomBar_->height() - 18);
    bottomBar_->raise();
    QFrame::resizeEvent(e);
}

void PhotoViewer::showPhoto(const Photo& photo, int index, int total, bool favorite,
                            bool deletedView)
{
    photo_ = photo;
    favorite_ = favorite;
    deletedView_ = deletedView;

    fileLabel_->setText(photo.name);
    metaLabel_->setText(QStringLiteral("%1 · %2 · %3×%4 · %5/%6")
                            .arg(humanDate(photo.dt))
                            .arg(humanSize(photo.size))
                            .arg(photo.width)
                            .arg(photo.height)
                            .arg(index)
                            .arg(total));

    if (deletedView_) {
        deleteButton_->setText(QStringLiteral("Geri Yükle"));
        deleteButton_->setIcon(icons::icon(QStringLiteral("restore"), 14));
        deleteButton_->setToolTip(QStringLiteral("Geri yükle"));
    } else {
        deleteButton_->setText(QStringLiteral("Sil"));
        deleteButton_->setIcon(icons::icon(QStringLiteral("trash"), 14));
        deleteButton_->setToolTip(QStringLiteral("Sil (Del)"));
    }
    updateFavButton();

    thumbs_->requestFull(photo.path);
    const QImage cached = thumbs_->get(photo.path, photo.size, 0);
    if (!cached.isNull())
        view_->setImage(cached);
    setFocus(Qt::OtherFocusReason);
}

void PhotoViewer::updateFavButton()
{
    favButton_->setIcon(icons::icon(favorite_ ? QStringLiteral("heart_filled")
                                              : QStringLiteral("heart"),
                                    15, favorite_ ? QColor(theme::ACCENT)
                                                  : QColor(theme::TEXT)));
}

void PhotoViewer::keyPressEvent(QKeyEvent* e)
{
    switch (e->key()) {
    case Qt::Key_Escape:
        emit closed();
        return;
    case Qt::Key_Left:
        emit navigateRequested(-1);
        return;
    case Qt::Key_Right:
        emit navigateRequested(1);
        return;
    case Qt::Key_Plus:
    case Qt::Key_Equal:
        view_->zoomIn();
        return;
    case Qt::Key_Minus:
        view_->zoomOut();
        return;
    case Qt::Key_F:
        emit favoriteToggled(photo_.path);
        return;
    case Qt::Key_Delete:
        if (deletedView_)
            emit restoreRequested(photo_.path);
        else
            emit deleteRequested(photo_.path);
        return;
    default:
        QFrame::keyPressEvent(e);
    }
}