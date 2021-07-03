#include "comparison_view.h"

#include "comparison_settings.h"
#include "image_difference.h"
#include "session.h"

#include <QGraphicsColorizeEffect>
#include <QGraphicsPixmapItem>

#include <cmath>

ComparisonView::ComparisonView(QWidget* parent)
  : ImageView(parent)
{
    setCaption("Comparison View");
}

void addColorFilter(QGraphicsPixmapItem* item, QColor const& color)
{
    if (!color.isValid()) {
        return;
    }

    auto effect = std::make_unique<QGraphicsColorizeEffect>();
    effect->setColor(color);
    item->setGraphicsEffect(effect.release());
}

void ComparisonView::update(Session const& session, ComparisonSettings const& settings)
{
    // Clearing the scene will reset the view position. Store the current view settings and restore it after the update.
    rememberView();

    clear();

    auto const& images = session.getImages();

    ComparisonMode comparisonMode = settings.getComparisonMode();
    if (comparisonMode == ComparisonMode::HighlightDifferences) {
        QImage differenceImage;
        if (images.size() >= 2) {
            std::vector<QImage> imagesToCompare;
            imagesToCompare.reserve(images.size());
            for (auto const& image : images) {
                imagesToCompare.push_back(image->image());
            }

            differenceImage = computeDifferenceImage(imagesToCompare,
                                                     settings.getDifferenceTolerance(),
                                                     settings.showMinorDifferences());
        }

        addPixmap(images[0]->toGrayscalePixmap());
        addPixmap(QPixmap::fromImage(differenceImage), 0.7);
    } else if (comparisonMode == ComparisonMode::BlendImages && images.size() >= 2) {
        double const sequenceBlendPosition = settings.getBlendPosition() * static_cast<double>(images.size() - 1);

        auto firstImageIndex = static_cast<size_t>(std::floor(sequenceBlendPosition));
        firstImageIndex = std::min(firstImageIndex, images.size() - 2);

        double const blendPosition = sequenceBlendPosition - firstImageIndex;

        auto* firstImage = getScene().addPixmap(images[firstImageIndex]->toPixmap());
        firstImage->setOpacity(1 - blendPosition);
        auto* secondImage = getScene().addPixmap(images[firstImageIndex + 1]->toPixmap());
        secondImage->setOpacity(blendPosition);

        QColor firstImageColor = settings.blendImage1Color();
        QColor secondImageColor = settings.blendImage2Color();
        if (firstImageIndex % 2 != 0) {
            std::swap(firstImageColor, secondImageColor);
        }

        addColorFilter(firstImage, firstImageColor);
        addColorFilter(secondImage, secondImageColor);
    }

    updateSceneRect();

    restoreView();
    forceViewPropagation();
}
