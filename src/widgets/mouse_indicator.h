#pragma once

#include <QGraphicsItem>

class MouseIndicatorGraphicsItem : public QGraphicsItem {
public:
    using QGraphicsItem::QGraphicsItem;

    QRectF boundingRect() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
};
