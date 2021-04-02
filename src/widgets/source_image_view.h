#pragma once

#include "image.h"
#include "widgets/image_view.h"

#include <memory>

class SourceImageView : public ImageView {
    Q_OBJECT

public:
    explicit SourceImageView(QWidget* parent = nullptr);

    void setImage(std::shared_ptr<Image> newImage);

private:
    std::shared_ptr<Image> image;
};
