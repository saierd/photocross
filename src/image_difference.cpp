#include "image_difference.h"

bool pixelsEqual(QRgb pixel1, QRgb pixel2, int tolerance)
{
    auto colorsEqual = [&](int color1, int color2) -> bool {
        return std::abs(color1 - color2) <= tolerance;
    };

    return colorsEqual(qRed(pixel1), qRed(pixel2)) && colorsEqual(qGreen(pixel1), qGreen(pixel2)) &&
           colorsEqual(qBlue(pixel1), qBlue(pixel2)) && colorsEqual(qAlpha(pixel1), qAlpha(pixel2));
}

QImage computeDifferenceImage(QImage const& image1, QImage const& image2, int tolerance, bool showMinorDifferences)
{
    QImage diff(image1.size(), QImage::Format_ARGB32);
    diff.fill(qRgba(255, 255, 255, 0));

    for (int y = 0; y < image1.height(); y++) {
        for (int x = 0; x < image1.width(); x++) {
            QRgb pixel1 = image1.pixel(x, y);
            QRgb pixel2 = image2.pixel(x, y);

            if (!pixelsEqual(pixel1, pixel2, tolerance)) {
                diff.setPixel(x, y, qRgb(255, 0, 0));
            } else if (showMinorDifferences && pixel1 != pixel2) {
                diff.setPixel(x, y, qRgb(0, 0, 255));
            }
        }
    }

    return diff;
}
