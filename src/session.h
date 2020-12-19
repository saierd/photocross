#pragma once

#include "image.h"

#include <vector>

class Session : public QObject {
    Q_OBJECT

public:
    explicit Session(std::vector<Image> _images = {});

    void loadImages(QStringList const& filenames);

    std::vector<Image> const& getImages() const&;
    QImage comparisonImage() const;

signals:
    void imagesChanged() const;

private:
    std::vector<Image> images;
};
