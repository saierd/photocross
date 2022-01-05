#include "comparison_view.h"

#include "busy_dialog.h"
#include "comparison_view_layers.h"
#include "session.h"

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
            layers = computeComparisonViewLayers(session.getImages(), settings);
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
