#include "image_view.h"

#include <QGraphicsPixmapItem>

#include "ui_image_view.h"

ImageView::ImageView(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::ImageView>();
    ui->setupUi(this);

    ui->fitToView->setDefaultAction(ui->actionFitToView);

    connect(ui->actionFitToView, &QAction::triggered, this, &ImageView::fitViewToScene);

    ui->graphicsView->setScene(&scene);
}

ImageView::~ImageView() = default;

void ImageView::synchronizeViews(ImageView const& other) const
{
    ui->graphicsView->synchronizeViews(*other.ui->graphicsView);
}

void ImageView::fitViewToScene()
{
    ui->graphicsView->fitSceneInView();
}

void ImageView::setCaption(QString const& caption)
{
    ui->caption->setText(caption);
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
