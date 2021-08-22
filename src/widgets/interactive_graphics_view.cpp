#include "interactive_graphics_view.h"

#include <QScopeGuard>
#include <QScrollBar>
#include <QWheelEvent>

#include <cmath>

double constexpr zoomFactor = 1.1;
double constexpr minimumVisiblePixels = 20;

InteractiveGraphicsView::InteractiveGraphicsView(QWidget* parent)
  : QGraphicsView(parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);

    connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, [this]() {
        didDragSinceLastMousePress = true;
        emit viewChanged();
    });
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, [this]() {
        didDragSinceLastMousePress = true;
        emit viewChanged();
    });
}

void InteractiveGraphicsView::zoom(int steps)
{
    if (steps == 0) {
        return;
    }

    auto const previousAnchor = transformationAnchor();
    auto restoreAnchor = qScopeGuard([this, previousAnchor]() {
        setTransformationAnchor(previousAnchor);
    });

    // Zoom towards mouse position.
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    double currentScaling = std::sqrt(transform().determinant());

    double const minimumScaling =
        std::min(viewport()->width() / sceneRect().width(), viewport()->height() / sceneRect().height());
    double const minimumScalingFactor = minimumScaling / currentScaling;

    double const maximumScaling = std::max(viewport()->width(), viewport()->height()) / minimumVisiblePixels;
    double const maximumScalingFactor = maximumScaling / currentScaling;

    double scalingFactor = std::pow(zoomFactor, steps);
    scalingFactor = std::max(scalingFactor, minimumScalingFactor);
    scalingFactor = std::min(scalingFactor, maximumScalingFactor);

    scale(scalingFactor, scalingFactor);

    emit zoomChangedExplicitly();
    emit viewChanged();

    if (scalingFactor == minimumScalingFactor) {
        emit zoomOutLimitReached();
    }
}

void InteractiveGraphicsView::zoomIn()
{
    zoom(1);
}

void InteractiveGraphicsView::zoomOut()
{
    zoom(-1);
}

void InteractiveGraphicsView::fitSceneInView()
{
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);
    emit viewChanged();
}

void InteractiveGraphicsView::overrideCursor(QCursor newCursor)
{
    if (!cursorOverridden) {
        previousCursor = viewport()->cursor();
    }

    cursorOverridden = true;
    cursor = std::move(newCursor);

    applyOverriddenCursor();
}

void InteractiveGraphicsView::resetCursor()
{
    if (!cursorOverridden) {
        return;
    }

    cursorOverridden = false;
    viewport()->setCursor(previousCursor);
}

void InteractiveGraphicsView::wheelEvent(QWheelEvent* event)
{
    if (event->angleDelta().y() > 0) {
        zoomIn();
    } else {
        zoomOut();
    }
}

void InteractiveGraphicsView::enterEvent(QEvent* event)
{
    QGraphicsView::enterEvent(event);
    applyOverriddenCursor();
}

void InteractiveGraphicsView::leaveEvent(QEvent* event)
{
    QGraphicsView::leaveEvent(event);
    emit mouseLeft();
}

void InteractiveGraphicsView::mousePressEvent(QMouseEvent* event)
{
    QGraphicsView::mousePressEvent(event);
    applyOverriddenCursor();

    didDragSinceLastMousePress = false;
}

void InteractiveGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);
    applyOverriddenCursor();

    if (!didDragSinceLastMousePress) {
        emit mouseClicked(mapToScene(event->pos()).toPoint());
    }
}

void InteractiveGraphicsView::applyOverriddenCursor()
{
    if (!cursorOverridden) {
        return;
    }

    viewport()->setCursor(cursor);
}
