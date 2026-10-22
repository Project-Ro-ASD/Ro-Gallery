#pragma once
#include <QString>
#include <QWidget>

// Tema renkleri ve Qt stil sayfası (karanlık tema, mavi vurgular).
namespace theme {

// Renkler
extern const QString BG;           // pencere zemini
extern const QString SIDEBAR;      // kenar çubuğu
extern const QString CONTENT;      // ana alan (siyah)
extern const QString PLACEHOLDER;  // küçük resim yüklenmeden önceki dolgu
extern const QString PILL;         // yüzen araç çubuğu zemini
extern const QString BORDER;       // ince çerçeveler
extern const QString HOVER;        // üzerine gelme
extern const QString SELECTED;     // seçili satır / buton
extern const QString TEXT;
extern const QString TEXT_DIM;
extern const QString TEXT_SECTION;
extern const QString ACCENT;       // mavi vurgu
extern const QString ACCENT_HOVER;
extern const QString ACCENT_DOWN;

// Uygulama genelinde stil sayfası
QString stylesheet();

} // namespace theme