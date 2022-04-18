#include "drag_drop_helpers.h"

#include "file_helpers.h"

#include <QDropEvent>
#include <QMimeData>

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
