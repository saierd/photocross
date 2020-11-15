#pragma once

#include <QGraphicsView>

// QGraphicsView, whose view can be changed interactively (zooming with the mouse wheel, panning by dragging).
class InteractiveGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    explicit InteractiveGraphicsView(QWidget* parent = nullptr);

public slots:
    void zoom(int steps);
    void zoomIn();
    void zoomOut();

signals:
    void viewChanged();

protected:
    void wheelEvent(QWheelEvent* event) override;
};
