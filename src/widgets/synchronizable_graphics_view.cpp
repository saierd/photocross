#include "synchronizable_graphics_view.h"

#include <QSignalBlocker>

SynchronizableGraphicsView::SynchronizableGraphicsView(QWidget* parent)
  : InteractiveGraphicsView(parent)
{
    connect(this,
            &InteractiveGraphicsView::viewChanged,
            this,
            &SynchronizableGraphicsView::emitSynchronizedViewChanged);
}

void SynchronizableGraphicsView::synchronizeViews(SynchronizableGraphicsView const& other) const
{
    connect(this, &SynchronizableGraphicsView::synchronizedViewChanged, &other, &SynchronizableGraphicsView::applyView);
    connect(&other, &SynchronizableGraphicsView::synchronizedViewChanged, this, &SynchronizableGraphicsView::applyView);
}

void SynchronizableGraphicsView::forceViewPropagation()
{
    emitSynchronizedViewChanged();
}

void SynchronizableGraphicsView::rememberView()
{
    rememberedView = currentView();
}

void SynchronizableGraphicsView::restoreView()
{
    applyView(rememberedView);
    forceViewPropagation();
}

void SynchronizableGraphicsView::emitSynchronizedViewChanged()
{
    emit synchronizedViewChanged(currentView());
}

void SynchronizableGraphicsView::applyView(View const& view)
{
    // Block signals to avoid recursion.
    QSignalBlocker blockSignals(this);

    setTransform(view.transform);
    centerOn(view.centeredScenePoint);
}

SynchronizableGraphicsView::View SynchronizableGraphicsView::currentView() const
{
    // Compute the center of the viewport rect. We must not round this point to integer coordinates to avoid rounding
    // when the viewport has an odd size. This could otherwise lead to systematic shifting of the view when propagating
    // views repeatedly.
    QRect viewportRect = viewport()->rect();
    QSizeF viewportSize(viewportRect.size());
    QPointF viewportCenter =
        QPointF(viewportRect.topLeft()) + QPointF(viewportSize.width(), viewportSize.height()) / 2.;

    // Transform the viewport center to scene coordinates. Note that we use the QPainterPath overload of mapToScene,
    // because it is the only one that does not use rounded integer coordinates.
    QPainterPath path;
    path.moveTo(viewportCenter);
    QPointF centerPoint = mapToScene(path).currentPosition();

    return {transform(), centerPoint};
}
