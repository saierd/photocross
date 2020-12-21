#pragma once

#include <QFileSystemWatcher>
#include <QImage>
#include <QPixmap>

class Image : public QObject {
    Q_OBJECT

public:
    explicit Image(QString _filename);
    ~Image() override;

    QString const& file() const&;
    QImage const& image() const&;

    QPixmap toPixmap() const;
    QPixmap toGrayscalePixmap() const;

    void setReloadWhenFileChanges(bool enable);

signals:
    void imageChanged() const;

public slots:
    void reload();

private slots:
    void imageFileChanged();

private:
    QString filename;
    QImage imageData;

    bool reloadWhenFileChanges = false;
    QFileSystemWatcher fileWatcher;
};
