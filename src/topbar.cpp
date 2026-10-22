#include "topbar.h"
#include "icons.h"
#include "theme.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QStyle>

TopBar::TopBar(QWidget* parent)
    : QFrame(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    auto* row = new QHBoxLayout(this);
    row->setContentsMargins(0, 0, 0, 0);
    row->setSpacing(10);

    // ---- Years / Months / Days
    auto* seg = new QFrame(this);
    seg->setObjectName(QStringLiteral("segContainer"));
    seg->setFixedHeight(42);
    auto* segBox = new QHBoxLayout(seg);
    segBox->setContentsMargins(4, 4, 4, 4);
    segBox->setSpacing(2);

    const struct {
        const char* id;
        const char* label;
    } segs[] = {{"years", "Years"}, {"months", "Months"}, {"days", "Days"}};
    for (const auto& s : segs) {
        auto* b = new QPushButton(QString::fromLatin1(s.label), seg);
        b->setObjectName(QStringLiteral("segButton"));
        b->setCursor(Qt::PointingHandCursor);
        segBox->addWidget(b);
        seg_.insert(QString::fromLatin1(s.id), b);
        const QString id = QString::fromLatin1(s.id);
        connect(b, &QPushButton::clicked, this, [this, id]() {
            applySegStyles();
            emit groupingChanged(id);
        });
    }
    row->addWidget(seg);

    // ---- zoom (− sürgü +)
    auto* zoomPill = new QFrame(this);
    zoomPill->setObjectName(QStringLiteral("pill"));
    zoomPill->setFixedHeight(42);
    auto* zBox = new QHBoxLayout(zoomPill);
    zBox->setContentsMargins(6, 6, 8, 6);
    zBox->setSpacing(4);

    auto* minus = new QPushButton(zoomPill);
    minus->setObjectName(QStringLiteral("iconButton"));
    minus->setFixedSize(28, 28);
    minus->setIcon(icons::icon(QStringLiteral("minus"), 13));
    minus->setIconSize(QSize(13, 13));
    minus->setCursor(Qt::PointingHandCursor);

    zoom_ = new QSlider(Qt::Horizontal, zoomPill);
    zoom_->setRange(0, 100);
    zoom_->setFixedWidth(108);
    zoom_->setCursor(Qt::PointingHandCursor);

    auto* plus = new QPushButton(zoomPill);
    plus->setObjectName(QStringLiteral("iconButton"));
    plus->setFixedSize(28, 28);
    plus->setIcon(icons::icon(QStringLiteral("plus"), 13));
    plus->setIconSize(QSize(13, 13));
    plus->setCursor(Qt::PointingHandCursor);

    zBox->addWidget(minus);
    zBox->addWidget(zoom_);
    zBox->addWidget(plus);
    row->addWidget(zoomPill);

    connect(zoom_, &QSlider::valueChanged, this,
            [this](int v) { emit zoomChanged(sliderToRowHeight(v)); });
    connect(minus, &QPushButton::clicked, this, [this]() { zoom_->setValue(zoom_->value() - 8); });
    connect(plus, &QPushButton::clicked, this, [this]() { zoom_->setValue(zoom_->value() + 8); });

    // ---- Upload
    auto* upload = new QPushButton(QStringLiteral("Upload"), this);
    upload->setObjectName(QStringLiteral("accentButton"));
    upload->setIcon(icons::icon(QStringLiteral("upload"), 15, Qt::white));
    upload->setIconSize(QSize(15, 15));
    upload->setFixedHeight(42);
    upload->setCursor(Qt::PointingHandCursor);
    connect(upload, &QPushButton::clicked, this, &TopBar::uploadRequested);
    row->addWidget(upload);

    // ---- Search
    auto* searchPill = new QFrame(this);
    searchPill->setObjectName(QStringLiteral("searchPill"));
    searchPill->setFixedHeight(42);
    searchPill->setFixedWidth(232);
    auto* sBox = new QHBoxLayout(searchPill);
    sBox->setContentsMargins(14, 0, 14, 0);
    sBox->setSpacing(8);

    auto* mag = new QLabel(searchPill);
    mag->setPixmap(icons::pixmap(QStringLiteral("search"), 15, Qt::white));
    sBox->addWidget(mag);

    search_ = new QLineEdit(searchPill);
    search_->setObjectName(QStringLiteral("searchEdit"));
    search_->setPlaceholderText(QStringLiteral("Search"));
    search_->setFrame(false);
    sBox->addWidget(search_);
    row->addWidget(searchPill);

    connect(search_, &QLineEdit::textChanged, this, &TopBar::searchChanged);

    // ---- kapat (aramayı temizle)
    auto* close = new QPushButton(this);
    close->setObjectName(QStringLiteral("closeButton"));
    close->setFixedSize(34, 34);
    close->setIcon(icons::icon(QStringLiteral("close"), 12, QColor(theme::TEXT_DIM)));
    close->setIconSize(QSize(12, 12));
    close->setCursor(Qt::PointingHandCursor);
    close->setToolTip(QStringLiteral("Aramayı temizle"));
    connect(close, &QPushButton::clicked, this, &TopBar::clearRequested);
    row->addWidget(close);

    applySegStyles();
    setFixedHeight(42);
}

void TopBar::applySegStyles()
{
    for (auto it = seg_.cbegin(); it != seg_.cend(); ++it) {
        const bool active = (it.key() == grouping_);
        it.value()->setProperty("active", active);
        it.value()->style()->unpolish(it.value());
        it.value()->style()->polish(it.value());
    }
}

int TopBar::rowHeightToSlider(int rowHeight)
{
    return qBound(0, qRound((rowHeight - 110) * 10.0 / 21.0), 100);
}

int TopBar::sliderToRowHeight(int value)
{
    return 110 + qRound(value * 21.0 / 10.0);
}

void TopBar::setGrouping(const QString& grouping)
{
    grouping_ = grouping;
    applySegStyles();
}

void TopBar::setZoom(int rowHeight)
{
    zoom_->blockSignals(true);
    zoom_->setValue(rowHeightToSlider(rowHeight));
    zoom_->blockSignals(false);
}

QString TopBar::searchText() const
{
    return search_->text();
}

void TopBar::setSearchText(const QString& text)
{
    search_->setText(text);
}