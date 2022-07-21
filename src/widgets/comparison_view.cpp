#include "comparison_view.h"

#include "busy_dialog.h"
#include "comparison_view_layers.h"
#include "session.h"
#include "utility/time.h"

namespace {

auto constexpr continuousBlendTimerInterval = 50ms;

QRect computeBoundingRect(Session::Images const& images)
{
    QRect result;
    for (auto const& image : images) {
        result |= image->toPixmap().rect();
    }

    return result;
}

}  // namespace

ComparisonView::ComparisonView(QWidget* parent)
  : ImageView(parent)
{
    setCaption("Comparison View");

    connect(&animationUpdateTimer, &QTimer::timeout, this, &ComparisonView::requestAnimationUpdate);
}

void ComparisonView::update(Session::Images const& images, ComparisonSettings const& settings)
{
    updateAnimationTimerSettings(settings);
    double animationStep = currentAnimationStep(settings);

    ComparisonViewLayers layers;
    QRect boundingRect;
    runWithBusyDialog(
        "Updating Comparison View...",
        [&]() {
            layers = computeComparisonViewLayers(images, settings, animationStep);
            boundingRect = computeBoundingRect(images);
        },
        this);

    // Clearing the scene will reset the view position. Store the current view settings and restore it after the update.
    rememberView();

    clear();
    for (auto const& layer : layers) {
        addPixmap(layer.pixmap, layer.opacity);
    }

    // Update the scene rect to the bounding rect of all images. In animated blending mode the images might have
    // different sizes and we might not show the largest image at the moment. In that case the scene rect should still
    // contain all possible images to avoid moving the scene when the displayed images change.
    updateSceneRect(boundingRect);

    restoreView();
    forceViewPropagation();
}

void ComparisonView::updateAnimationTimerSettings(ComparisonSettings const& settings)
{
    if (settings.mode != ComparisonMode::BlendImagesAnimated) {
        if (animationUpdateTimer.isActive()) {
            animationUpdateTimer.stop();
        }
    } else {
        if (settings.animatedBlending.continuous) {
            animationUpdateTimer.setInterval(continuousBlendTimerInterval);
        } else {
            animationUpdateTimer.setInterval(settings.animatedBlending.timeBetweenImages);
        }

        if (!animationUpdateTimer.isActive()) {
            animationStartTime = std::chrono::steady_clock::now();
            animationUpdateTimer.start();
        } else if (previousAnimationTimeBetweenImages != settings.animatedBlending.timeBetweenImages) {
            // Patch animation start time to keep the same animation step.

            ComparisonSettings oldSettings = settings;
            oldSettings.animatedBlending.timeBetweenImages = previousAnimationTimeBetweenImages;
            double animationStepWithOldSettings = currentAnimationStep(oldSettings);

            auto patchedTimeSinceAnimationStart = std::chrono::duration_cast<std::chrono::nanoseconds>(
                animationStepWithOldSettings * settings.animatedBlending.timeBetweenImages);
            animationStartTime = std::chrono::steady_clock::now() - patchedTimeSinceAnimationStart;
        }

        previousAnimationTimeBetweenImages = settings.animatedBlending.timeBetweenImages;
    }
}

double ComparisonView::currentAnimationStep(ComparisonSettings const& settings)
{
    auto timeSinceAnimationStart = std::chrono::steady_clock::now() - animationStartTime;
    return durationRatio(timeSinceAnimationStart, settings.animatedBlending.timeBetweenImages);
}