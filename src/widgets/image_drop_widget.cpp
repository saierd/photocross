#include "image_drop_widget.h"

#include "file_helpers.h"

#include <QDropEvent>
#include <QMimeData>

ImageDropWidget::ImageDropWidget(QWidget* parent)
  : QWidget(parent)
{
    setAcceptDrops(true);
}

QStringList extractImageFiles(QDropEvent* event)
{
    if (!event->mimeData()->hasUrls()) {
        return {};
    }

    QStringList files;
    auto const urls = event->mimeData()->urls();
    for (auto const& url : urls) {
        QString file = url.toLocalFile();
        if (!file.isEmpty() && isImageFile(file)) {
            files.push_back(file);
        }
    }

    return files;
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
