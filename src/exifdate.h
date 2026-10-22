#pragma once
#include <QDateTime>
#include <QString>

// JPEG EXIF DateTimeOriginal/DateTime okuma (bağımlılık gerektirmez).
// Okunamazsa dosyanın değişiklik zamanını döndürür.
QDateTime exifDate(const QString& filePath);