#include "comparison_view_layers.h"

#include "image_difference.h"
#include "widgets/comparison_settings.h"

#include <cmath>

namespace {

ComparisonViewLayers computeHighlightDifferencesLayers(std::vector<Session::ImageHandle> const& images,
                                                       ComparisonSettings const& settings)
{
    if (images.empty()) {
        return {};
    }
    if (images.size() < 2) {
        return {{images[0]->toGrayscalePixmap()}};
    }

    QImage differenceImage;
    std::vector<QImage> imagesToCompare;
    std::transform(images.begin(), images.end(), std::back_inserter(imagesToCompare), [](auto const& image) {
        return image->image();
    });

    differenceImage =
        computeDifferenceImage(imagesToCompare, settings.getDifferenceTolerance(), settings.showMinorDifferences());

    return {
        {images[0]->toGrayscalePixmap()},
        {QPixmap::fromImage(differenceImage), 0.7},
    };
}

ComparisonViewLayers computeBlendLayers(std::vector<Session::ImageHandle> const& images,
                                        ComparisonSettings const& settings)
{
    if (images.size() < 2) {
        return {};
    }

    double const sequenceBlendPosition = settings.getBlendPosition() * static_cast<double>(images.size() - 1);

    auto firstImageIndex = static_cast<size_t>(std::floor(sequenceBlendPosition));
    firstImageIndex = std::min(firstImageIndex, images.size() - 2);

    double const blendPosition = sequenceBlendPosition - firstImageIndex;

    QColor firstImageColor = settings.blendImage1Color();
    QColor secondImageColor = settings.blendImage2Color();
    if (firstImageIndex % 2 != 0) {
        std::swap(firstImageColor, secondImageColor);
    }

    return {
        {images[firstImageIndex]->toColorizedPixmap(firstImageColor), 1 - blendPosition},
        {images[firstImageIndex + 1]->toColorizedPixmap(secondImageColor), blendPosition},
    };
}

}  // namespace

ComparisonViewLayers computeComparisonViewLayers(std::vector<Session::ImageHandle> const& images,
                                                 ComparisonSettings const& settings)
{
    switch (settings.getComparisonMode()) {
        case ComparisonMode::HighlightDifferences:
            return computeHighlightDifferencesLayers(images, settings);
        case ComparisonMode::BlendImages:
            return computeBlendLayers(images, settings);
    }

    return {};
}
