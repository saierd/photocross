#pragma once

#include <QImage>

QImage computeDifferenceImage(std::vector<QImage const*> images, int tolerance = 0, bool showMinorDifferences = true);
