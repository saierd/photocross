#pragma once

#include "image.h"

#include <vector>

class Session : public QObject {
    Q_OBJECT

public:
    explicit Session(std::vector<Image> _images);

    Image const& image1() const&;
    Image const& image2() const&;

    QImage comparisonImage() const;

signals:
    void image1Changed() const;
    void image2Changed() const;

private:
    std::vector<Image> images;
};
