#include "source_image_view.h"

#include "string_helpers.h"

namespace {

QString highlight(QString const& string)
{
    return QString("<b><font color=\"darkred\">%1</color></b>").arg(string);
}

QString formatImageSize(Image const& image, std::vector<std::shared_ptr<Image>> const& allImages)
{
    QSize size = image.size();

    bool widthDifferent = false;
    bool heightDifferent = false;
    for (auto const& otherImage : allImages) {
        if (size.width() != otherImage->size().width()) {
            widthDifferent = true;
        }
        if (size.height() != otherImage->size().height()) {
            heightDifferent = true;
        }
    }

    QString widthString = QString::number(size.width());
    if (widthDifferent) {
        widthString = highlight(widthString);
    }

    QString heightString = QString::number(size.height());
    if (heightDifferent) {
        heightString = highlight(heightString);
    }

    return QString("%1 × %2 pixels").arg(widthString, heightString);
}

QString commonFilenamePrefix(std::vector<std::shared_ptr<Image>> const& allImages)
{
    QString prefix = allImages[0]->file();
    for (auto const& image : allImages) {
        prefix = commonPrefix(prefix, image->file());
    }

    return prefix;
}

QString simplifyFilename(QString const& filename, std::vector<std::shared_ptr<Image>> const& allImages)
{
    return filename.right(filename.size() - commonFilenamePrefix(allImages).size());
}

}  // namespace

void SourceImageView::setImage(std::shared_ptr<Image> newImage, std::vector<std::shared_ptr<Image>> const& allImages)
{
    image = std::move(newImage);
    setModifiable(image.get());

    QString filename = image->file();
    QString caption = simplifyFilename(filename, allImages) + " (" + formatImageSize(*image, allImages) + ")";
    QString tooltip = image->canonicalFilename();

    setCaption(caption, tooltip);

    clear();
    addPixmap(image->toPixmap());
}
