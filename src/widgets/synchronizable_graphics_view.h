#pragma once

#include "interactive_graphics_view.h"

// QGraphicsView, whose view can be synchronized with other instances of this widget.
class SynchronizableGraphicsView : public InteractiveGraphicsView {
    Q_OBJECT

    // Describes the current view, so that it can be transferred to another instance.
    struct View {
        // View transform of the graphics view. This is mainly used for scaling, the translation comes from the
        // additional information below.
        QTransform transform;

        // The scene coordinate that is currently in the center of the widget. Note that we need this in addition to the
        // transform above, because the different instances of the widget might have different sizes. In that case we
        // want to preserve what the view is currently centered on, which is not done by the transform alone.
        QPointF centeredScenePoint;
    };

public:
    explicit SynchronizableGraphicsView(QWidget* parent = nullptr);

    void synchronizeViews(SynchronizableGraphicsView const& other) const;
    void forceViewPropagation() const;

signals:
    void synchronizedViewChanged(View const& view) const;

private slots:
    void emitSynchronizedViewChanged() const;
    void applyView(View const& view);
};
