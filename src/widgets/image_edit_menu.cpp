#include "image_edit_menu.h"

#include "debounce.h"
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

    ui->xOffset->setValue(image->getOffset().x());
    ui->yOffset->setValue(image->getOffset().y());

    auto updateOffset = debounceSlot(
        [image, this]() {
            image->setOffset(getOffset());
        },
        500ms);

    connect(ui->xOffset, QOverload<int>::of(&QSpinBox::valueChanged), updateOffset);
    connect(ui->resetXOffset, &QToolButton::clicked, [this]() {
        ui->xOffset->setValue(0);
    });

    connect(ui->yOffset, QOverload<int>::of(&QSpinBox::valueChanged), updateOffset);
    connect(ui->resetYOffset, &QToolButton::clicked, [this]() {
        ui->yOffset->setValue(0);
    });
}

ImageEditMenu::~ImageEditMenu() = default;

QPoint ImageEditMenu::getOffset() const
{
    return QPoint(ui->xOffset->value(), ui->yOffset->value());
}

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
