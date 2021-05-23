#include "image_view_scene.h"

#include <QGraphicsSceneMouseEvent>

void ImageViewScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    emit mouseMoved({this, event->scenePos()});
    QGraphicsScene::mouseMoveEvent(event);
}
