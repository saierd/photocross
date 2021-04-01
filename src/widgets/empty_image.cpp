#include "empty_image.h"

#include "file_helpers.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QPainter>

#include "ui_empty_image.h"

int constexpr iconSize = 120;
QColor const iconColor = Qt::GlobalColor::gray;

void colorizePixmap(QPixmap& pixmap, QColor const& color)
{
    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), color);
    painter.end();
}

EmptyImage::EmptyImage(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::EmptyImage>();
    ui->setupUi(this);

    setAcceptDrops(true);

    QPixmap icon = QIcon(":/icons/icons/image.svg").pixmap(iconSize, iconSize);
    colorizePixmap(icon, iconColor);
    ui->icon->setPixmap(icon);
}

EmptyImage::~EmptyImage() = default;

QStringList extractImageFiles(QDropEvent* event)
{
    if (!event->mimeData()->hasUrls()) {
        return {};
    }

    QStringList files;
    for (auto const& url : event->mimeData()->urls()) {
        QString file = url.toLocalFile();
        if (!file.isEmpty() && isImageFile(file)) {
            files.push_back(file);
        }
    }

    return files;
}

void EmptyImage::dragEnterEvent(QDragEnterEvent* event)
{
    if (!extractImageFiles(event).empty()) {
        event->acceptProposedAction();
    }
}

void EmptyImage::dropEvent(QDropEvent* event)
{
    emit imagesDropped(extractImageFiles(event));
}
