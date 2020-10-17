#pragma once

#include <QImage>
#include <QPixmap>

class Image {
public:
    explicit Image(QImage const& image)
      : imageData(image)
    {
    }

    QImage const& image() const&
    {
        return imageData;
    }

    QPixmap toPixmap() const
    {
        return QPixmap::fromImage(imageData);
    }

    QPixmap toGrayscalePixmap() const
    {
        return QPixmap::fromImage(imageData.convertToFormat(QImage::Format_Grayscale8));
    }

private:
    QImage imageData;
};
