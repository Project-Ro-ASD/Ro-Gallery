#pragma once
#include <QImage>
#include <QScrollArea>
#include <QSet>
#include <QWidget>

#include "library.h"
#include "thumbs.h"

// Tarih gruplarına göre, satırları sayfaya tam oturan (justified) fotoğraf ızgarası.
class PhotoGrid : public QWidget
{
    Q_OBJECT
public:
    explicit PhotoGrid(ThumbCache* thumbs, QWidget* parent = nullptr);

    void setGroups(const QVector<library::Group>& groups);
    void setRowHeight(int h);
    int rowHeight() const { return rowH_; }

    int currentIndex() const { return current_; }
    void setCurrentIndex(int idx);
    void clearSelection();
    void scrollToIndex(int idx);
    void setScrollArea(QScrollArea* scroll) { scroll_ = scroll; }

signals:
    void photoActivated(int index);
    void contextMenuRequested(int index, const QPoint& globalPos);
    void indexChanged(int index);

protected:
    void paintEvent(QPaintEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void leaveEvent(QEvent* e) override;

private:
    struct Cell {
        QRectF rect;
        int index;
    };
    struct Header {
        QRectF rect;
        QString label;
        QString subtitle;
    };

    void relayout();
    int hitTest(const QPointF& pos) const;
    void ensureCurrentVisible();
    void scheduleUpdate();

    ThumbCache* thumbs_ = nullptr;
    QScrollArea* scroll_ = nullptr;
    QVector<library::Group> groups_;
    QVector<Photo> photos_;
    QVector<Cell> cells_;
    QVector<Header> headers_;
    QVector<int> rowOfIndex_;
    QVector<QVector<int>> rows_;

    int rowH_ = 190;
    double gap_ = 6.0;
    double marginX_ = 22.0;
    double totalH_ = 0.0;
    int lastWidth_ = -1;

    int current_ = -1;
    int hover_ = -1;
    bool relayouting_ = false;
    bool updatePending_ = false;
    QSet<QString> requested_;
};