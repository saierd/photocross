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

    void fitSceneInView();

    void overrideCursor(QCursor newCursor);
    void resetCursor();

signals:
    void zoomChangedExplicitly();
    void zoomOutLimitReached();
    void viewChanged();

    void mouseLeft();

protected:
    void wheelEvent(QWheelEvent* event) override;
    void enterEvent(QEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    void applyOverriddenCursor();

private:
    bool cursorOverridden = false;
    QCursor cursor;
    QCursor previousCursor;
};
