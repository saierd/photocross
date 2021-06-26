#include "image_difference.h"

#include <cmath>
#include <limits>

int pixelDifference(int pixel1, int pixel2)
{
    return std::abs(pixel1 - pixel2);
}

int pixelDifference(QRgb pixel1, QRgb pixel2)
{
    int r = pixelDifference(qRed(pixel1), qRed(pixel2));
    int g = pixelDifference(qGreen(pixel1), qGreen(pixel2));
    int b = pixelDifference(qBlue(pixel1), qBlue(pixel2));
    int a = pixelDifference(qAlpha(pixel1), qAlpha(pixel2));

    return std::max(std::max(std::max(r, g), b), a);
}

QImage computeDifferenceImage(std::vector<QImage> const& images, int tolerance, bool showMinorDifferences)
{
    if (images.empty()) {
        return {};
    }

    QRgb const noDifferenceColor = qRgba(255, 255, 255, 0);
    QRgb const differenceColor = qRgb(255, 0, 0);
    QRgb const minorDifferenceColor = qRgb(0, 0, 255);

    QSize totalSize;
    QSize commonSize(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
    for (auto const& image : images) {
        totalSize = totalSize.expandedTo(image.size());
        commonSize = commonSize.boundedTo(image.size());
    }

    QImage diff(totalSize, QImage::Format_ARGB32);
    diff.fill(noDifferenceColor);

    for (int y = 0; y < commonSize.height(); y++) {
        for (int x = 0; x < commonSize.width(); x++) {
            int maxDifference = 0;
            for (size_t i = 1; i < images.size(); i++) {
                for (size_t j = 0; j < i; j++) {
                    QRgb pixel1 = images[i].pixel(x, y);
                    QRgb pixel2 = images[j].pixel(x, y);
                    maxDifference = std::max(maxDifference, pixelDifference(pixel1, pixel2));
                }
            }

            if (maxDifference > tolerance) {
                diff.setPixel(x, y, differenceColor);
            } else if (showMinorDifferences && maxDifference != 0) {
                diff.setPixel(x, y, minorDifferenceColor);
            }
        }

        // Fill right slice that is not contained in all images.
        for (int x = commonSize.width(); x < totalSize.width(); x++) {
            diff.setPixel(x, y, differenceColor);
        }
    }

    // Fill bottom slice that is not contained in all images.
    for (int y = commonSize.height(); y < totalSize.height(); y++) {
        for (int x = 0; x < totalSize.width(); x++) {
            diff.setPixel(x, y, differenceColor);
        }
    }

    return diff;
}
