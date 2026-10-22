#include "icons.h"

#include <QHash>
#include <QPainter>
#include <QPainterPath>
#include <QPen>
#include <QtMath>

namespace icons {

using Drawer = void (*)(QPainter&);

static void dLibrary(QPainter& p)
{
    p.drawRoundedRect(QRectF(2.6, 2.6, 4.9, 4.9), 1.2, 1.2);
    p.drawRoundedRect(QRectF(8.5, 2.6, 4.9, 4.9), 1.2, 1.2);
    p.drawRoundedRect(QRectF(2.6, 8.5, 4.9, 4.9), 1.2, 1.2);
    p.drawRoundedRect(QRectF(8.5, 8.5, 4.9, 4.9), 1.2, 1.2);
}

static void dAlbums(QPainter& p)
{
    p.drawRoundedRect(QRectF(6.2, 2.4, 7.4, 7.4), 1.5, 1.5);
    p.drawRoundedRect(QRectF(2.4, 6.2, 7.4, 7.4), 1.5, 1.5);
}

static void dHeart(QPainter& p)
{
    QPainterPath h;
    h.moveTo(8, 13.6);
    h.cubicTo(3.2, 10.4, 1.8, 7.6, 1.8, 5.7);
    h.cubicTo(1.8, 3.6, 3.4, 2.2, 5.3, 2.2);
    h.cubicTo(6.6, 2.2, 7.6, 2.9, 8, 3.9);
    h.cubicTo(8.4, 2.9, 9.4, 2.2, 10.7, 2.2);
    h.cubicTo(12.6, 2.2, 14.2, 3.6, 14.2, 5.7);
    h.cubicTo(14.2, 7.6, 12.8, 10.4, 8, 13.6);
    p.drawPath(h);
}

static void dHeartFilled(QPainter& p)
{
    QPainterPath h;
    h.moveTo(8, 13.6);
    h.cubicTo(3.2, 10.4, 1.8, 7.6, 1.8, 5.7);
    h.cubicTo(1.8, 3.6, 3.4, 2.2, 5.3, 2.2);
    h.cubicTo(6.6, 2.2, 7.6, 2.9, 8, 3.9);
    h.cubicTo(8.4, 2.9, 9.4, 2.2, 10.7, 2.2);
    h.cubicTo(12.6, 2.2, 14.2, 3.6, 14.2, 5.7);
    h.cubicTo(14.2, 7.6, 12.8, 10.4, 8, 13.6);
    p.setBrush(QBrush(p.pen().color()));
    p.drawPath(h);
}

static void dPhoto(QPainter& p)
{
    p.drawRoundedRect(QRectF(2, 3.2, 12, 9.6), 1.6, 1.6);
    p.setBrush(QBrush(p.pen().color()));
    p.drawEllipse(QPointF(5.4, 6.2), 1.0, 1.0);
    p.setBrush(Qt::NoBrush);
    QPainterPath m;
    m.moveTo(3.2, 11.4);
    m.lineTo(6.6, 8.2);
    m.lineTo(8.8, 10.3);
    m.lineTo(10.6, 8.8);
    m.lineTo(12.9, 11.4);
    p.drawPath(m);
}

static void dVideo(QPainter& p)
{
    p.drawRoundedRect(QRectF(1.8, 3.6, 12.4, 8.8), 2, 2);
    QPolygonF tri;
    tri << QPointF(6.6, 6.4) << QPointF(6.6, 9.6) << QPointF(9.6, 8.0);
    p.setBrush(QBrush(p.pen().color()));
    p.drawPolygon(tri);
}

static void dTrash(QPainter& p)
{
    p.drawLine(QPointF(2.8, 4.6), QPointF(13.2, 4.6));
    p.drawLine(QPointF(6.2, 4.6), QPointF(6.2, 3.1));
    p.drawLine(QPointF(9.8, 4.6), QPointF(9.8, 3.1));
    p.drawLine(QPointF(6.2, 3.1), QPointF(9.8, 3.1));
    p.drawRoundedRect(QRectF(3.8, 4.6, 8.4, 8.8), 1.6, 1.6);
    p.drawLine(QPointF(6.4, 6.8), QPointF(6.4, 11.2));
    p.drawLine(QPointF(9.6, 6.8), QPointF(9.6, 11.2));
}

static void dSource(QPainter& p)
{
    p.drawRoundedRect(QRectF(2, 4.2, 12, 7.6), 2, 2);
    p.drawLine(QPointF(6.9, 8.0), QPointF(11.3, 8.0));
    p.setBrush(QBrush(p.pen().color()));
    p.drawEllipse(QPointF(4.5, 8.0), 0.95, 0.95);
}

static void dPhone(QPainter& p)
{
    p.drawRoundedRect(QRectF(4.6, 1.8, 6.8, 12.4), 1.9, 1.9);
    p.drawLine(QPointF(6.9, 3.5), QPointF(9.1, 3.5));
    p.drawLine(QPointF(6.7, 12.5), QPointF(9.3, 12.5));
}

static void dSpark(QPainter& p)
{
    QPainterPath s;
    s.moveTo(8, 1.6);
    s.cubicTo(8.6, 4.6, 11.4, 7.4, 14.4, 8);
    s.cubicTo(11.4, 8.6, 8.6, 11.4, 8, 14.4);
    s.cubicTo(7.4, 11.4, 4.6, 8.6, 1.6, 8);
    s.cubicTo(4.6, 7.4, 7.4, 4.6, 8, 1.6);
    p.setBrush(QBrush(p.pen().color()));
    p.drawPath(s);
}

static void dPanorama(QPainter& p)
{
    p.drawRoundedRect(QRectF(1.6, 4.2, 12.8, 7.6), 2, 2);
    QPainterPath w;
    w.moveTo(3.2, 9.6);
    w.cubicTo(5.2, 6.6, 7.2, 6.6, 8.6, 8.6);
    w.cubicTo(9.9, 10.4, 11.4, 10.2, 12.9, 8.4);
    p.drawPath(w);
}

static void dScreenshot(QPainter& p)
{
    p.drawPolyline(QPolygonF() << QPointF(2.2, 5.6) << QPointF(2.2, 2.4) << QPointF(5.6, 2.4));
    p.drawPolyline(QPolygonF() << QPointF(10.4, 2.4) << QPointF(13.8, 2.4) << QPointF(13.8, 5.6));
    p.drawPolyline(QPolygonF() << QPointF(13.8, 10.4) << QPointF(13.8, 13.6) << QPointF(10.4, 13.6));
    p.drawPolyline(QPolygonF() << QPointF(5.6, 13.6) << QPointF(2.2, 13.6) << QPointF(2.2, 10.4));
    p.drawRoundedRect(QRectF(5.4, 5.4, 5.2, 5.2), 1, 1);
}

static void d360(QPainter& p)
{
    p.drawArc(QRectF(2.6, 2.6, 10.8, 10.8), 55 * 16, 295 * 16);
    QPolygonF head;
    head << QPointF(11.0, 3.9) << QPointF(14.0, 5.1) << QPointF(11.8, 7.6);
    p.setBrush(QBrush(p.pen().color()));
    p.drawPolygon(head);
}

static void dPlus(QPainter& p)
{
    p.drawLine(QPointF(8, 3), QPointF(8, 13));
    p.drawLine(QPointF(3, 8), QPointF(13, 8));
}

static void dMinus(QPainter& p)
{
    p.drawLine(QPointF(3.2, 8), QPointF(12.8, 8));
}

static void dChevron(QPainter& p)
{
    p.drawPolyline(QPolygonF() << QPointF(6, 3.6) << QPointF(10.6, 8) << QPointF(6, 12.4));
}

static void dSearch(QPainter& p)
{
    p.drawEllipse(QPointF(7, 7), 4.4, 4.4);
    p.drawLine(QPointF(10.3, 10.3), QPointF(13.9, 13.9));
}

static void dUpload(QPainter& p)
{
    p.drawPolyline(QPolygonF() << QPointF(2.6, 9.8) << QPointF(2.6, 13.2)
                               << QPointF(13.4, 13.2) << QPointF(13.4, 9.8));
    p.drawLine(QPointF(8, 10.4), QPointF(8, 3));
    p.drawPolyline(QPolygonF() << QPointF(5.4, 5.6) << QPointF(8, 3) << QPointF(10.6, 5.6));
}

static void dClose(QPainter& p)
{
    p.drawLine(QPointF(4.2, 4.2), QPointF(11.8, 11.8));
    p.drawLine(QPointF(11.8, 4.2), QPointF(4.2, 11.8));
}

static void dFolder(QPainter& p)
{
    QPainterPath f;
    f.moveTo(2.2, 12.8);
    f.lineTo(2.2, 4.2);
    f.lineTo(6.2, 4.2);
    f.lineTo(7.5, 5.9);
    f.lineTo(13.8, 5.9);
    f.lineTo(13.8, 12.8);
    f.closeSubpath();
    p.drawPath(f);
}

static void dStar(QPainter& p)
{
    QPolygonF s;
    for (int i = 0; i < 10; ++i) {
        double ang = -M_PI / 2 + i * M_PI / 5;
        double r = (i % 2 == 0) ? 6.6 : 2.8;
        s << QPointF(8 + r * qCos(ang), 8 + r * qSin(ang));
    }
    p.setBrush(QBrush(p.pen().color()));
    p.drawPolygon(s);
}

static void dRestore(QPainter& p)
{
    p.drawArc(QRectF(2.6, 2.6, 10.8, 10.8), 300 * 16, 240 * 16);
    QPolygonF head;
    head << QPointF(4.4, 2.2) << QPointF(2.2, 5.4) << QPointF(5.6, 6.0);
    p.setBrush(QBrush(p.pen().color()));
    p.drawPolygon(head);
}

static void dOpen(QPainter& p)
{
    p.drawRoundedRect(QRectF(2.4, 2.4, 11.2, 11.2), 1.8, 1.8);
    p.drawLine(QPointF(6, 10), QPointF(10.6, 5.4));
    p.drawPolyline(QPolygonF() << QPointF(7.0, 5.2) << QPointF(10.8, 5.2) << QPointF(10.8, 9.0));
}

static QHash<QString, Drawer>& drawers()
{
    static QHash<QString, Drawer> d = {
        {"library", &dLibrary},   {"albums", &dAlbums},
        {"heart", &dHeart},       {"heart_filled", &dHeartFilled},
        {"photo", &dPhoto},       {"video", &dVideo},
        {"trash", &dTrash},       {"source", &dSource},
        {"phone", &dPhone},       {"spark", &dSpark},
        {"panorama", &dPanorama}, {"screenshot", &dScreenshot},
        {"360", &d360},           {"plus", &dPlus},
        {"minus", &dMinus},       {"chevron", &dChevron},
        {"search", &dSearch},     {"upload", &dUpload},
        {"close", &dClose},       {"folder", &dFolder},
        {"star", &dStar},         {"restore", &dRestore},
        {"open", &dOpen},
    };
    return d;
}

QPixmap pixmap(const QString& name, int size, const QColor& color)
{
    static QHash<QString, QPixmap> cache;
    const QString key = name + QLatin1Char('/') + QString::number(size)
                        + QLatin1Char('/') + color.name();
    auto it = cache.constFind(key);
    if (it != cache.constEnd())
        return it.value();

    QPixmap pm(size * 2, size * 2);
    pm.fill(Qt::transparent);
    {
        QPainter p(&pm);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.scale(2.0, 2.0);
        p.setPen(QPen(color, 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.setBrush(Qt::NoBrush);
        Drawer fn = drawers().value(name, nullptr);
        if (fn)
            fn(p);
    }
    pm.setDevicePixelRatio(2.0);
    cache.insert(key, pm);
    return pm;
}

QIcon icon(const QString& name, int size, const QColor& color)
{
    return QIcon(pixmap(name, size, color));
}

QIcon appIcon()
{
    QPixmap pm(128, 128);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing, true);

    QLinearGradient g(0, 0, 128, 128);
    g.setColorAt(0, QColor(QStringLiteral("#3b82f6")));
    g.setColorAt(1, QColor(QStringLiteral("#2563eb")));
    p.setPen(Qt::NoPen);
    p.setBrush(g);
    p.drawRoundedRect(QRectF(8, 8, 112, 112), 28, 28);

    QPen pen(Qt::white, 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p.setPen(pen);
    p.setBrush(Qt::NoBrush);
    p.drawRoundedRect(QRectF(30, 36, 68, 56), 10, 10);

    p.setBrush(Qt::white);
    p.drawEllipse(QPointF(48, 53), 6, 6);

    QPainterPath m;
    m.moveTo(35, 84);
    m.lineTo(54, 64);
    m.lineTo(68, 77);
    m.lineTo(80, 68);
    m.lineTo(94, 84);
    m.closeSubpath();
    p.setPen(Qt::NoPen);
    p.drawPath(m);
    p.end();
    return QIcon(pm);
}

} // namespace icons