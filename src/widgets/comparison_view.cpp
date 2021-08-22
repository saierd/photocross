#include "comparison_view.h"

#include "busy_dialog.h"
#include "comparison_settings.h"
#include "image_difference.h"
#include "session.h"

#include <QGraphicsColorizeEffect>
#include <QGraphicsPixmapItem>

#include <cmath>

namespace {

struct ComparisonViewLayer {
    QPixmap pixmap;
    double opacity = 1.;
};

using ComparisonViewLayers = std::vector<ComparisonViewLayer>;

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

ComparisonViewLayers computeComparisonLayers(std::vector<Session::ImageHandle> const& images,
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

}  // namespace

ComparisonView::ComparisonView(QWidget* parent)
  : ImageView(parent)
{
    setCaption("Comparison View");
}

void ComparisonView::update(Session const& session, ComparisonSettings const& settings)
{
    ComparisonViewLayers layers;
    runWithBusyDialog(
        "Updating Comparison View...",
        [&]() {
            layers = computeComparisonLayers(session.getImages(), settings);
        },
        this);

    // Clearing the scene will reset the view position. Store the current view settings and restore it after the update.
    rememberView();

    clear();
    for (auto const& layer : layers) {
        addPixmap(layer.pixmap, layer.opacity);
    }

    updateSceneRect();

    restoreView();
    forceViewPropagation();
}
