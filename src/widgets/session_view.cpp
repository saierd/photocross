#include "session_view.h"

#include "session.h"
#include "widgets/image_view.h"

#include "ui_session_view.h"

SessionView::SessionView(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::SessionView>();
    ui->setupUi(this);

    ui->comparisonView->setCaption("Comparison View");

    connect(ui->splitter, &QSplitter::splitterMoved, [this]() {
        ui->comparisonView->forceViewPropagation();
    });
}

SessionView::~SessionView() = default;

void SessionView::setSession(Session* _session)
{
    session = _session;

    connect(session, &Session::imagesChanged, this, &SessionView::updateImages);
    updateImages();
}

void SessionView::flipLayoutDirection()
{
    if (ui->splitter->orientation() == Qt::Horizontal) {
        ui->splitter->setOrientation(Qt::Vertical);
        ui->imagesLayout->setDirection(QBoxLayout::LeftToRight);
    } else {
        ui->splitter->setOrientation(Qt::Horizontal);
        ui->imagesLayout->setDirection(QBoxLayout::TopToBottom);
    }

    // Flipping the layout might change the center point visible in the image if the image does not fit the graphics
    // view. Force synchronization of the views so that they are all centered on the same point again.
    QApplication::processEvents();
    ui->comparisonView->forceViewPropagation();
}

void SessionView::fitToView()
{
    // TODO: Fit in smallest image view.
}

void SessionView::updateImages()
{
    size_t numImages = session->getImages().size();
    size_t previousNumImages = imageViews.size();

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

        imageViews.push_back(newImageView.get());
        ui->imagesLayout->addWidget(newImageView.release());
    }

    for (size_t i = 0; i < numImages; i++) {
        auto const& image = session->getImages()[i];
        auto* imageView = imageViews[i];

        imageView->setCaption(image.file());
        imageView->clear();
        imageView->addPixmap(image.toPixmap());
    }

    updateComparisonView();

    if (previousNumImages == 0) {
        // Initial load of images. Set up the layout of the view properly.

        // Split space evenly between images and comparison view. This line will assign each widget a size of 1px and
        // the rest of the available space will be distributed equally between them. Note that a size of 0 has a special
        // meaning for this function and hides the widgets completely.
        ui->splitter->setSizes({1, 1});

        fitToView();
    }
}

void SessionView::updateComparisonView()
{
    ui->comparisonView->clear();
    if (!session->getImages().empty()) {
        ui->comparisonView->addPixmap(session->getImages()[0].toGrayscalePixmap());
        ui->comparisonView->addPixmap(QPixmap::fromImage(session->comparisonImage()), 0.7);
    }
}
