#pragma once

#include "image.h"

#include <memory>
#include <vector>

class Session : public QObject {
    Q_OBJECT

public:
    using ImageHandle = std::shared_ptr<Image>;
    using Images = std::vector<ImageHandle>;

    bool getWatchFiles() const;
    void setWatchFiles(bool enable);

    bool positionSelectionModeEnabled() const;

    Images const& getImages() const;

signals:
    void watchFilesChanged(bool enabled);
    void imagesChanged();

    void positionSelectionModeChanged(bool enabled);
    void imagePositionSelected(Image* image, QPoint const& point);

public slots:
    void loadImages(QStringList const& files);
    void addImage(QImage image);
    void replaceImage(Session::ImageHandle const& image, QStringList const& files);
    void closeImage(Session::ImageHandle const& image);

    void reload();

    void enablePositionSelectionMode(bool enabled = true);
    void disablePositionSelectionMode();

private:
    Images::const_iterator insertImage(Images::const_iterator position, ImageHandle image);
    void insertImages(Images::const_iterator position, Images const& newImages);

    Images images;

    bool watchFiles = true;
    bool isPositionSelectionModeEnabled = false;
};
