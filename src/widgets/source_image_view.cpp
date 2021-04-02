#include "source_image_view.h"

SourceImageView::SourceImageView(QWidget* parent)
  : ImageView(parent)
{
    setModifiable();

    connect(this, &ImageView::imageClosed, [this]() {
        if (image) {
            emit image->imageClosed();
        }
    });
    connect(this, &ImageView::imageReplaced, [this](QStringList const& files) {
        if (image) {
            emit image->imageReplaced(files);
        }
    });
}

void SourceImageView::setImage(std::shared_ptr<Image> newImage)
{
    image = std::move(newImage);

    setCaption(image->file());

    clear();
    addPixmap(image->toPixmap());
}
