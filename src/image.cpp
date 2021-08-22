#include "image.h"

#include "image_helpers.h"

Image::Image(QString _filename)
  : filename(std::move(_filename))
  , imageData(filename)
{
    fileWatcher.addPath(filename);
    connect(&fileWatcher, &QFileSystemWatcher::fileChanged, this, &Image::imageFileChanged);
}

QString const& Image::file() const&
{
    return filename;
}

QImage Image::image() const&
{
    QImage rotated;
    if (rotation != 0) {
        // Note that rotation by positive angles is clockwise here since Qt widget coordinates have their y axis
        // downwards.
        rotated = imageData.transformed(QMatrix().rotate(rotation * -90));
    } else {
        rotated = imageData;
    }

    return addTransparentOffset(rotated, offset);
}

QImage Image::toGrayscaleImage() const
{
    QImage originalImage = image();
    QImage grayscale = originalImage.convertToFormat(QImage::Format_Grayscale8);
    if (originalImage.hasAlphaChannel()) {
        // Preserve alpha channel in the grayscale image.
        grayscale.setAlphaChannel(originalImage.convertToFormat(QImage::Format_Alpha8));
    }

    return grayscale;
}

QPixmap Image::toPixmap() const
{
    return QPixmap::fromImage(image());
}

QPixmap Image::toGrayscalePixmap() const
{
    return QPixmap::fromImage(toGrayscaleImage());
}

QPixmap Image::toColorizedPixmap(QColor const& color) const
{
    return QPixmap::fromImage(colorizeGrayscaleImage(toGrayscaleImage(), color));
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
