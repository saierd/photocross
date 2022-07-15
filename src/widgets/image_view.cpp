#include "image_view.h"

#include "image.h"
#include "image_edit_menu.h"
#include "utility/file.h"

#include <QGraphicsOpacityEffect>
#include <QGraphicsPixmapItem>
#include <QStackedLayout>

#include "ui_image_view.h"

namespace {

void setUpDropIndicator(EmptyImage* dropIndicator)
{
    dropIndicator->setLabelText(QApplication::tr("Replace Image"));
    dropIndicator->hideBorder();
    dropIndicator->hide();

    // We only use the drop indicator as a visual indicator, but handle drop events ourselves.
    dropIndicator->setAcceptDrops(false);

    // Make the indicator transparent so that once can still see the original image in the background.
    dropIndicator->setAutoFillBackground(true);

    auto* effect = new QGraphicsOpacityEffect(dropIndicator);
    effect->setOpacity(0.95);
    dropIndicator->setGraphicsEffect(effect);
}

}  // namespace

ImageView::ImageView(QWidget* parent)
  : ImageDropWidget(parent)
{
    ui = std::make_unique<Ui::ImageView>();
    ui->setupUi(this);

    ui->widgetStack->setCurrentWidget(ui->viewPage);
    dynamic_cast<QStackedLayout*>(ui->widgetStack->layout())->setStackingMode(QStackedLayout::StackAll);

    setUpDropIndicator(ui->dropIndicator);

    ui->fitToView->setDefaultAction(ui->actionFitToView);
    connect(ui->actionFitToView, &QAction::triggered, this, [this]() {
        fitViewToScene();
        emit zoomChangedExplicitly();
    });

    editMenu = new ImageEditMenuAction(this);
    ui->editImage->addAction(editMenu);

    ui->closeImage->setDefaultAction(ui->actionCloseImage);
    connect(ui->actionCloseImage, &QAction::triggered, this, [this]() {
        if (modifiable && imageToModify != nullptr) {
            emit imageToModify->imageClosed();
        }
    });

    ui->replaceImage->setDefaultAction(ui->actionReplaceImage);
    connect(ui->actionReplaceImage, &QAction::triggered, this, [this]() {
        if (!modifiable || imageToModify == nullptr) {
            return;
        }

        auto files = selectImageFiles(this);
        if (!files.empty()) {
            emit imageToModify->imageReplaced(files);
        }
    });

    connect(this, &ImageDropWidget::dropping, [this](bool dropping) {
        ui->dropIndicator->setVisible(dropping);
        ui->widgetStack->setCurrentWidget(dropping ? ui->dropIndicatorPage : ui->viewPage);
    });
    connect(this, &ImageDropWidget::imagesDropped, [this](QStringList const& files) {
        if (modifiable && imageToModify != nullptr) {
            emit imageToModify->imageReplaced(files);
        }
    });

    connect(ui->graphicsView, &InteractiveGraphicsView::zoomChangedExplicitly, this, &ImageView::zoomChangedExplicitly);
    connect(ui->graphicsView, &InteractiveGraphicsView::zoomOutLimitReached, this, &ImageView::zoomOutLimitReached);
    connect(ui->graphicsView, &InteractiveGraphicsView::mouseLeft, &scene, &ImageViewScene::mouseLeft);

    setNotModifiable();
    ui->graphicsView->setMouseTracking(true);
    ui->graphicsView->setScene(&scene);
}

ImageView::~ImageView() = default;

void ImageView::setNotModifiable()
{
    imageToModify = nullptr;
    setModifiable(false);
}

void ImageView::setModifiable(Image* image)
{
    imageToModify = image;
    editMenu->setImage(image);
    setModifiable(true);
}

void ImageView::synchronizeViews(ImageView const& other) const
{
    ui->graphicsView->synchronizeViews(*other.ui->graphicsView);
}

void ImageView::forceViewPropagation() const
{
    ui->graphicsView->forceViewPropagation();
}

void ImageView::rememberView()
{
    ui->graphicsView->rememberView();
}

void ImageView::restoreView()
{
    ui->graphicsView->restoreView();
}

void ImageView::fitViewToScene()
{
    ui->graphicsView->fitSceneInView();
}

void ImageView::setCaption(QString const& caption, QString const& tooltip)
{
    ui->caption->setText(caption);
    if (!tooltip.isEmpty()) {
        ui->caption->setToolTip(tooltip);
    }
}

ImageViewScene& ImageView::getScene()
{
    return scene;
}

ImageViewScene const& ImageView::getScene() const
{
    return scene;
}

QGraphicsView& ImageView::getGraphicsView()
{
    return *ui->graphicsView;
}

void ImageView::clear()
{
    scene.clear();
    scene.setSceneRect({});
}

void ImageView::addPixmap(QPixmap const& image, double opacity)
{
    auto* item = scene.addPixmap(image);
    item->setOpacity(opacity);
    updateSceneRectFromItems();
}

ImageView::DelayUpdates::DelayUpdates(QGraphicsView* _graphicsView)
  : graphicsView(_graphicsView)
{
    graphicsView->setUpdatesEnabled(false);
}

ImageView::DelayUpdates::~DelayUpdates()
{
    restoreUpdates();
}

ImageView::DelayUpdates::DelayUpdates(DelayUpdates&& other) noexcept
{
    restoreUpdates();
    graphicsView = other.graphicsView;
    other.graphicsView = nullptr;
}

ImageView::DelayUpdates& ImageView::DelayUpdates::operator=(DelayUpdates&& other) noexcept
{
    restoreUpdates();
    graphicsView = other.graphicsView;
    other.graphicsView = nullptr;

    return *this;
}

void ImageView::DelayUpdates::restoreUpdates()
{
    if (graphicsView != nullptr) {
        graphicsView->setUpdatesEnabled(true);
        graphicsView = nullptr;
    }
}

ImageView::DelayUpdates ImageView::delayUpdates()
{
    return DelayUpdates(&getGraphicsView());
}

void ImageView::zoomIn()
{
    ui->graphicsView->zoomIn();
}

void ImageView::zoomOut()
{
    ui->graphicsView->zoomOut();
}

void ImageView::updateSceneRectFromItems()
{
    updateSceneRect(scene.itemsBoundingRect());
}

void ImageView::updateSceneRect(QRectF rect)
{
    scene.setSceneRect(rect);
}

void ImageView::setModifiable(bool enable)
{
    modifiable = enable;
    ui->editImage->setVisible(enable);
    ui->replaceImage->setVisible(enable);
    ui->closeImage->setVisible(enable);
    setAcceptDrops(enable);
}
