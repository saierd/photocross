#include "session_view.h"

#include "session.h"
#include "widgets/mouse_indicator.h"
#include "widgets/source_image_view.h"

#include <QGraphicsItem>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QScopeGuard>

#include <algorithm>
#include <cmath>
#include <vector>

#include "ui_session_view.h"

namespace {

class ImageViewsUpdateBlocker {
public:
    void block(ImageView& imageView)
    {
        reenableUpdates.push_back(imageView.delayUpdates());
    }

private:
    std::vector<ImageView::DelayUpdates> reenableUpdates;
};

}  // namespace

SessionView::SessionView(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::SessionView>();
    ui->setupUi(this);

    initializeImageView(*ui->comparisonView);

    connect(ui->splitter, &QSplitter::splitterMoved, this, [this]() {
        adaptViewToWindow();
        ui->comparisonView->forceViewPropagation();

        sourceImagesVisible = (ui->splitter->sizes().at(0) > 0);
        emit sourceImagesVisibleChanged(getSourceImagesVisible());
    });

    connect(ui->comparisonSettings,
            &ComparisonSettingsWidget::settingsChanged,
            this,
            &SessionView::updateComparisonView);
    connect(ui->comparisonView, &ComparisonView::requestAnimationUpdate, this, &SessionView::updateComparisonView);
    connect(ui->comparisonView, &ImageView::canSaveImageChanged, this, &SessionView::canSaveComparisonViewChanged);
}

SessionView::~SessionView() = default;

void SessionView::setSession(Session* _session)
{
    session = _session;
    ui->sourceImages->setSession(session);

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

bool SessionView::canSaveComparisonView() const
{
    return ui->comparisonView->canSaveImage();
}

void SessionView::flipLayoutDirection()
{
    bool newLayoutHorizontal = !getLayoutIsHorizontal();
    if (newLayoutHorizontal) {
        ui->splitter->setOrientation(Qt::Vertical);
        ui->sourceImages->setDirection(QBoxLayout::LeftToRight);
        ui->comparisonLayout->setDirection(QBoxLayout::LeftToRight);
    } else {
        ui->splitter->setOrientation(Qt::Horizontal);
        ui->sourceImages->setDirection(QBoxLayout::TopToBottom);
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

void SessionView::fitToView()
{
    if (session->getImages().empty()) {
        return;
    }

    if (auto* view = selectImageViewForFitToView(); view != nullptr) {
        view->fitViewToScene();
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

void SessionView::saveComparisonView()
{
    ui->comparisonView->saveImage();
}

void SessionView::adjustNumberOfImageViews(size_t numImages)
{
    // Clear mouse indicators to avoid dangling pointers to items in deleted image views.
    removeMouseIndicatorsFromScenes();

    if (!imageViews.empty()) {
        while (imageViews.size() > numImages) {
            ui->sourceImages->popImageWidget();
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
        ui->sourceImages->addImageWidget(newImageView.release());
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
        imageViews[i]->setImage(image, session->getImages());
    }

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
    // Delay updating the image views until all changes are done (including updating the mouse indicators). Otherwise
    // they might flicker.
    ImageViewsUpdateBlocker imageViewUpdates;
    imageViewUpdates.block(*ui->comparisonView);
    for (auto const& imageView : imageViews) {
        imageViewUpdates.block(*imageView);
    }

    // Block mouse indicator updates while we update the view in case there is a mouse move event. We will restore the
    // previous mouse indicator position at the end of this function.
    mouseIndicatorUpdateBlocked = true;
    removeMouseIndicatorsFromScenes();
    auto restoreMouseIndicators = qScopeGuard([this]() {
        mouseIndicatorUpdateBlocked = false;
        updateMouseIndicators();
    });

    auto const& images = session->getImages();
    auto settings = ui->comparisonSettings->getComparisonSettings();

    bool const comparisonVisible = (images.size() >= 2);
    ui->comparisonView->setVisible(comparisonVisible);
    ui->comparisonSettings->setVisible(comparisonVisible);

    if (!comparisonVisible) {
        setSourceImagesVisible(getSourceImagesVisible());
        return;
    }

    ui->comparisonView->update(session->getImages(), settings);
}

void SessionView::imageViewReachedZoomOutLimit()
{
    // Select tightest image views with a bit of tolerance in order to snap to the fit to view functionality when zoom
    // is close enough, even if zooming happened in an image view that is technically not the tightest.
    for (ImageView* view : tightestImageViews(0.01)) {
        if (sender() == view) {
            // Zoomed out in the tightest image view that decides the zoom level for the fit to view function.
            // Automatically enable the fit to view functionality again.
            fitToView();
            return;
        }
    }
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
    if (mouseIndicatorUpdateBlocked) {
        return;
    }

    if (lastMouseIndicatorPosition) {
        updateMouseIndicators(*lastMouseIndicatorPosition);
    } else {
        removeMouseIndicatorsFromScenes();
    }
}

void SessionView::updateMouseIndicators(MouseIndicatorPosition const& position)
{
    if (mouseIndicatorUpdateBlocked) {
        // Update is blocked for now. We still remember the new position so that the position is up to date when we
        // restore the mouse indicators later.
        if (lastMouseIndicatorPosition) {
            lastMouseIndicatorPosition = position;
        }
        return;
    }

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

void SessionView::keyPressEvent(QKeyEvent* event)
{
    if (session != nullptr && session->positionSelectionModeEnabled() && event->matches(QKeySequence::Cancel)) {
        session->disablePositionSelectionMode();
        return;
    }

    QWidget::keyPressEvent(event);
}

void SessionView::initializeImageView(ImageView& imageView)
{
    connect(&imageView, &ImageView::zoomChangedExplicitly, this, [this]() {
        setAutoFitInView(false);
    });
    connect(&imageView, &ImageView::zoomOutLimitReached, this, &SessionView::imageViewReachedZoomOutLimit);
    connect(&imageView.getScene(),
            &ImageViewScene::mouseMoved,
            this,
            qOverload<MouseIndicatorPosition const&>(&SessionView::updateMouseIndicators));
    connect(&imageView.getScene(), &ImageViewScene::mouseLeft, this, &SessionView::clearMouseIndicators);
}

namespace {

double computeSceneToViewSizeRatio(ImageView const* view)
{
    return std::max(view->getScene().sceneRect().width() / view->width(),
                    view->getScene().sceneRect().height() / view->height());
}

std::vector<ImageView*> selectTightestImageViews(std::vector<ImageView*> views, double tolerance)
{
    auto* tightest = *std::max_element(views.begin(), views.end(), [](ImageView const* a, ImageView const* b) {
        return computeSceneToViewSizeRatio(a) < computeSceneToViewSizeRatio(b);
    });

    std::vector<ImageView*> result;
    std::copy_if(views.begin(), views.end(), std::back_inserter(result), [&](ImageView const* view) {
        if (computeSceneToViewSizeRatio(tightest) == computeSceneToViewSizeRatio(view)) {
            return true;
        }

        return std::abs(computeSceneToViewSizeRatio(tightest) / computeSceneToViewSizeRatio(view) - 1) < tolerance;
    });

    return result;
}

}  // namespace

std::vector<ImageView*> SessionView::tightestImageViews(double tolerance)
{
    std::vector<ImageView*> views;
    if (getSourceImagesVisible()) {
        std::copy(imageViews.begin(), imageViews.end(), std::back_inserter(views));
    }
    if (ui->comparisonView->isVisible()) {
        views.push_back(ui->comparisonView);
    }

    if (views.empty()) {
        return {};
    }

    return selectTightestImageViews(views, tolerance);
}

ImageView* SessionView::selectImageViewForFitToView()
{
    auto tightestViews = tightestImageViews();

    if (tightestViews.empty()) {
        return nullptr;
    }

    return tightestViews[0];
}