#include "image.h"

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

QImage colorizeGrayscaleImage(QImage const& image, QColor const& color)
{
    if (!color.isValid()) {
        return image;
    }

    QImage target = image.convertToFormat(QImage::Format_ARGB32);

    QPainter painter(&target);
    painter.setCompositionMode(QPainter::CompositionMode_Screen);
    painter.fillRect(image.rect(), color);
    painter.end();

    if (image.hasAlphaChannel()) {
        target.setAlphaChannel(image.alphaChannel());
    }

    return target;
}
