#include "image.h"

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
    if (rotation == 0) {
        return imageData;
    }

    // Note that rotation by positive angles is clockwise here since Qt widget coordinates have their y axis downwards.
    return imageData.transformed(QMatrix().rotate(rotation * -90));
}

QPixmap Image::toPixmap() const
{
    return QPixmap::fromImage(image());
}

QPixmap Image::toGrayscalePixmap() const
{
    QImage originalImage = image();
    QImage grayscale = originalImage.convertToFormat(QImage::Format_Grayscale8);
    if (originalImage.hasAlphaChannel()) {
        // Preserve alpha channel in the grayscale image.
        grayscale.setAlphaChannel(originalImage.convertToFormat(QImage::Format_Alpha8));
    }

    return QPixmap::fromImage(grayscale);
}

void Image::setReloadWhenFileChanges(bool enable)
{
    reloadWhenFileChanges = enable;
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
