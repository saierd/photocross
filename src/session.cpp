#include "session.h"

#include "image_diff.h"

Session::Session(std::vector<Image> _images)
  : images(std::move(_images))
{
}

Image const& Session::image1() const&
{
    return images[0];
}

Image const& Session::image2() const&
{
    return images[1];
}

QImage Session::comparisonImage() const
{
    return imageDiff(images[0].image(), images[1].image());
}
