#include "session_view.h"

#include "image_difference.h"
#include "session.h"
#include "widgets/image_view.h"

#include <QGraphicsColorizeEffect>
#include <QGraphicsPixmapItem>

#include <cmath>

#include "ui_session_view.h"

SessionView::SessionView(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::SessionView>();
    ui->setupUi(this);

    ui->comparisonView->setCaption("Comparison View");
    connect(ui->comparisonView, &ImageView::zoomChangedExplicitly, [this]() {
        setAutoFitInView(false);
    });

    connect(ui->splitter, &QSplitter::splitterMoved, [this]() {
        adaptViewToWindow();
        ui->comparisonView->forceViewPropagation();

        sourceImagesVisible = (ui->splitter->sizes()[0] > 0);
        emit sourceImagesVisibleChanged(getSourceImagesVisible());
    });

    connect(ui->comparisonSettings, &ComparisonSettings::settingsChanged, this, &SessionView::updateComparisonView);
}

SessionView::~SessionView() = default;

void SessionView::setSession(Session* _session)
{
    session = _session;

    connect(session, &Session::imagesChanged, this, &SessionView::updateImages);
    updateImages();
}

bool SessionView::getSourceImagesVisible() const
{
    return sourceImagesVisible;
}

void SessionView::setSourceImagesVisible(bool enable)
{
    sourceImagesVisible = enable;

    // QSplitter::setSizes interprets 0 as "hide the widget" and everything else as the minimum size of the widget. Any
    // leftover available space after assigning the minimum size will be split evenly between images and comparison
    // view.
    int imageWidgetSize = enable ? 1 : 0;
    ui->splitter->setSizes({imageWidgetSize, 1});

    adaptViewToWindow();

    emit sourceImagesVisibleChanged(getSourceImagesVisible());
}

bool SessionView::getAutoFitInView() const
{
    return autoFitInView;
}

void SessionView::setAutoFitInView(bool enable)
{
    autoFitInView = enable;
    emit autoFitInViewChanged(autoFitInView);
}

void SessionView::flipLayoutDirection()
{
    if (ui->splitter->orientation() == Qt::Horizontal) {
        ui->splitter->setOrientation(Qt::Vertical);
        ui->imagesLayout->setDirection(QBoxLayout::LeftToRight);
        ui->comparisonLayout->setDirection(QBoxLayout::LeftToRight);
    } else {
        ui->splitter->setOrientation(Qt::Horizontal);
        ui->imagesLayout->setDirection(QBoxLayout::TopToBottom);
        ui->comparisonLayout->setDirection(QBoxLayout::TopToBottom);
    }

    // Make Qt compute the new sizes of the image views.
    QApplication::processEvents();

    adaptViewToWindow();

    // Flipping the layout might change the center point visible in the image if the image does not fit the graphics
    // view. Force synchronization of the views so that they are all centered on the same point again.
    ui->comparisonView->forceViewPropagation();
}

void SessionView::zoomIn()
{
    ui->comparisonView->zoomIn();
}

void SessionView::zoomOut()
{
    ui->comparisonView->zoomOut();
}

void SessionView::fitToView()
{
    if (session->getImages().empty()) {
        return;
    }

    auto const& pivotImage = session->getImages()[0]->image();
    bool imageIsLandscape = pivotImage.width() >= pivotImage.height();

    ImageView* smallestImageView = nullptr;

    auto viewIsSmallerThanCurrentSmallest = [&](ImageView* view) -> bool {
        if (imageIsLandscape) {
            return view->width() < smallestImageView->width();
        }
        return view->height() < smallestImageView->height();
    };

    auto selectViewIfSmaller = [&](ImageView* view) {
        if (smallestImageView == nullptr || viewIsSmallerThanCurrentSmallest(view)) {
            smallestImageView = view;
        }
    };

    if (getSourceImagesVisible()) {
        for (auto const& view : imageViews) {
            selectViewIfSmaller(view);
        }
    }
    selectViewIfSmaller(ui->comparisonView);

    if (smallestImageView != nullptr) {
        smallestImageView->fitViewToScene();
        setAutoFitInView(true);
    }
}

void SessionView::fitComparisonImageToView()
{
    ui->comparisonView->fitViewToScene();
    setAutoFitInView(false);
}

void SessionView::updateImages()
{
    size_t numImages = session->getImages().size();
    size_t previousNumImages = imageViews.size();

    ui->comparisonSettings->setNumberOfImages(numImages);

    // Adapt the number of image widgets to the number of images.
    if (!imageViews.empty()) {
        for (size_t i = imageViews.size() - 1; i >= numImages; i--) {
            delete ui->imagesLayout->takeAt(i);
            imageViews.pop_back();
        }
    }
    while (imageViews.size() < numImages) {
        auto newImageView = std::make_unique<ImageView>();

        for (auto const& existingImageView : imageViews) {
            existingImageView->synchronizeViews(*newImageView);
        }
        ui->comparisonView->synchronizeViews(*newImageView);

        connect(newImageView.get(), &ImageView::zoomChangedExplicitly, [this]() {
            setAutoFitInView(false);
        });

        imageViews.push_back(newImageView.get());
        ui->imagesLayout->addWidget(newImageView.release());
    }

    for (size_t i = 0; i < numImages; i++) {
        auto const& image = *(session->getImages()[i]);
        auto* imageView = imageViews[i];

        imageView->setCaption(image.file());
        imageView->clear();
        imageView->addPixmap(image.toPixmap());
    }

    updateComparisonView();

    if (previousNumImages == 0) {
        // Initial load of images. Set up the layout of the view properly.
        setSourceImagesVisible(getSourceImagesVisible());
        fitToView();
    }
}

void addColorFilter(QGraphicsPixmapItem* item, QColor const& color)
{
    if (!color.isValid()) {
        return;
    }

    auto effect = std::make_unique<QGraphicsColorizeEffect>();
    effect->setColor(color);
    item->setGraphicsEffect(effect.release());
}

void SessionView::updateComparisonView()
{
    auto const& images = session->getImages();
    auto const& settings = *ui->comparisonSettings;
    auto& comparisonScene = ui->comparisonView->getScene();

    comparisonScene.clear();
    if (images.empty()) {
        return;
    }

    ComparisonMode comparisonMode = settings.getComparisonMode();
    if (comparisonMode == ComparisonMode::HighlightDifferences) {
        QImage differenceImage;
        if (images.size() >= 2) {
            std::vector<QImage const*> imagesToCompare;
            imagesToCompare.reserve(images.size());
            for (auto const& image : images) {
                imagesToCompare.push_back(&image->image());
            }

            differenceImage = computeDifferenceImage(imagesToCompare,
                                                     settings.getDifferenceTolerance(),
                                                     settings.showMinorDifferences());
        }

        ui->comparisonView->addPixmap(images[0]->toGrayscalePixmap());
        ui->comparisonView->addPixmap(QPixmap::fromImage(differenceImage), 0.7);
    } else if (comparisonMode == ComparisonMode::BlendImages && images.size() >= 2) {
        double const sequenceBlendPosition = settings.getBlendPosition() * static_cast<double>(images.size() - 1);

        auto firstImageIndex = static_cast<size_t>(std::floor(sequenceBlendPosition));
        firstImageIndex = std::min(firstImageIndex, images.size() - 2);

        double const blendPosition = sequenceBlendPosition - firstImageIndex;

        auto* firstImage = comparisonScene.addPixmap(images[firstImageIndex]->toPixmap());
        auto* secondImage = comparisonScene.addPixmap(images[firstImageIndex + 1]->toPixmap());
        secondImage->setOpacity(blendPosition);

        QColor firstImageColor = settings.blendImage1Color();
        QColor secondImageColor = settings.blendImage2Color();
        if (firstImageIndex % 2 != 0) {
            std::swap(firstImageColor, secondImageColor);
        }

        addColorFilter(firstImage, firstImageColor);
        addColorFilter(secondImage, secondImageColor);
    }
}

void SessionView::adaptViewToWindow()
{
    if (autoFitInView) {
        fitToView();
    }
}

void SessionView::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    adaptViewToWindow();
}
