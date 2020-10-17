#include "image_diff.h"

QImage imageDiff(QImage const& image1, QImage const& image2)
{
    QImage diff(image1.size(), QImage::Format_ARGB32);
    diff.fill(qRgba(255, 255, 255, 0));

    for (int y = 0; y < image1.height(); y++) {
        for (int x = 0; x < image1.width(); x++) {
            if (image1.pixel(x, y) != image2.pixel(x, y)) {
                diff.setPixel(x, y, qRgb(255, 0, 0));
            }
        }
    }

    return diff;
}
