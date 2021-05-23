#pragma once

#include <QGraphicsScene>

class ImageViewScene : public QGraphicsScene {
    Q_OBJECT

public:
    struct MouseMovedInfo {
        ImageViewScene const* hoveredScene;
        QPointF sceneCoordinates;
    };

signals:
    void mouseMoved(ImageViewScene::MouseMovedInfo const& info);
    void mouseLeft();

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
};
