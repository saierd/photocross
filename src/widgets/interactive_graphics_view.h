#pragma once

#include <QGraphicsScene>
#include <QGraphicsView>

// QGraphicsView, whose view can be changed interactively (zooming with the mouse wheel, panning by dragging) and whose
// view can be synchronized with other instances of this widget.
class InteractiveGraphicsView : public QGraphicsView {
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
    explicit InteractiveGraphicsView(QWidget* parent = nullptr);

    void synchronizeViews(InteractiveGraphicsView const& other);

    void clear();
    void addPixmap(QPixmap const& image, double opacity = 1.);

public slots:
    void fitViewToScene();

    void zoom(int steps);
    void zoomIn();
    void zoomOut();

signals:
    void viewChanged(View const& view) const;

protected:
    void wheelEvent(QWheelEvent* event) override;

private slots:
    void emitViewChanged() const;
    void applyView(View const& view);

private:
    QGraphicsScene scene;
};
