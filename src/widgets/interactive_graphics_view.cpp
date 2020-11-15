#include "interactive_graphics_view.h"

#include <QScopeGuard>
#include <QScrollBar>
#include <QWheelEvent>

#include <cmath>

double constexpr zoomFactor = 1.1;

InteractiveGraphicsView::InteractiveGraphicsView(QWidget* parent)
  : QGraphicsView(parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);

    connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &InteractiveGraphicsView::viewChanged);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &InteractiveGraphicsView::viewChanged);
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

    double scaleFactor = std::pow(zoomFactor, steps);
    scale(scaleFactor, scaleFactor);

    emit viewChanged();
}

void InteractiveGraphicsView::zoomIn()
{
    zoom(1);
}

void InteractiveGraphicsView::zoomOut()
{
    zoom(-1);
}

void InteractiveGraphicsView::wheelEvent(QWheelEvent* event)
{
    if (event->angleDelta().y() > 0) {
        zoomIn();
    } else {
        zoomOut();
    }
}
