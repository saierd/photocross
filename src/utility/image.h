#pragma once

#include <QImage>

QImage addTransparentOffset(QImage const& image, QPoint const& offset);

QImage colorizeGrayscaleImage(QImage const& image, QColor const& color);
