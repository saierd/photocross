#include "image_view.h"

#include <QGraphicsPixmapItem>

#include "ui_image_view.h"

ImageView::ImageView(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::ImageView>();
    ui->setupUi(this);

    ui->fitToView->setDefaultAction(ui->actionFitToView);
    connect(ui->actionFitToView, &QAction::triggered, [this]() {
        fitViewToScene();
        emit zoomChangedExplicitly();
    });

    connect(ui->graphicsView, &InteractiveGraphicsView::zoomChangedExplicitly, this, &ImageView::zoomChangedExplicitly);

    ui->graphicsView->setScene(&scene);
}

ImageView::~ImageView() = default;

void ImageView::synchronizeViews(ImageView const& other) const
{
    ui->graphicsView->synchronizeViews(*other.ui->graphicsView);
}

void ImageView::forceViewPropagation() const
{
    ui->graphicsView->forceViewPropagation();
}

void ImageView::fitViewToScene()
{
    ui->graphicsView->fitSceneInView();
}

void ImageView::setCaption(QString const& caption)
{
    ui->caption->setText(caption);
}

QGraphicsScene& ImageView::getScene() &
{
    return scene;
}

void ImageView::clear()
{
    scene.clear();
}

void ImageView::addPixmap(QPixmap const& image, double opacity)
{
    auto* item = scene.addPixmap(image);
    item->setOpacity(opacity);
}

void ImageView::zoomIn()
{
    ui->graphicsView->zoomIn();
}

void ImageView::zoomOut()
{
    ui->graphicsView->zoomOut();
}
