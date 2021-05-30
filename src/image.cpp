#include "image.h"

Image::Image(QString _filename)
  : filename(std::move(_filename))
  , imageData(filename)
{
    fileWatcher.addPath(filename);
    connect(&fileWatcher, &QFileSystemWatcher::fileChanged, this, &Image::imageFileChanged);
}

Image::~Image() = default;

QString const& Image::file() const&
{
    return filename;
}

QImage const& Image::image() const&
{
    return imageData;
}

QPixmap Image::toPixmap() const
{
    return QPixmap::fromImage(imageData);
}

QPixmap Image::toGrayscalePixmap() const
{
    QImage grayscale = imageData.convertToFormat(QImage::Format_Grayscale8);
    if (imageData.hasAlphaChannel()) {
        // Preserve alpha channel in the grayscale image.
        grayscale.setAlphaChannel(imageData.convertToFormat(QImage::Format_Alpha8));
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

void Image::imageFileChanged()
{
    if (reloadWhenFileChanges) {
        reload();
    }
}
