#include "image_view.h"

#include <QGraphicsPixmapItem>

#include "ui_image_view.h"

ImageView::ImageView(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::ImageView>();
    ui->setupUi(this);

    ui->graphicsView->setScene(&scene);
}

ImageView::~ImageView() = default;

void ImageView::synchronizeViews(ImageView const& other) const
{
    ui->graphicsView->synchronizeViews(*other.ui->graphicsView);
}

void ImageView::fitViewToScene()
{
    ui->graphicsView->fitInView(scene.sceneRect(), Qt::KeepAspectRatio);
}

void ImageView::addPixmap(QPixmap const& image, double opacity)
{
    auto* item = scene.addPixmap(image);
    item->setOpacity(opacity);
}
