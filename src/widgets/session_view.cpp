#include "session_view.h"

#include "session.h"
#include "widgets/mouse_indicator.h"
#include "widgets/source_image_view.h"

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QScopeGuard>

#include <algorithm>
#include <cmath>
#include <vector>

#include "ui_session_view.h"

SessionView::SessionView(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::SessionView>();
    ui->setupUi(this);

    initializeImageView(*ui->comparisonView);

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

bool SessionView::getMouseIndicatorsVisible() const
{
    return mouseIndicatorsVisible;
}

void SessionView::setMouseIndicatorsVisible(bool enable)
{
    mouseIndicatorsVisible = enable;
    updateMouseIndicators();

    emit mouseIndicatorsVisibleChanged(getMouseIndicatorsVisible());
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

bool SessionView::getLayoutIsHorizontal() const
{
    // The splitter's layout is the inverse of what we call the layout, since e.g. the list of source images is layed
    // out perpendicular to the splitter itself.
    return ui->splitter->orientation() != Qt::Horizontal;
}

void SessionView::flipLayoutDirection()
{
    bool newLayoutHorizontal = !getLayoutIsHorizontal();
    if (newLayoutHorizontal) {
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

    emit layoutDirectionChanged(newLayoutHorizontal);
}

void SessionView::zoomIn()
{
    ui->comparisonView->zoomIn();
}

void SessionView::zoomOut()
{
    ui->comparisonView->zoomOut();
}

double computeSceneToViewSizeRatio(ImageView const* view)
{
    return std::max(view->getScene().sceneRect().width() / view->width(),
                    view->getScene().sceneRect().height() / view->height());
}

ImageView* selectTightestImageView(std::vector<ImageView*> views)
{
    return *std::max_element(views.begin(), views.end(), [](ImageView const* a, ImageView const* b) {
        return computeSceneToViewSizeRatio(a) < computeSceneToViewSizeRatio(b);
    });
}

void SessionView::fitToView()
{
    if (session->getImages().empty()) {
        return;
    }

    std::vector<ImageView*> views;
    if (getSourceImagesVisible()) {
        std::copy(imageViews.begin(), imageViews.end(), std::back_inserter(views));
    }
    if (ui->comparisonView->isVisible()) {
        views.push_back(ui->comparisonView);
    }

    if (!views.empty()) {
        auto* tightestImageView = selectTightestImageView(views);
        tightestImageView->fitViewToScene();
        setAutoFitInView(true);
    }
}

void SessionView::fitComparisonImageToView()
{
    ui->comparisonView->fitViewToScene();
    setAutoFitInView(false);
}

void SessionView::rotateImagesLeft()
{
    for (auto const& image : session->getImages()) {
        image->rotateLeft();
    }
    adaptViewToWindow();
}

void SessionView::rotateImagesRight()
{
    for (auto const& image : session->getImages()) {
        image->rotateRight();
    }
    adaptViewToWindow();
}

void SessionView::adjustNumberOfImageViews(size_t numImages)
{
    // Clear mouse indicators to avoid dangling pointers to items in deleted image views.
    removeMouseIndicatorsFromScenes();

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
        initializeImageView(*newImageView);

        for (auto const& existingImageView : imageViews) {
            existingImageView->synchronizeViews(*newImageView);
        }
        ui->comparisonView->synchronizeViews(*newImageView);

        imageViews.push_back(newImageView.get());
        ui->imagesLayout->addWidget(newImageView.release());
    }
}

void SessionView::updateImages()
{
    ui->comparisonView->rememberView();

    // Clear mouse indicators before changing images. Note that we do not need to restore them in this method, since the
    // updateComparisonView method will do that for us.
    removeMouseIndicatorsFromScenes();

    // Update the comparison view first. We do this before updating the images, because it usually takes longer and
    // might show a progress dialog while it runs. Changes of the source images should not be reflected in the UI yet
    // while this runs.
    updateComparisonView();

    size_t numImages = session->getImages().size();
    size_t previousNumImages = imageViews.size();

    ui->comparisonSettings->setNumberOfImages(numImages);
    adjustNumberOfImageViews(numImages);

    for (size_t i = 0; i < numImages; i++) {
        auto const& image = session->getImages()[i];
        imageViews[i]->setImage(image);
    }

    ui->emptyImage->setVisible(numImages < 2);

    if (numImages > 0 && previousNumImages < 2) {
        // Initial load of images. Set up the layout of the view properly.
        QApplication::processEvents();
        setSourceImagesVisible(getSourceImagesVisible());
        fitToView();
    }

    ui->comparisonView->restoreView();

    // Adapt view to window in case the number of image views (and the available space for each of them) changed.
    QApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
    adaptViewToWindow();
}

void SessionView::updateComparisonView()
{
    removeMouseIndicatorsFromScenes();
    auto restoreMouseIndicators = qScopeGuard([this]() {
        updateMouseIndicators();
    });

    auto const& images = session->getImages();
    auto const& settings = *ui->comparisonSettings;

    bool const comparisonVisible = (images.size() >= 2);
    ui->comparisonView->setVisible(comparisonVisible);
    ui->comparisonSettings->setVisible(comparisonVisible);

    if (!comparisonVisible) {
        setSourceImagesVisible(getSourceImagesVisible());
        return;
    }

    ui->comparisonView->update(*session, settings);
}

void SessionView::clearMouseIndicators()
{
    lastMouseIndicatorPosition.reset();
    updateMouseIndicators();
}

void SessionView::removeMouseIndicatorsFromScenes()
{
    for (auto* item : mouseIndicatorItems) {
        auto* scene = item->scene();
        scene->removeItem(item);
        // Redraw the scene where the indicator was to remove it from the screen.
        scene->update(item->sceneTransform().mapRect(item->boundingRect()));
        delete item;
    }
    mouseIndicatorItems.clear();
}

void SessionView::updateMouseIndicators()
{
    if (lastMouseIndicatorPosition) {
        updateMouseIndicators(*lastMouseIndicatorPosition);
    } else {
        removeMouseIndicatorsFromScenes();
    }
}

void SessionView::updateMouseIndicators(MouseIndicatorPosition const& position)
{
    removeMouseIndicatorsFromScenes();

    if (!mouseIndicatorsVisible) {
        return;
    }

    auto addMouseIndicator = [&](QGraphicsView& view) {
        if (view.scene() == position.hoveredScene) {
            return;
        }

        int const scenePixelSize = (view.mapFromScene(QPoint(1, 0)) - view.mapFromScene(QPoint(0, 0))).x();
        if (scenePixelSize < 3) {
            // Image view is zoomed out far and the mouse indicator would be very small and difficult to see. Do not
            // draw it until the image is zoomed in more.
            return;
        }

        auto mouseIndicator = std::make_unique<MouseIndicatorGraphicsItem>();

        QPointF hoveredPixel = (position.sceneCoordinates + QPointF(0.5, 0.5)).toPoint() - QPointF(0.5, 0.5);
        mouseIndicator->setPos(hoveredPixel);

        // Only add the indicator if it is not outside of the displayed image. Otherwise it would extend the bounding
        // box of the scene and change the scrolling behavior of the graphics view.
        auto mouseIndicatorRect = mouseIndicator->boundingRect().translated(hoveredPixel);
        if (view.scene()->sceneRect().contains(mouseIndicatorRect)) {
            mouseIndicatorItems.push_back(mouseIndicator.get());
            view.scene()->addItem(mouseIndicator.release());
        }
    };

    for (auto const& imageView : imageViews) {
        addMouseIndicator(imageView->getGraphicsView());
    }
    addMouseIndicator(ui->comparisonView->getGraphicsView());

    lastMouseIndicatorPosition = position;
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

void SessionView::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    adaptViewToWindow();
}

void SessionView::initializeImageView(ImageView& imageView)
{
    connect(&imageView, &ImageView::zoomChangedExplicitly, [this]() {
        setAutoFitInView(false);
    });
    connect(&imageView.getScene(),
            &ImageViewScene::mouseMoved,
            this,
            qOverload<MouseIndicatorPosition const&>(&SessionView::updateMouseIndicators));
    connect(&imageView.getScene(), &ImageViewScene::mouseLeft, this, &SessionView::clearMouseIndicators);
}
