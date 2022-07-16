#include "image_edit_menu.h"

#include "image.h"
#include "session.h"
#include "utility/debounce.h"

#include "ui_image_edit_menu.h"

ImageEditMenu::ImageEditMenu(Image* image,
                             bool isPickingOffsetFromLocations,
                             bool canStartPickingOffsetFromLocations,
                             QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::ImageEditMenu>();
    ui->setupUi(this);

    ui->resetRotationButton->setDefaultAction(ui->actionResetRotation);
    connect(ui->actionResetRotation, &QAction::triggered, this, [image]() {
        if (image != nullptr) {
            image->resetRotation();
        }
    });

    ui->rotateLeftButton->setDefaultAction(ui->actionRotateLeft);
    connect(ui->actionRotateLeft, &QAction::triggered, this, [image]() {
        if (image != nullptr) {
            image->rotateLeft();
        }
    });

    ui->rotateRightButton->setDefaultAction(ui->actionRotateRight);
    connect(ui->actionRotateRight, &QAction::triggered, this, [image]() {
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
        300ms);

    connect(ui->xOffset, QOverload<int>::of(&QSpinBox::valueChanged), updateOffset);
    connect(ui->resetXOffset, &QToolButton::clicked, this, [this]() {
        ui->xOffset->setValue(0);
    });

    connect(ui->yOffset, QOverload<int>::of(&QSpinBox::valueChanged), updateOffset);
    connect(ui->resetYOffset, &QToolButton::clicked, this, [this]() {
        ui->yOffset->setValue(0);
    });

    connect(ui->actionPickOffsetFromLocations, &QAction::triggered, this, &ImageEditMenu::startPickOffsetFromLocations);
    connect(ui->actionCancelPickOffsetFromLocations,
            &QAction::triggered,
            this,
            &ImageEditMenu::cancelPickOffsetFromLocations);

    ui->actionPickOffsetFromLocations->setEnabled(canStartPickingOffsetFromLocations);
    if (isPickingOffsetFromLocations) {
        ui->pickOffsetFromLocationsButton->setDefaultAction(ui->actionCancelPickOffsetFromLocations);
    } else {
        ui->pickOffsetFromLocationsButton->setDefaultAction(ui->actionPickOffsetFromLocations);
    }
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
    session = image->getSession();

    disconnect(imagePositionSelectedConnection);
    if (session != nullptr) {
        imagePositionSelectedConnection =
            connect(session, &Session::imagePositionSelected, this, &ImageEditMenuAction::imagePositionSelected);
    }
}

bool ImageEditMenuAction::canStartPickingOffsetFromLocations() const
{
    return session != nullptr && !image->getSession()->positionSelectionModeEnabled();
}

bool ImageEditMenuAction::isPickingOffsetFromLocations() const
{
    if (!pickOffsetFromLocations.enabled) {
        return false;
    }

    if (session == nullptr) {
        return false;
    }

    if (!image->getSession()->positionSelectionModeEnabled()) {
        // Position selection mode was canceled externally, this also cancels our offset picking action.
        return false;
    }

    return true;
}

QWidget* ImageEditMenuAction::createWidget(QWidget* parent)
{
    auto widget = std::make_unique<ImageEditMenu>(image,
                                                  isPickingOffsetFromLocations(),
                                                  canStartPickingOffsetFromLocations(),
                                                  parent);
    connect(widget.get(),
            &ImageEditMenu::startPickOffsetFromLocations,
            this,
            &ImageEditMenuAction::startPickOffsetFromLocations);
    connect(widget.get(),
            &ImageEditMenu::cancelPickOffsetFromLocations,
            this,
            &ImageEditMenuAction::cancelPickOffsetFromLocationsAndHideMenu);

    lastCreatedMenuWidget = widget.get();

    return widget.release();
}

void ImageEditMenuAction::startPickOffsetFromLocations()
{
    if (!canStartPickingOffsetFromLocations()) {
        return;
    }

    pickOffsetFromLocations = {};
    pickOffsetFromLocations.enabled = true;

    session->enablePositionSelectionMode();

    // Close the menu. Note that the menu widget and its parent must be valid here, since we just got a signal from
    // them to run this function.
    if (lastCreatedMenuWidget != nullptr) {
        lastCreatedMenuWidget->parentWidget()->close();
    }
}

void ImageEditMenuAction::cancelPickOffsetFromLocations()
{
    pickOffsetFromLocations = {};

    if (session != nullptr) {
        session->disablePositionSelectionMode();
    }
}

void ImageEditMenuAction::cancelPickOffsetFromLocationsAndHideMenu()
{
    cancelPickOffsetFromLocations();

    if (lastCreatedMenuWidget != nullptr) {
        lastCreatedMenuWidget->parentWidget()->close();
    }
}

void ImageEditMenuAction::imagePositionSelected(Image* selectedImage, QPoint const& selectedPosition)
{
    if (!pickOffsetFromLocations.enabled) {
        return;
    }

    if (selectedImage == image) {
        pickOffsetFromLocations.localPoint = selectedPosition;
    } else {
        pickOffsetFromLocations.remotePoint = selectedPosition;
    }

    if (pickOffsetFromLocations.localPoint && pickOffsetFromLocations.remotePoint) {
        // Two points selected. Apply the offset between the selected points and exit the location picking mode.
        QPoint offset = *pickOffsetFromLocations.remotePoint - *pickOffsetFromLocations.localPoint;
        image->setOffset(image->getOffset() + offset);

        cancelPickOffsetFromLocations();
    }
}
