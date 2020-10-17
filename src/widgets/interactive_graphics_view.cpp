#include "interactive_graphics_view.h"

#include "utility/finally.h"
#include "utility/signal_blocker.h"

#include <QGraphicsPixmapItem>
#include <QScrollBar>
#include <QWheelEvent>

#include <cmath>

double constexpr zoomFactor = 1.1;

InteractiveGraphicsView::InteractiveGraphicsView(QWidget* parent)
  : QGraphicsView(parent)
{
    setScene(&scene);
    setDragMode(QGraphicsView::ScrollHandDrag);

    connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &InteractiveGraphicsView::emitViewChanged);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &InteractiveGraphicsView::emitViewChanged);
}

void InteractiveGraphicsView::synchronizeViews(InteractiveGraphicsView const& other)
{
    connect(this, &InteractiveGraphicsView::viewChanged, &other, &InteractiveGraphicsView::applyView);
    connect(&other, &InteractiveGraphicsView::viewChanged, this, &InteractiveGraphicsView::applyView);
}

void InteractiveGraphicsView::clear()
{
    scene.clear();
}

void InteractiveGraphicsView::addPixmap(QPixmap const& image, double opacity)
{
    auto item = scene.addPixmap(image);
    item->setOpacity(opacity);
}

void InteractiveGraphicsView::fitViewToScene()
{
    fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
}

void InteractiveGraphicsView::zoom(int steps)
{
    if (steps == 0) {
        return;
    }

    auto const previousAnchor = transformationAnchor();
    Finally restoreAnchor([this, previousAnchor]() {
        setTransformationAnchor(previousAnchor);
    });

    // Zoom towards mouse position.
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    double scaleFactor = std::pow(zoomFactor, steps);
    scale(scaleFactor, scaleFactor);

    emitViewChanged();
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

void InteractiveGraphicsView::emitViewChanged() const
{
    auto centerPoint = mapToScene(viewport()->rect().center());
    emit(viewChanged({transform(), centerPoint}));
}

void InteractiveGraphicsView::applyView(View const& view)
{
    // Block signals to avoid recursion.
    SignalBlocker blockSignals(this);

    setTransform(view.transform);
    centerOn(view.centeredScenePoint);
}
