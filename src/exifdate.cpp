#include "exifdate.h"

#include <cstring>

#include <QFile>
#include <QFileInfo>
#include <QList>

namespace {

quint16 rd16(const uchar* p, bool be)
{
    return be ? quint16((p[0] << 8) | p[1]) : quint16((p[1] << 8) | p[0]);
}

quint32 rd32(const uchar* p, bool be)
{
    return be ? quint32((quint32(p[0]) << 24) | (quint32(p[1]) << 16)
                        | (quint32(p[2]) << 8) | quint32(p[3]))
              : quint32((quint32(p[3]) << 24) | (quint32(p[2]) << 16)
                        | (quint32(p[1]) << 8) | quint32(p[0]));
}

// TIFF IFD içindeki etiketlerden tarih metnini arar.
QString findDateTag(const uchar* base, qint64 len, quint32 ifdOff, bool be,
                    const QList<quint16>& tags)
{
    if (ifdOff + 2 > quint32(len))
        return {};
    const quint16 count = rd16(base + ifdOff, be);
    for (quint16 i = 0; i < count; ++i) {
        const qint64 e = qint64(ifdOff) + 2 + qint64(i) * 12;
        if (e + 12 > len)
            break;
        const quint16 tag = rd16(base + e, be);
        if (!tags.contains(tag))
            continue;
        const quint32 valOff = rd32(base + e + 8, be);
        if (valOff + 19 >= quint32(len))
            return {};
        QString s = QString::fromLatin1(reinterpret_cast<const char*>(base + valOff), 19);
        return s;
    }
    return {};
}

QString parseExif(const uchar* base, qint64 len)
{
    if (len < 14 || memcmp(base, "Exif\0\0", 6) != 0)
        return {};
    const uchar* t = base + 6;
    const bool be = (memcmp(t, "MM", 2) == 0);
    if (!be && memcmp(t, "II", 2) != 0)
        return {};
    const quint32 ifd0 = rd32(t + 4, be);
    const qint64 tiffLen = len - 6;

    // Doğrudan IFD0'daki DateTime (0x0132)
    QString d = findDateTag(t, tiffLen, ifd0, be, {0x0132});
    if (!d.isEmpty())
        return d;

    // Exif alt IFD (0x8769) içinde DateTimeOriginal (0x9003)
    const quint16 count = rd16(t + ifd0, be);
    for (quint16 i = 0; i < count; ++i) {
        const qint64 e = qint64(ifd0) + 2 + qint64(i) * 12;
        if (e + 12 > tiffLen)
            break;
        if (rd16(t + e, be) == 0x8769) {
            const quint32 sub = rd32(t + e + 8, be);
            d = findDateTag(t, tiffLen, sub, be, {0x9003, 0x9004});
            if (!d.isEmpty())
                return d;
        }
    }
    return {};
}

} // namespace

QDateTime exifDate(const QString& filePath)
{
    QFile f(filePath);
    if (f.open(QIODevice::ReadOnly)) {
        const QByteArray head = f.read(512 * 1024);
        const uchar* base = reinterpret_cast<const uchar*>(head.constData());
        const qint64 len = head.size();

        // JPEG: APP1 segmentini bul
        if (len > 4 && base[0] == 0xFF && base[1] == 0xD8) {
            qint64 pos = 2;
            while (pos + 4 < len) {
                if (base[pos] != 0xFF)
                    break;
                const quint8 marker = base[pos + 1];
                const quint16 segLen = quint16((base[pos + 2] << 8) | base[pos + 3]);
                if (marker == 0xE1 && pos + 4 + 6 <= len) {
                    const QString d = parseExif(base + pos + 4, qMin<qint64>(segLen, len - pos - 4));
                    if (!d.isEmpty()) {
                        QDateTime dt = QDateTime::fromString(d, QStringLiteral("yyyy:MM:dd hh:mm:ss"));
                        if (dt.isValid())
                            return dt;
                    }
                }
                if (marker == 0xDA) // SOS: veri akışı başlar
                    break;
                pos += 2 + segLen;
                if (segLen < 2)
                    break;
            }
        }
        // HEIC/AVIF/PNG gibi diğerleri için tarih etiketi genelde bulunmaz
        if (len > 8 && memcmp(base, "Exif\0\0", 6) == 0) {
            const QString d = parseExif(base, len);
            if (!d.isEmpty()) {
                QDateTime dt = QDateTime::fromString(d, QStringLiteral("yyyy:MM:dd hh:mm:ss"));
                if (dt.isValid())
                    return dt;
            }
        }
    }
    return QFileInfo(filePath).lastModified();
}