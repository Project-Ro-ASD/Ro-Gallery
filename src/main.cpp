#include <QApplication>
#include <QCommandLineParser>
#include <QFont>
#include <QFontInfo>
#include <QTimer>

#include "icons.h"
#include "library.h"
#include "mainwindow.h"
#include "theme.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    QApplication::setApplicationName(QStringLiteral("ro-gallery"));
    QApplication::setApplicationDisplayName(QStringLiteral("Ro-Gallery"));
    QApplication::setApplicationVersion(QStringLiteral(RO_GALLERY_VERSION));
    QApplication::setOrganizationName(QStringLiteral("Ro-ASD"));
    QApplication::setDesktopFileName(QStringLiteral("ro-gallery"));
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Ro-Gallery photo library and viewer"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption({QStringLiteral("size"), QStringLiteral("Window size (WIDTHxHEIGHT)"), QStringLiteral("size")});
    parser.addOption({QStringLiteral("screenshot"), QStringLiteral("Save window screenshot and exit"), QStringLiteral("file")});
    parser.addOption({QStringLiteral("viewer-shot"), QStringLiteral("Save viewer screenshot and exit"), QStringLiteral("file")});
    parser.process(app);
    app.setWindowIcon(icons::appIcon());

    QFont font(QStringLiteral("Inter"), 10);
    if (QFontInfo(font).family() != QLatin1String("Inter"))
        font = QFont(QStringLiteral("Noto Sans"), 10);
    app.setFont(font);
    app.setStyleSheet(theme::stylesheet());

    qRegisterMetaType<Photo>("Photo");
    qRegisterMetaType<QVector<Photo>>("QVector<Photo>");

    MainWindow w;
    w.show();

    // Test yardımcıları: --screenshot <dosya>, --viewer-shot <dosya>, --size BxY
    const QStringList args = app.arguments();
    const int sizeIdx = args.indexOf(QStringLiteral("--size"));
    if (sizeIdx >= 0 && sizeIdx + 1 < args.size()) {
        const QStringList wh = args.at(sizeIdx + 1).split(QLatin1Char('x'));
        if (wh.size() == 2) {
            w.resize(wh.at(0).toInt(), wh.at(1).toInt());
            w.setFixedSize(wh.at(0).toInt(), wh.at(1).toInt());
        }
    }
    const int shotIdx = args.indexOf(QStringLiteral("--screenshot"));
    const int viewerIdx = args.indexOf(QStringLiteral("--viewer-shot"));

    if (viewerIdx >= 0 && viewerIdx + 1 < args.size()) {
        const QString out = args.at(viewerIdx + 1);
        QTimer::singleShot(1500, &w, [&w]() { w.debugOpenFirstPhoto(); });
        QTimer::singleShot(4500, &w, [&w, out]() {
            const bool saved = w.grab().save(out);
            qInfo("kaydedildi: %s", qUtf8Printable(out));
            w.close();
            QApplication::exit(saved ? 0 : 1);
        });
    } else if (shotIdx >= 0 && shotIdx + 1 < args.size()) {
        const QString out = args.at(shotIdx + 1);
        QTimer::singleShot(3500, &w, [&w, out]() {
            const bool saved = w.grab().save(out);
            qInfo("kaydedildi: %s", qUtf8Printable(out));
            w.close();
            QApplication::exit(saved ? 0 : 1);
        });
    }

    return app.exec();
}
