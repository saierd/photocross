#include "source_image_view.h"

void SourceImageView::setImage(std::shared_ptr<Image> newImage)
{
    image = std::move(newImage);
    setModifiable(image.get());

    setCaption(image->file());

    clear();
    addPixmap(image->toPixmap());
}
