#include "theme.h"

namespace theme {

const QString BG          = "#0b0b0d";
const QString SIDEBAR     = "#131316";
const QString CONTENT     = "#000000";
const QString PLACEHOLDER = "#141418";
const QString PILL        = "#1b1b20";
const QString BORDER      = "#2d2d35";
const QString HOVER       = "#22222a";
const QString SELECTED    = "#2b2b33";
const QString TEXT        = "#f4f4f7";
const QString TEXT_DIM    = "#9a9aa4";
const QString TEXT_SECTION= "#6c6c76";
const QString ACCENT      = "#3b82f6";
const QString ACCENT_HOVER= "#5c9bf8";
const QString ACCENT_DOWN = "#2563eb";

QString stylesheet()
{
    return QStringLiteral(R"(
* { font-family: "Inter", "Noto Sans", "DejaVu Sans", sans-serif; }
QWidget { color: #f4f4f7; background: transparent; font-size: 13px; }

#sidebar { background: #131316; border-top-left-radius: 16px; border-bottom-left-radius: 16px; }
#content  { background: #000000; border-top-right-radius: 16px; border-bottom-right-radius: 16px; }

#appTitle { font-size: 27px; font-weight: 700; color: #ffffff; }

#navButton {
    text-align: left; padding: 7px 10px; border-radius: 9px;
    background: transparent; border: none; color: #d5d5dc; font-size: 13px;
}
#navButton:hover { background: #22222a; color: #ffffff; }
#navButton[active="true"] { background: #2b2b33; color: #ffffff; font-weight: 600; }

#sectionLabel { color: #6c6c76; font-size: 11px; font-weight: 600; }
#sectionButton { background: transparent; border: none; border-radius: 7px; color: #6c6c76; padding: 2px 5px; }
#sectionButton:hover { background: #22222a; color: #d5d5dc; }

#segContainer { background: #1b1b20; border: 1px solid #2d2d35; border-radius: 21px; }
#segButton {
    background: transparent; border: none; border-radius: 16px;
    color: #9a9aa4; padding: 7px 15px; font-size: 12px; font-weight: 600;
}
#segButton:hover { color: #f4f4f7; }
#segButton[active="true"] { background: #f4f4f7; color: #101014; }

#pill { background: #1b1b20; border: 1px solid #2d2d35; border-radius: 21px; }
#iconButton { background: transparent; border: none; border-radius: 16px; color: #d5d5dc; }
#iconButton:hover { background: #2b2b33; color: #ffffff; }
#iconButton:pressed { background: #35353e; }

#accentButton {
    background: #3b82f6; color: #ffffff; border: none; border-radius: 21px;
    padding: 9px 17px; font-size: 12px; font-weight: 600;
}
#accentButton:hover { background: #5c9bf8; }
#accentButton:pressed { background: #2563eb; }

#searchPill { background: #3b82f6; border-radius: 21px; }
#searchEdit {
    background: transparent; border: none; color: #ffffff;
    font-size: 12px; font-weight: 600; selection-background-color: #2563eb;
}
#searchEdit::placeholder { color: #d4e4fd; }

#closeButton {
    background: transparent; border: 1px solid #2d2d35; border-radius: 17px; color: #9a9aa4;
}
#closeButton:hover { background: #22222a; color: #ffffff; }

QSlider::groove:horizontal {
    height: 4px; background: #3a3a44; border-radius: 2px;
}
QSlider::sub-page:horizontal { background: #3a3a44; border-radius: 2px; }
QSlider::handle:horizontal {
    width: 13px; height: 13px; margin: -5px 0;
    background: #d8d8de; border-radius: 7px;
}
QSlider::handle:horizontal:hover { background: #ffffff; }

QScrollBar:vertical { background: transparent; width: 10px; margin: 2px 3px 2px 0; }
QScrollBar::handle:vertical { background: #2b2b33; border-radius: 5px; min-height: 32px; }
QScrollBar::handle:vertical:hover { background: #3a3a44; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; width: 0; }
QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical { background: transparent; }
QScrollBar:horizontal { background: transparent; height: 10px; margin: 0 2px 0 2px; }
QScrollBar::handle:horizontal { background: #2b2b33; border-radius: 5px; min-width: 32px; }
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal { height: 0; width: 0; }
QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal { background: transparent; }

QMenu {
    background: #1b1b20; border: 1px solid #2d2d35; border-radius: 10px; padding: 6px;
}
QMenu::item { padding: 7px 16px; border-radius: 7px; color: #e8e8ee; background: transparent; }
QMenu::item:selected { background: #2b2b33; color: #ffffff; }
QMenu::separator { height: 1px; background: #2d2d35; margin: 5px 8px; }

QToolTip {
    background: #2b2b33; color: #f4f4f7; border: 1px solid #3a3a44;
    padding: 4px 8px; border-radius: 6px;
}

#statusPill {
    background: #1b1b20; border: 1px solid #2d2d35; border-radius: 15px;
    color: #9a9aa4; padding: 6px 14px; font-size: 11px;
}

#emptyTitle { font-size: 18px; font-weight: 600; color: #f4f4f7; }
#emptyText  { font-size: 12px; color: #9a9aa4; }

#viewerBar {
    background: #16161a; border: 1px solid #2d2d35; border-radius: 23px;
}
#viewerFile { font-size: 13px; font-weight: 600; color: #ffffff; background: transparent; }
#viewerMeta { font-size: 11px; color: #9a9aa4; background: transparent; }
#navArrow {
    background: #1b1b20; border: 1px solid #2d2d35; border-radius: 21px; color: #f4f4f7;
}
#navArrow:hover { background: #2b2b33; }
#navArrow:pressed { background: #35353e; }

QScrollArea { border: none; }
QAbstractScrollArea::pane { border: none; }
)");
}

} // namespace theme