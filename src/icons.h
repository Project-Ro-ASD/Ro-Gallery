#pragma once
#include <QColor>
#include <QIcon>
#include <QPixmap>
#include <QString>

// QPainter ile çizilen küçük simgeler (harici SVG dosyası gerekmez).
namespace icons {

// 16 birimlik koordinat alanında çizilir, istenen piksel boyutuna ölçeklenir.
QPixmap pixmap(const QString& name, int size = 16,
               const QColor& color = QColor("#d5d5dc"));
QIcon icon(const QString& name, int size = 16,
           const QColor& color = QColor("#d5d5dc"));

// Uygulama simgesi (mavi yuvarlak kare + beyaz fotoğraf işareti).
QIcon appIcon();

} // namespace icons