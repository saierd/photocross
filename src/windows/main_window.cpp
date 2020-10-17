#include "main_window.h"

#include "image.h"
#include "image_diff.h"

#include "ui_main_window.h"

MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
    ui = std::make_unique<Ui::MainWindow>();
    ui->setupUi(this);

    ui->leftView->synchronizeViews(*ui->rightView);
    ui->leftView->synchronizeViews(*ui->comparisonView);
    ui->rightView->synchronizeViews(*ui->comparisonView);

    Image image1(QImage("test_data/1.png"));
    Image image2(QImage("test_data/2.png"));

    ui->leftView->addPixmap(image1.toPixmap());
    ui->rightView->addPixmap(image2.toPixmap());

    ui->comparisonView->addPixmap(image1.toGrayscalePixmap());
    ui->comparisonView->addPixmap(QPixmap::fromImage(imageDiff(image1.image(), image2.image())), 0.7);

    ui->leftView->fitViewToScene();
    ui->rightView->fitViewToScene();
    ui->comparisonView->fitViewToScene();
}

MainWindow::~MainWindow() = default;
