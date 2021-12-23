#include "single_direction_scroll_area.h"

#include <QEvent>
#include <QScrollBar>

SingleDirectionScrollArea::SingleDirectionScrollArea(QWidget* parent)
  : QScrollArea(parent)
{
    setWidgetResizable(true);

    setOrientation(orientation);
}

void SingleDirectionScrollArea::setOrientation(Qt::Orientation _orientation)
{
    orientation = _orientation;

    if (orientation == Qt::Horizontal) {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    } else {
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    }
    updateMinimumSize();
}

bool SingleDirectionScrollArea::eventFilter(QObject* object, QEvent* event)
{
    // QScrollArea::setWidget installs an event filter on the widget, so we get its resize event here.
    if (object == widget() && event->type() == QEvent::Resize) {
        updateMinimumSize();
    }

    return QScrollArea::eventFilter(object, event);
}

void SingleDirectionScrollArea::updateMinimumSize()
{
    if (widget() == nullptr) {
        return;
    }

    if (orientation == Qt::Horizontal) {
        setMinimumWidth(0);
        setMinimumHeight(widget()->minimumSizeHint().height() + horizontalScrollBar()->height());
    } else {
        setMinimumWidth(widget()->minimumSizeHint().width() + verticalScrollBar()->width());
        setMinimumHeight(0);
    }
}
