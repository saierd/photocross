#include "image_view.h"

#include "file_helpers.h"
#include "image.h"
#include "image_edit_menu.h"

#include <QGraphicsPixmapItem>

#include "ui_image_view.h"

ImageView::ImageView(QWidget* parent)
  : ImageDropWidget(parent)
{
    ui = std::make_unique<Ui::ImageView>();
    ui->setupUi(this);

    ui->fitToView->setDefaultAction(ui->actionFitToView);
    connect(ui->actionFitToView, &QAction::triggered, [this]() {
        fitViewToScene();
        emit zoomChangedExplicitly();
    });

    editMenu = new ImageEditMenuAction(this);
    ui->editImage->addAction(editMenu);

    ui->closeImage->setDefaultAction(ui->actionCloseImage);
    connect(ui->actionCloseImage, &QAction::triggered, [this]() {
        if (modifiable && imageToModify != nullptr) {
            emit imageToModify->imageClosed();
        }
    });

    ui->replaceImage->setDefaultAction(ui->actionReplaceImage);
    connect(ui->actionReplaceImage, &QAction::triggered, [this]() {
        if (!modifiable || imageToModify == nullptr) {
            return;
        }

        auto files = selectImageFiles(this);
        if (!files.empty()) {
            emit imageToModify->imageReplaced(files);
        }
    });

    connect(this, &ImageDropWidget::imagesDropped, [this](QStringList const& files) {
        if (modifiable && imageToModify != nullptr) {
            emit imageToModify->imageReplaced(files);
        }
    });

    connect(ui->graphicsView, &InteractiveGraphicsView::zoomChangedExplicitly, this, &ImageView::zoomChangedExplicitly);
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

void ImageView::setCaption(QString const& caption)
{
    ui->caption->setText(caption);
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
    updateSceneRect();
}

void ImageView::zoomIn()
{
    ui->graphicsView->zoomIn();
}

void ImageView::zoomOut()
{
    ui->graphicsView->zoomOut();
}

void ImageView::updateSceneRect()
{
    scene.setSceneRect(scene.itemsBoundingRect());
}

void ImageView::setModifiable(bool enable)
{
    modifiable = enable;
    ui->editImage->setVisible(enable);
    ui->replaceImage->setVisible(enable);
    ui->closeImage->setVisible(enable);
    setAcceptDrops(enable);
}
