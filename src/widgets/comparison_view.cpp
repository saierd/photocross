#include "comparison_view.h"

#include "busy_dialog.h"
#include "comparison_view_layers.h"
#include "session.h"
#include "time_helpers.h"

auto constexpr continuousBlendTimerInterval = 50ms;

ComparisonView::ComparisonView(QWidget* parent)
  : ImageView(parent)
{
    setCaption("Comparison View");

    connect(&animationUpdateTimer, &QTimer::timeout, this, &ComparisonView::animationUpdate);
}

void ComparisonView::update(Session const& session, ComparisonSettings const& _settings)
{
    images = session.getImages();
    settings = _settings;
    update();

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
        }
    }
}

void ComparisonView::update()
{
    auto timeSinceAnimationStart = std::chrono::steady_clock::now() - animationStartTime;
    double animationStep = durationRatio(timeSinceAnimationStart, settings.animatedBlending.timeBetweenImages);

    ComparisonViewLayers layers;
    runWithBusyDialog(
        "Updating Comparison View...",
        [&]() {
            layers = computeComparisonViewLayers(images, settings, animationStep);
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

void ComparisonView::animationUpdate()
{
    update();
}
