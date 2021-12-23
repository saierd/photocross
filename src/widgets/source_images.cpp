#include "source_images.h"

#include "session.h"
#include "single_direction_scroll_area.h"
#include "utility/drag_drop.h"

#include <QApplication>
#include <QDragEnterEvent>
#include <QDragMoveEvent>

#include <cmath>

SourceImages::SourceImages(QWidget* parent)
  : QWidget(parent)
  , layout(QBoxLayout::LeftToRight)
{
    auto* rootLayout = new QBoxLayout(QBoxLayout::LeftToRight, this);
    rootLayout->setMargin(0);
    setLayout(rootLayout);

    auto* scrollArea = new SingleDirectionScrollArea(this);
    scrollArea->setOrientation(Qt::Horizontal);

    // TODO: Frame of scroll bar is incomplete
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setFrameShadow(QFrame::Plain);

    QWidget* dummyWidget = new QWidget(this);
    dummyWidget->setLayout(&layout);
    layout.setMargin(0);

    rootLayout->addWidget(scrollArea);
    scrollArea->setWidget(dummyWidget);

    layout.addWidget(&emptyImage);
    updateEmptyImage();

    connect(&emptyImage, &EmptyImage::imagesDropped, this, [this](QStringList const& files) {
        session->loadImages(files);
    });

    // Drag and drop event are handled by this widget. Child widgets must not handle them, otherwise we do not get
    // notified about events here and cannot adjust our layout.
    setAcceptDrops(true);
    emptyImage.installEventFilter(this);
}

void SourceImages::setSession(Session* _session)
{
    session = _session;
}

void SourceImages::setDirection(QBoxLayout::Direction direction)
{
    layout.setDirection(direction);
}

int SourceImages::numImageWidgets() const
{
    return layout.count() - 1;  // -1 for empty image widget.
}

void SourceImages::addImageWidget(QWidget* widget)
{
    widget->installEventFilter(this);

    layout.insertWidget(numImageWidgets(), widget);
    updateEmptyImage();
}

void SourceImages::popImageWidget()
{
    if (numImageWidgets() == 0) {
        return;
    }

    auto* item = layout.takeAt(numImageWidgets() - 1);
    item->widget()->deleteLater();
    delete item;

    updateEmptyImage();
}

namespace {

bool isDragEvent(QEvent* event)
{
    return event->type() == QEvent::DragEnter || event->type() == QEvent::DragMove ||
           event->type() == QEvent::DragLeave;
}

}  // namespace

bool SourceImages::eventFilter(QObject* /* watched */, QEvent* event)
{
    // Steal drag and drop events from children. Drag and drop events are handled in this widget and only
    // selectively forwarded to the hovered child widgets. This way we can adjust our layout to the events even
    // though the actual drop target is one of our child widgets.
    return isDragEvent(event);
}

void SourceImages::dragEnterEvent(QDragEnterEvent* event)
{
    if (extractImageFiles(event).empty()) {
        return;
    }

    dropping = true;
    updateEmptyImage();

    event->acceptProposedAction();
}

void SourceImages::dragLeaveEvent(QDragLeaveEvent* /* event */)
{
    leaveLastDropWidget();
    finishDropEvent();
}

void SourceImages::dragMoveEvent(QDragMoveEvent* event)
{
    auto* hoveredChild = hoveredWidget(event);
    if (hoveredChild != nullptr) {
        event->acceptProposedAction();
        forwardDragMoveEvent(hoveredChild, event);
    } else {
        leaveLastDropWidget();
    }
}

void SourceImages::dropEvent(QDropEvent* event)
{
    if (lastDropWidget != nullptr) {
        lastDropWidget->dropEvent(event);
    }

    finishDropEvent();
}

void SourceImages::updateEmptyImage()
{
    bool tooFewImages = numImageWidgets() < 2;

    emptyImage.setVisible(dropping || tooFewImages);

    auto policy = tooFewImages ? QSizePolicy::Expanding : QSizePolicy::Preferred;
    emptyImage.setSizePolicy(policy, QSizePolicy::Preferred);
}

ImageDropWidget* SourceImages::hoveredWidget(QDragMoveEvent* event)
{
    for (int i = 0; i < layout.count(); i++) {
        QWidget* child = layout.itemAt(i)->widget();
        if (child != nullptr && child->isVisible() && child->geometry().contains(event->pos())) {
            return dynamic_cast<ImageDropWidget*>(child);
        }
    }

    return nullptr;
}

void SourceImages::forwardDragMoveEvent(ImageDropWidget* hoveredChild, QDragMoveEvent* event)
{
    if (hoveredChild != lastDropWidget) {
        leaveLastDropWidget();

        QDragEnterEvent enterEvent(event->pos(),
                                   event->possibleActions(),
                                   event->mimeData(),
                                   event->mouseButtons(),
                                   event->keyboardModifiers());
        hoveredChild->dragEnterEvent(&enterEvent);
        lastDropWidget = hoveredChild;
    }

    hoveredChild->dragMoveEvent(event);
}

void SourceImages::leaveLastDropWidget()
{
    if (lastDropWidget == nullptr) {
        return;
    }

    QDragLeaveEvent event;
    lastDropWidget->dragLeaveEvent(&event);

    lastDropWidget = nullptr;
}

void SourceImages::finishDropEvent()
{
    dropping = false;
    updateEmptyImage();
}
