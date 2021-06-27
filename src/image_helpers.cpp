#include "image_helpers.h"

#include <QPainter>

QImage addTransparentOffset(QImage const& image, QPoint const& offset)
{
    if (offset.isNull()) {
        return image;
    }

    QImage newImage(image.size() + QSize(offset.x(), offset.y()), QImage::Format_ARGB32);
    newImage.fill(qRgba(0, 0, 0, 0));

    QPainter painter(&newImage);
    painter.drawImage(offset, image);

    return newImage;
}
