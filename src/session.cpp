#include "session.h"

#include "image_diff.h"

void Session::loadImages(QStringList const& filenames)
{
    for (auto const& filename : filenames) {
        images.emplace_back(std::make_unique<Image>(filename));

        images.back()->setReloadWhenFileChanges(watchFiles);
        connect(images.back().get(), &Image::imageChanged, this, &Session::imagesChanged);
    }

    emit(imagesChanged());
}

bool Session::getWatchFiles() const
{
    return watchFiles;
}

void Session::setWatchFiles(bool enable)
{
    watchFiles = enable;
    for (auto& image : images) {
        image->setReloadWhenFileChanges(watchFiles);
    }

    emit(watchFilesChanged(enable));
}

std::vector<std::unique_ptr<Image>> const& Session::getImages() const&
{
    return images;
}

QImage Session::comparisonImage() const
{
    if (images.size() < 2) {
        return QImage();
    }

    // TODO: Handle more than 2 images.
    return imageDiff(images[0]->image(), images[1]->image());
}

void Session::reload()
{
    for (auto& image : images) {
        image->reload();
    }
    emit(imagesChanged());
}
