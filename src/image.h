#pragma once

#include <QImage>
#include <QPixmap>

class Image {
public:
    explicit Image(QString _filename)
      : filename(std::move(_filename))
      , imageData(filename)
    {
    }

    QString const& file() const&
    {
        return filename;
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
    QString filename;
    QImage imageData;
};
