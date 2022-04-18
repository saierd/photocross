#include "source_images.h"

#include "session.h"

SourceImages::SourceImages(QWidget* parent)
  : QWidget(parent)
  , layout(QBoxLayout::LeftToRight)
{
    setLayout(&layout);

    emptyImage.setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    emptyImage.hide();
    layout.addWidget(&emptyImage);

    connect(&emptyImage, &EmptyImage::imagesDropped, this, [this](QStringList const& files) {
        session->loadImages(files);
    });
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
    layout.insertWidget(numImageWidgets(), widget);
    numWidgetsChanged();
}

void SourceImages::popImageWidget()
{
    if (numImageWidgets() == 0) {
        return;
    }

    auto* item = layout.takeAt(numImageWidgets() - 1);
    item->widget()->deleteLater();
    delete item;

    numWidgetsChanged();
}

void SourceImages::numWidgetsChanged()
{
    emptyImage.setVisible(numImageWidgets() < 2);
}
