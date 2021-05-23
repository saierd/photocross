#include "mouse_indicator.h"

#include <QPainter>

#include <array>

double const baseSize = 1.2;
double const extensionSize = 1;
double const lineWidth = 1;

// Add a margin to the bounding box to make sure all the rounded edges of the lines are inside of the bounding box.
double const boundingBoxTolerance = 0.5;

// Color for the mouse indicator. Note that the color specified here is the inverted color from what will be displayed
// on a white background, since we draw the mouse indicator in difference mode.
// The color is chosen as a kind of magenta on a white background, but with some green added. This is to make the mouse
// indicator still visible on a pure green background.
int const colorDistanceFromMagenta = 70;
QColor const color(colorDistanceFromMagenta, 255 - colorDistanceFromMagenta, colorDistanceFromMagenta);

QRectF MouseIndicatorGraphicsItem::boundingRect() const
{
    double const size = baseSize + extensionSize + boundingBoxTolerance;
    return QRectF(-size, -size, 2 * size, 2 * size);
}

void MouseIndicatorGraphicsItem::paint(QPainter* painter,
                                       const QStyleOptionGraphicsItem* /*option*/,
                                       QWidget* /*widget*/)
{
    // Draw with composition mode difference to make the indicator visible regardless of the color of the background
    // image.
    painter->setCompositionMode(QPainter::CompositionMode_Difference);

    QPen pen(color);
    pen.setWidthF(lineWidth);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::MiterJoin);
    painter->setPen(pen);

    std::array<QPointF, 3> pointsTopLeft = {{
        {-baseSize, -baseSize - extensionSize},
        {-baseSize, -baseSize},
        {-baseSize - extensionSize, -baseSize},
    }};
    painter->drawPolyline(pointsTopLeft.data(), pointsTopLeft.size());

    std::array<QPointF, 3> pointsTopRight = {{
        {baseSize, -baseSize - extensionSize},
        {baseSize, -baseSize},
        {baseSize + extensionSize, -baseSize},
    }};
    painter->drawPolyline(pointsTopRight.data(), pointsTopRight.size());

    std::array<QPointF, 3> pointsBottomLeft = {{
        {-baseSize, baseSize + extensionSize},
        {-baseSize, baseSize},
        {-baseSize - extensionSize, baseSize},
    }};
    painter->drawPolyline(pointsBottomLeft.data(), pointsBottomLeft.size());

    std::array<QPointF, 3> pointsBottomRight = {{
        {baseSize, baseSize + extensionSize},
        {baseSize, baseSize},
        {baseSize + extensionSize, baseSize},
    }};
    painter->drawPolyline(pointsBottomRight.data(), pointsBottomRight.size());
}
