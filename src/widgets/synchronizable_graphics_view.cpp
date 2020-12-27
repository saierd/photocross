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

void SynchronizableGraphicsView::emitSynchronizedViewChanged()
{
    auto centerPoint = mapToScene(viewport()->rect().center());
    emit synchronizedViewChanged({transform(), centerPoint});
}

void SynchronizableGraphicsView::applyView(View const& view)
{
    // Block signals to avoid recursion.
    QSignalBlocker blockSignals(this);

    setTransform(view.transform);
    centerOn(view.centeredScenePoint);
}
