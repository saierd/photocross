#include "session_view.h"

#include "session.h"
#include "widgets/source_image_view.h"

#include "ui_session_view.h"

SessionView::SessionView(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::SessionView>();
    ui->setupUi(this);

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

    connect(ui->emptyImage, &EmptyImage::imagesDropped, [this](QStringList const& files) {
        session->loadImages(files);
    });
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

    // Forcibly display the source images if there are not enough images to show the comparison view yet. The source
    // image view will also include the placeholder where images can be drag and dropped to.
    bool showSourceImages = sourceImagesVisible || (imageViews.size() < 2);

    // QSplitter::setSizes interprets 0 as "hide the widget" and everything else as the minimum size of the widget. Any
    // leftover available space after assigning the minimum size will be split evenly between images and comparison
    // view.
    int imageWidgetSize = showSourceImages ? 1 : 0;
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
    if (ui->comparisonView->isVisible()) {
        selectViewIfSmaller(ui->comparisonView);
    }

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

void SessionView::adjustNumberOfImageViews(size_t numImages)
{
    if (!imageViews.empty()) {
        while (imageViews.size() > numImages) {
            auto* item = ui->imagesLayout->takeAt(static_cast<int>(imageViews.size()) - 1);
            item->widget()->deleteLater();
            delete item;

            imageViews.pop_back();
        }
    }
    while (imageViews.size() < numImages) {
        auto newImageView = std::make_unique<SourceImageView>();

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
}

void SessionView::updateImages()
{
    size_t numImages = session->getImages().size();
    size_t previousNumImages = imageViews.size();

    ui->comparisonSettings->setNumberOfImages(numImages);
    adjustNumberOfImageViews(numImages);

    for (size_t i = 0; i < numImages; i++) {
        auto const& image = session->getImages()[i];
        imageViews[i]->setImage(image);
    }

    ui->emptyImage->setVisible(numImages < 2);

    updateComparisonView();

    if (numImages > 0 && previousNumImages < 2) {
        // Initial load of images. Set up the layout of the view properly.
        QApplication::processEvents();
        setSourceImagesVisible(getSourceImagesVisible());
        fitToView();
    }

    // Adapt view to window in case the number of image views (and the available space for each of them) changed.
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    adaptViewToWindow();
}

void SessionView::updateComparisonView()
{
    auto const& images = session->getImages();
    auto const& settings = *ui->comparisonSettings;

    ui->comparisonView->clear();

    bool const comparisonVisible = (images.size() >= 2);
    ui->comparisonView->setVisible(comparisonVisible);
    ui->comparisonSettings->setVisible(comparisonVisible);

    if (!comparisonVisible) {
        setSourceImagesVisible(getSourceImagesVisible());
        return;
    }

    ui->comparisonView->update(*session, settings);
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
