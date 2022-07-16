#pragma once

#include <QWidgetAction>

#include <memory>
#include <optional>

namespace Ui {
class ImageEditMenu;
}

class Image;
class Session;

class ImageEditMenu : public QWidget {
    Q_OBJECT

public:
    ImageEditMenu(Image* image,
                  bool isPickingOffsetFromLocations,
                  bool canStartPickingOffsetFromLocations,
                  QWidget* parent = nullptr);
    ~ImageEditMenu() override;

    QPoint getOffset() const;

signals:
    void startPickOffsetFromLocations();
    void cancelPickOffsetFromLocations();

private:
    std::unique_ptr<Ui::ImageEditMenu> ui;
};

class ImageEditMenuAction : public QWidgetAction {
    Q_OBJECT

public:
    explicit ImageEditMenuAction(QWidget* parent = nullptr);

    void setImage(Image* _image);

    bool canStartPickingOffsetFromLocations() const;
    bool isPickingOffsetFromLocations() const;

protected:
    QWidget* createWidget(QWidget* parent) override;

private slots:
    void startPickOffsetFromLocations();
    void cancelPickOffsetFromLocations();
    void cancelPickOffsetFromLocationsAndHideMenu();
    void imagePositionSelected(Image* selectedImage, QPoint const& selectedPosition);

private:
    Image* image = nullptr;
    Session* session = nullptr;

    ImageEditMenu* lastCreatedMenuWidget = nullptr;

    // State for the location picking action. This has to live on the action class, because it works across multiple
    // invocations of the menu (which creates a new widget each time).
    struct {
        bool enabled = false;
        std::optional<QPoint> localPoint;
        std::optional<QPoint> remotePoint;
    } pickOffsetFromLocations;

    QMetaObject::Connection imagePositionSelectedConnection;
};
