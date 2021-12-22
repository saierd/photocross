#include "image.h"

#include "image_helpers.h"

#include <QFileInfo>

struct Image::Cache {
    QImage image;
    QImage grayscale;

    QPixmap pixmap;
    QPixmap grayscalePixmap;

    QColor colorizedPixmapColor;
    QPixmap colorizedPixmap;

    void invalidate()
    {
        *this = Cache();
    }
};

Image::Image(QString _filename)
  : filename(std::move(_filename))
  , imageData(filename)
{
    fileWatcher.addPath(filename);
    connect(&fileWatcher, &QFileSystemWatcher::fileChanged, this, &Image::imageFileChanged);

    // We will save converted versions of the image in the cache to avoid recomputing them whenever the display of the
    // image in the UI changes.
    cache = std::make_unique<Cache>();

    // Clear the cache when the image changes. Note that this must be the first slot that gets connected to the
    // imageChanged signal so that any other slots reacting to the event can get refreshed images. Qt guarantees that
    // slots are called in the order they are connected.
    connect(this, &Image::imageChanged, [this]() {
        if (cache) {
            cache->invalidate();
        }
    });
}

Image::~Image() = default;

QString const& Image::file() const
{
    return filename;
}

QString Image::canonicalFilename() const
{
    return QFileInfo(filename).canonicalFilePath();
}

QSize Image::size() const
{
    return imageData.size();
}

QImage Image::image() const
{
    if (!cache->image.isNull()) {
        return cache->image;
    }

    QImage rotated;
    if (rotation != 0) {
        // Note that rotation by positive angles is clockwise here since Qt widget coordinates have their y axis
        // downwards.
        rotated = imageData.transformed(QMatrix().rotate(rotation * -90));
    } else {
        rotated = imageData;
    }

    cache->image = addTransparentOffset(rotated, offset);
    return cache->image;
}

QImage Image::toGrayscaleImage() const
{
    if (!cache->grayscale.isNull()) {
        return cache->grayscale;
    }

    QImage originalImage = image();
    QImage grayscale = originalImage.convertToFormat(QImage::Format_Grayscale8);
    if (originalImage.hasAlphaChannel()) {
        // Preserve alpha channel in the grayscale image.
        grayscale.setAlphaChannel(originalImage.convertToFormat(QImage::Format_Alpha8));
    }

    cache->grayscale = grayscale;
    return cache->grayscale;
}

QPixmap Image::toPixmap() const
{
    if (cache->pixmap.isNull()) {
        cache->pixmap = QPixmap::fromImage(image());
    }
    return cache->pixmap;
}

QPixmap Image::toGrayscalePixmap() const
{
    if (cache->grayscalePixmap.isNull()) {
        cache->grayscalePixmap = QPixmap::fromImage(toGrayscaleImage());
    }
    return cache->grayscalePixmap;
}

QPixmap Image::toColorizedPixmap(QColor const& color) const
{
    if (!color.isValid()) {
        return toPixmap();
    }

    if (!cache->colorizedPixmap.isNull() && cache->colorizedPixmapColor == color) {
        return cache->colorizedPixmap;
    }

    auto colorized = QPixmap::fromImage(colorizeGrayscaleImage(toGrayscaleImage(), color));
    cache->colorizedPixmap = colorized;
    cache->colorizedPixmapColor = color;

    return cache->colorizedPixmap;
}

void Image::setReloadWhenFileChanges(bool enable)
{
    reloadWhenFileChanges = enable;
}

QPoint Image::getOffset() const
{
    return offset;
}

void Image::setOffset(QPoint const& _offset)
{
    offset = _offset;
    emit imageChanged();
}

void Image::reload()
{
    imageData = QImage(filename);
    emit imageChanged();
}

void Image::resetRotation()
{
    if (rotation == 0) {
        return;
    }

    rotation = 0;
    emit imageChanged();
}

void Image::rotateLeft()
{
    rotation = (rotation + 1) % 4;
    emit imageChanged();
}

void Image::rotateRight()
{
    rotation = (rotation - 1) % 4;
    emit imageChanged();
}

void Image::imageFileChanged()
{
    if (reloadWhenFileChanges) {
        reload();
    }
}
