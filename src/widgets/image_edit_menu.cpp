#include "image_edit_menu.h"

#include "ui_image_edit_menu.h"

ImageEditMenu::ImageEditMenu(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::ImageEditMenu>();
    ui->setupUi(this);

    ui->resetRotationButton->setDefaultAction(ui->actionResetRotation);
    connect(ui->actionResetRotation, &QAction::triggered, this, &ImageEditMenu::resetRotation);

    ui->rotateLeftButton->setDefaultAction(ui->actionRotateLeft);
    connect(ui->actionRotateLeft, &QAction::triggered, this, &ImageEditMenu::rotateLeft);

    ui->rotateRightButton->setDefaultAction(ui->actionRotateRight);
    connect(ui->actionRotateRight, &QAction::triggered, this, &ImageEditMenu::rotateRight);
}

ImageEditMenu::~ImageEditMenu() = default;

ImageEditMenuAction::ImageEditMenuAction(QWidget* parent)
  : QWidgetAction(parent)
{
}

QWidget* ImageEditMenuAction::createWidget(QWidget* parent)
{
    auto widget = std::make_unique<ImageEditMenu>(parent);
    connect(widget.get(), &ImageEditMenu::resetRotation, this, &ImageEditMenuAction::resetRotation);
    connect(widget.get(), &ImageEditMenu::rotateLeft, this, &ImageEditMenuAction::rotateLeft);
    connect(widget.get(), &ImageEditMenu::rotateRight, this, &ImageEditMenuAction::rotateRight);

    return widget.release();
}
