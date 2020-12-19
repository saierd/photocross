#include "session.h"

#include "image_diff.h"

Session::Session(std::vector<Image> _images)
  : images(std::move(_images))
{
}

void Session::loadImages(QStringList const& filenames)
{
    for (auto const& filename : filenames) {
        images.emplace_back(filename);
    }

    emit(imagesChanged());
}

std::vector<Image> const& Session::getImages() const&
{
    return images;
}

QImage Session::comparisonImage() const
{
    if (images.size() < 2) {
        return QImage();
    }

    // TODO: Handle more than 2 images.
    return imageDiff(images[0].image(), images[1].image());
}
