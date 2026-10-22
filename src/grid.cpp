#include "grid.h"
#include "theme.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include <QTimer>
#include <algorithm>

PhotoGrid::PhotoGrid(ThumbCache* thumbs, QWidget* parent)
    : QWidget(parent), thumbs_(thumbs)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);
    setMinimumHeight(200);
}

void PhotoGrid::setGroups(const QVector<library::Group>& groups)
{
    // mevcut seçimi koru
    QString keepPath;
    if (current_ >= 0 && current_ < photos_.size())
        keepPath = photos_[current_].path;

    groups_ = groups;
    photos_.clear();
    for (const library::Group& g : groups)
        for (const Photo& p : g.photos)
            photos_.append(p);

    current_ = -1;
    if (!keepPath.isEmpty()) {
        for (int i = 0; i < photos_.size(); ++i) {
            if (photos_[i].path == keepPath) {
                current_ = i;
                break;
            }
        }
    }

    relayout();
}

void PhotoGrid::setRowHeight(int h)
{
    h = qBound(110, h, 320);
    if (h == rowH_)
        return;
    rowH_ = h;
    relayout();
}

void PhotoGrid::setCurrentIndex(int idx)
{
    if (idx < -1 || idx >= photos_.size())
        return;
    if (current_ == idx)
        return;
    current_ = idx;
    emit indexChanged(current_);
    update();
}

void PhotoGrid::clearSelection()
{
    setCurrentIndex(-1);
}

void PhotoGrid::scrollToIndex(int idx)
{
    setCurrentIndex(idx);
    ensureCurrentVisible();
}

void PhotoGrid::relayout()
{
    relayouting_ = true;
    cells_.clear();
    headers_.clear();
    rows_.clear();
    rowOfIndex_ = QVector<int>(photos_.size(), -1);

    const double avail = qMax(240.0, double(width()) - 2.0 * marginX_);
    double y = 18.0;
    int idx = 0;

    for (const library::Group& g : groups_) {
        Header h;
        h.rect = QRectF(marginX_, y, avail, 54.0);
        h.label = g.label;
        h.subtitle = g.subtitle;
        headers_.append(h);
        y += 54.0 + 12.0;

        QVector<int> rowIdx;
        double rowX = 0.0;

        auto flushRow = [&](bool last) {
            if (rowIdx.isEmpty())
                return;
            double sumA = 0.0;
            for (int i : rowIdx)
                sumA += qBound(0.25, photos_[i].aspect(), 5.0);
            double h = (avail - gap_ * double(rowIdx.size() - 1)) / sumA;
            if (last)
                h = qMin(h, double(rowH_));
            else
                h = qBound(double(rowH_) * 0.72, h, double(rowH_) * 1.35);

            double x = marginX_;
            QVector<int> row;
            for (int i : rowIdx) {
                const double w = qBound(0.25, photos_[i].aspect(), 5.0) * h;
                cells_.append({QRectF(x, y, w, h), i});
                rowOfIndex_[i] = rows_.size();
                row.append(i);
                x += w + gap_;
            }
            rows_.append(row);
            y += h + gap_;
            rowIdx.clear();
            rowX = 0.0;
        };

        for (const Photo& p : g.photos) {
            rowIdx.append(idx);
            rowX += qBound(0.25, p.aspect(), 5.0) * double(rowH_) + gap_;
            ++idx;
            if (rowX >= avail)
                flushRow(false);
        }
        flushRow(true);
        y += 14.0;
    }

    totalH_ = y + 20.0;
    setMinimumHeight(int(totalH_) + 1);
    updateGeometry();
    relayouting_ = false;
    update();
}

int PhotoGrid::hitTest(const QPointF& pos) const
{
    for (const Cell& c : cells_) {
        if (c.rect.contains(pos))
            return c.index;
    }
    return -1;
}

void PhotoGrid::ensureCurrentVisible()
{
    if (!scroll_ || current_ < 0)
        return;
    for (const Cell& c : cells_) {
        if (c.index != current_)
            continue;
        QScrollBar* sb = scroll_->verticalScrollBar();
        const int top = int(c.rect.top());
        const int bottom = int(c.rect.bottom());
        const int viewTop = sb->value();
        const int viewH = scroll_->viewport()->height();
        if (top < viewTop + 30)
            sb->setValue(qMax(0, top - 90));
        else if (bottom > viewTop + viewH - 30)
            sb->setValue(bottom - viewH + 90);
        break;
    }
}

void PhotoGrid::scheduleUpdate()
{
    if (updatePending_)
        return;
    updatePending_ = true;
    QTimer::singleShot(0, this, [this]() {
        updatePending_ = false;
        update();
    });
}

void PhotoGrid::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const QRectF paintRect = e->rect().adjusted(0, -60, 0, 60);

    // grup başlıkları
    QFont hf = font();
    hf.setPixelSize(19);
    hf.setWeight(QFont::DemiBold);
    p.setFont(hf);
    p.setPen(QColor(theme::TEXT));
    QFont sf = font();
    sf.setPixelSize(12);
    sf.setWeight(QFont::Normal);

    for (const Header& h : headers_) {
        if (!h.rect.intersects(paintRect))
            continue;
        p.setFont(hf);
        p.setPen(QColor(theme::TEXT));
        p.drawText(h.rect, Qt::AlignLeft | Qt::AlignVCenter, h.label);
        QRectF sub = h.rect.adjusted(0, 30, 0, 0);
        p.setFont(sf);
        p.setPen(QColor(theme::TEXT_SECTION));
        p.drawText(sub, Qt::AlignLeft | Qt::AlignVCenter, h.subtitle);
    }

    // fotoğraf hücreleri
    for (const Cell& c : cells_) {
        if (!c.rect.intersects(paintRect))
            continue;
        const Photo& ph = photos_[c.index];

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(theme::PLACEHOLDER));
        p.drawRoundedRect(c.rect, 6, 6);

        const QImage img = thumbs_->get(ph.path, ph.size, rowH_);
        if (!img.isNull()) {
            p.save();
            QPainterPath clip;
            clip.addRoundedRect(c.rect, 6, 6);
            p.setClipPath(clip);
            p.drawImage(c.rect, img);
            p.restore();
        } else {
            const QString rk = ph.path + QLatin1Char('|') + QString::number(rowH_);
            if (!requested_.contains(rk)) {
                requested_.insert(rk);
                thumbs_->request(ph, rowH_);
            }
        }

        if (c.index == hover_ && c.index != current_) {
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(QColor(255, 255, 255, 70), 2));
            p.drawRoundedRect(c.rect.adjusted(1, 1, -1, -1), 6, 6);
        }
        if (c.index == current_) {
            p.setBrush(Qt::NoBrush);
            p.setPen(QPen(QColor(theme::ACCENT), 3));
            p.drawRoundedRect(c.rect.adjusted(1.5, 1.5, -1.5, -1.5), 6, 6);
        }
    }
}

void PhotoGrid::resizeEvent(QResizeEvent* e)
{
    if (!relayouting_ && width() != lastWidth_) {
        lastWidth_ = width();
        relayout();
    }
    QWidget::resizeEvent(e);
}

void PhotoGrid::mouseMoveEvent(QMouseEvent* e)
{
    const int hit = hitTest(e->position());
    if (hit != hover_) {
        hover_ = hit;
        setCursor(hit >= 0 ? Qt::PointingHandCursor : Qt::ArrowCursor);
        update();
    }
    QWidget::mouseMoveEvent(e);
}

void PhotoGrid::mousePressEvent(QMouseEvent* e)
{
    setFocus(Qt::MouseFocusReason);
    const int hit = hitTest(e->position());
    if (e->button() == Qt::RightButton) {
        if (hit >= 0) {
            if (hit != current_)
                setCurrentIndex(hit);
            emit contextMenuRequested(hit, e->globalPosition().toPoint());
        }
        return;
    }
    setCurrentIndex(hit);
    QWidget::mousePressEvent(e);
}

void PhotoGrid::mouseDoubleClickEvent(QMouseEvent* e)
{
    const int hit = hitTest(e->position());
    if (hit >= 0)
        emit photoActivated(hit);
    QWidget::mouseDoubleClickEvent(e);
}

void PhotoGrid::leaveEvent(QEvent* e)
{
    hover_ = -1;
    setCursor(Qt::ArrowCursor);
    update();
    QWidget::leaveEvent(e);
}

void PhotoGrid::keyPressEvent(QKeyEvent* e)
{
    if (photos_.isEmpty()) {
        QWidget::keyPressEvent(e);
        return;
    }
    if (current_ < 0) {
        if (e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter) {
            setCurrentIndex(0);
            ensureCurrentVisible();
            return;
        }
    }

    const int row = (current_ >= 0 && current_ < rowOfIndex_.size()) ? rowOfIndex_[current_] : -1;
    switch (e->key()) {
    case Qt::Key_Left:
        setCurrentIndex(qMax(0, current_ - 1));
        ensureCurrentVisible();
        return;
    case Qt::Key_Right:
        if (current_ < 0)
            setCurrentIndex(0);
        else
            setCurrentIndex(qMin(photos_.size() - 1, current_ + 1));
        ensureCurrentVisible();
        return;
    case Qt::Key_Up:
        if (row > 0)
            setCurrentIndex(rows_[row - 1].value(0, current_));
        ensureCurrentVisible();
        return;
    case Qt::Key_Down:
        if (row >= 0 && row + 1 < rows_.size())
            setCurrentIndex(rows_[row + 1].value(0, current_));
        else if (row < 0)
            setCurrentIndex(0);
        ensureCurrentVisible();
        return;
    case Qt::Key_Home:
        setCurrentIndex(0);
        ensureCurrentVisible();
        return;
    case Qt::Key_End:
        setCurrentIndex(photos_.size() - 1);
        ensureCurrentVisible();
        return;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    case Qt::Key_Space:
        if (current_ >= 0)
            emit photoActivated(current_);
        return;
    default:
        QWidget::keyPressEvent(e);
    }
}