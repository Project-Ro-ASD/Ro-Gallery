#pragma once
#include <QFrame>
#include <QHash>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>

// Üstteki yüzen araç çubuğu: Years/Months/Days, zoom, Upload, Search, kapat.
class TopBar : public QFrame
{
    Q_OBJECT
public:
    explicit TopBar(QWidget* parent = nullptr);

    void setGrouping(const QString& grouping);
    void setZoom(int rowHeight);
    QString searchText() const;
    void setSearchText(const QString& text);

signals:
    void groupingChanged(const QString& grouping);
    void zoomChanged(int rowHeight);
    void uploadRequested();
    void searchChanged(const QString& text);
    void clearRequested();

private:
    void applySegStyles();
    static int rowHeightToSlider(int rowHeight);
    static int sliderToRowHeight(int value);

    QHash<QString, QPushButton*> seg_;
    QSlider* zoom_ = nullptr;
    QLineEdit* search_ = nullptr;
    QString grouping_ = QStringLiteral("months");
};