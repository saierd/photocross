#include "image_edit_menu.h"

#include "image.h"

#include "ui_image_edit_menu.h"

ImageEditMenu::ImageEditMenu(Image* image, QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::ImageEditMenu>();
    ui->setupUi(this);

    ui->resetRotationButton->setDefaultAction(ui->actionResetRotation);
    connect(ui->actionResetRotation, &QAction::triggered, [image]() {
        if (image != nullptr) {
            image->resetRotation();
        }
    });

    ui->rotateLeftButton->setDefaultAction(ui->actionRotateLeft);
    connect(ui->actionRotateLeft, &QAction::triggered, [image]() {
        if (image != nullptr) {
            image->rotateLeft();
        }
    });

    ui->rotateRightButton->setDefaultAction(ui->actionRotateRight);
    connect(ui->actionRotateRight, &QAction::triggered, [image]() {
        if (image != nullptr) {
            image->rotateRight();
        }
    });
}

ImageEditMenu::~ImageEditMenu() = default;

ImageEditMenuAction::ImageEditMenuAction(QWidget* parent)
  : QWidgetAction(parent)
{
}

void ImageEditMenuAction::setImage(Image* _image)
{
    image = _image;
}

QWidget* ImageEditMenuAction::createWidget(QWidget* parent)
{
    auto widget = std::make_unique<ImageEditMenu>(image, parent);
    return widget.release();
}
