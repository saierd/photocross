#pragma once

#include <QImage>

QImage computeDifferenceImage(QImage const& image1,
                              QImage const& image2,
                              int tolerance = 0,
                              bool showMinorDifferences = true);
