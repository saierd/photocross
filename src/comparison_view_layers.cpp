#include "comparison_view_layers.h"

#include "comparison_settings.h"
#include "image_difference.h"

#include <cmath>

namespace {

ComparisonViewLayers computeHighlightDifferencesLayers(Session::Images const& images,
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

    int tolerance = 0;
    if (settings.highlightDifferences.applyTolerance) {
        tolerance = settings.highlightDifferences.tolerance;
    }

    differenceImage =
        computeDifferenceImage(imagesToCompare, tolerance, settings.highlightDifferences.showMinorDifferences);

    return {
        {images[0]->toGrayscalePixmap()},
        {QPixmap::fromImage(differenceImage), settings.highlightDifferences.differencesTransparency},
    };
}

ComparisonViewLayers blendLayers(QPixmap const& image1, QPixmap const& image2, double blendPosition)
{
    return {
        {image1, 1 - blendPosition},
        {image2, blendPosition},
    };
}

ComparisonViewLayers computeBlendLayers(Session::Images const& images, ComparisonSettings const& settings)
{
    if (images.size() < 2) {
        return {};
    }

    auto const& leftImage = *images[0];
    auto const& rightImage = *images[1];

    double const blendPosition = settings.blendPosition;

    QColor leftImageColor;
    QColor rightImageColor;
    if (settings.mode == ComparisonMode::BlendImagesFalseColors) {
        leftImageColor = settings.blendFalseColors.leftColor;
        rightImageColor = settings.blendFalseColors.rightColor;
    }

    return blendLayers(leftImage.toColorizedPixmap(leftImageColor),
                       rightImage.toColorizedPixmap(rightImageColor),
                       blendPosition);
}

ComparisonViewLayers computeAnimatedBlendLayers(Session::Images const& images,
                                                ComparisonSettings const& settings,
                                                double animationStep)
{
    if (images.size() < 2) {
        return {};
    }

    int const baseImageIndex = static_cast<int>(std::floor(animationStep));
    size_t const firstImageIndex = baseImageIndex % images.size();
    size_t const secondImageIndex = (firstImageIndex + 1) % images.size();

    if (!settings.animatedBlending.continuous) {
        return {
            {images[firstImageIndex]->toPixmap()},
        };
    }

    double const blendPosition = animationStep - baseImageIndex;
    return blendLayers(images[firstImageIndex]->toPixmap(), images[secondImageIndex]->toPixmap(), blendPosition);
}

}  // namespace

ComparisonViewLayers computeComparisonViewLayers(Session::Images const& images,
                                                 ComparisonSettings const& settings,
                                                 double animationStep)
{
    switch (settings.mode) {
        case ComparisonMode::HighlightDifferences:
            return computeHighlightDifferencesLayers(images, settings);
        case ComparisonMode::BlendImagesFalseColors:
        case ComparisonMode::BlendImagesTrueColors:
            return computeBlendLayers(images, settings);
        case ComparisonMode::BlendImagesAnimated:
            return computeAnimatedBlendLayers(images, settings, animationStep);
    }

    assert(false);
    return {};
}
