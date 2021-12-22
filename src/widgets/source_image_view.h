#pragma once

#include "image.h"
#include "widgets/image_view.h"

#include <memory>
#include <vector>

class SourceImageView : public ImageView {
    Q_OBJECT

public:
    void setImage(std::shared_ptr<Image> newImage, std::vector<std::shared_ptr<Image>> const& allImages);

private:
    std::shared_ptr<Image> image;
};
