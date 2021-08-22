#pragma once

#include <QFileSystemWatcher>
#include <QImage>
#include <QPixmap>
#include <QStringList>

class Image : public QObject {
    Q_OBJECT

public:
    explicit Image(QString _filename);

    QString const& file() const&;
    QImage image() const&;
    QImage toGrayscaleImage() const;

    QPixmap toPixmap() const;
    QPixmap toGrayscalePixmap() const;
    QPixmap toColorizedPixmap(QColor const& color) const;

    void setReloadWhenFileChanges(bool enable);

    QPoint getOffset() const;
    void setOffset(QPoint const& _offset);

signals:
    void imageChanged();
    void imageClosed();
    void imageReplaced(QStringList const& files);

public slots:
    void reload();

    void resetRotation();
    void rotateLeft();
    void rotateRight();

private slots:
    void imageFileChanged();

private:
    QString filename;
    QImage imageData;

    QPoint offset;

    int rotation = 0;  // Rotation in 90° degree steps.

    bool reloadWhenFileChanges = false;
    QFileSystemWatcher fileWatcher;
};
