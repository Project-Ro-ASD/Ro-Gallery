#pragma once
#include <QFrame>
#include <QImage>
#include <QLabel>
#include <QPushButton>

#include "library.h"
#include "thumbs.h"

// Yakınlaştırılabilir, kaydırılabilir resim alanı.
class ImageView : public QWidget
{
    Q_OBJECT
public:
    explicit ImageView(QWidget* parent = nullptr);

    void setImage(const QImage& img);
    void zoomIn();
    void zoomOut();
    void fitToWindow();
    void actualSize();
    int scalePercent() const;

signals:
    void scaleChanged(int percent);

protected:
    void paintEvent(QPaintEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    void zoomAt(const QPointF& pos, double factor);

    QImage img_;
    double scale_ = 1.0;
    QPointF offset_;
    bool fitted_ = true;
    QPoint lastPos_;
};

// Fotoğraf görüntüleyici: dosya bilgisi, gezinme, yakınlaştırma, favori, silme.
class PhotoViewer : public QFrame
{
    Q_OBJECT
public:
    explicit PhotoViewer(ThumbCache* thumbs, QWidget* parent = nullptr);

    void showPhoto(const Photo& photo, int index, int total, bool favorite,
                   bool deletedView);

signals:
    void closed();
    void navigateRequested(int delta);
    void favoriteToggled(const QString& path);
    void deleteRequested(const QString& path);
    void restoreRequested(const QString& path);
    void openExternalRequested(const QString& path);

protected:
    void keyPressEvent(QKeyEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;

private:
    void updateFavButton();

    ThumbCache* thumbs_ = nullptr;
    ImageView* view_ = nullptr;
    QLabel* fileLabel_ = nullptr;
    QLabel* metaLabel_ = nullptr;
    QLabel* zoomLabel_ = nullptr;
    QPushButton* favButton_ = nullptr;
    QPushButton* deleteButton_ = nullptr;
    QWidget* bottomBar_ = nullptr;
    Photo photo_;
    bool favorite_ = false;
    bool deletedView_ = false;
};