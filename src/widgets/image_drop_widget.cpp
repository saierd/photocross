#include "image_drop_widget.h"

#include "utility/drag_drop.h"

#include <QDropEvent>

ImageDropWidget::ImageDropWidget(QWidget* parent)
  : QWidget(parent)
{
    setAcceptDrops(true);
}

void ImageDropWidget::dragEnterEvent(QDragEnterEvent* event)
{
    if (!extractImageFiles(event).empty()) {
        event->acceptProposedAction();
        emit dropping(true);
    }
}

void ImageDropWidget::dragLeaveEvent(QDragLeaveEvent* event)
{
    QWidget::dragLeaveEvent(event);
    emit dropping(false);
}

void ImageDropWidget::dropEvent(QDropEvent* event)
{
    emit imagesDropped(extractImageFiles(event));
    emit dropping(false);
}
