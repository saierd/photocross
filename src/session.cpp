#include "session.h"

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

    emit watchFilesChanged(enable);
}

std::vector<Session::ImageHandle> const& Session::getImages() const
{
    return images;
}

Session::Images loadImagesFromFiles(QStringList const& files)
{
    Session::Images result;
    result.reserve(files.size());
    std::transform(files.begin(), files.end(), std::back_inserter(result), [](QString const& file) {
        return std::make_shared<Image>(file);
    });
    return result;
}

void Session::loadImages(QStringList const& files)
{
    if (files.empty()) {
        return;
    }

    insertImages(images.end(), loadImagesFromFiles(files));
    emit imagesChanged();
}

void Session::replaceImage(ImageHandle const& image, QStringList const& files)
{
    auto it = std::find(images.begin(), images.end(), image);
    if (it != images.end()) {
        it = images.erase(it);
    }

    insertImages(it, loadImagesFromFiles(files));

    emit imagesChanged();
}

void Session::closeImage(ImageHandle const& image)
{
    auto it = std::find(images.begin(), images.end(), image);
    if (it != images.end()) {
        images.erase(it);
        emit imagesChanged();
    }
}

void Session::reload()
{
    for (auto& image : images) {
        image->reload();
    }
    emit imagesChanged();
}

void Session::insertImages(Images::const_iterator position, Images const& newImages)
{
    for (auto const& image : newImages) {
        image->setReloadWhenFileChanges(watchFiles);
        connect(image.get(), &Image::imageChanged, this, &Session::imagesChanged);

        std::weak_ptr<Image> imageReference = image;

        connect(image.get(), &Image::imageClosed, [this, imageReference]() {
            if (auto imageHandle = imageReference.lock()) {
                closeImage(imageHandle);
            }
        });

        connect(image.get(), &Image::imageReplaced, [this, imageReference](QStringList const& files) {
            if (auto imageHandle = imageReference.lock()) {
                replaceImage(imageHandle, files);
            }
        });
    }

    images.insert(position, std::make_move_iterator(newImages.begin()), std::make_move_iterator(newImages.end()));
}
