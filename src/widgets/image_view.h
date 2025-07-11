#pragma once

#include "image_drop_widget.h"
#include "image_view_scene.h"

#include <QWidget>

#include <memory>

namespace Ui {
class ImageView;
}

class QGraphicsView;
class Image;
class ImageEditMenuAction;

class ImageView : public ImageDropWidget {
    Q_OBJECT

public:
    explicit ImageView(QWidget* parent = nullptr);
    ~ImageView() override;

    void setNotModifiable();
    void setModifiable(Image* image);

    bool canSaveImage() const;

    void synchronizeViews(ImageView const& other) const;
    void forceViewPropagation() const;
    void rememberView();
    void restoreView();

    void fitViewToScene();

    void setCaption(QString const& caption, QString const& tooltip = "");

    ImageViewScene& getScene();
    ImageViewScene const& getScene() const;
    QGraphicsView& getGraphicsView();

    void clear();
    void addPixmap(QPixmap const& image, double opacity = 1.);

    class DelayUpdates {
    public:
        explicit DelayUpdates(QGraphicsView* _graphicsView = nullptr);
        ~DelayUpdates();

        DelayUpdates(DelayUpdates&& other) noexcept;
        DelayUpdates& operator=(DelayUpdates&& other) noexcept;

        DelayUpdates(DelayUpdates const&) = delete;
        DelayUpdates& operator=(DelayUpdates const&) = delete;

        void restoreUpdates();

    private:
        QGraphicsView* graphicsView = nullptr;
    };

    DelayUpdates delayUpdates();

signals:
    void zoomChangedExplicitly();
    void zoomOutLimitReached();

    void canSaveImageChanged(bool enabled);

public slots:
    void zoomIn();
    void zoomOut();

    void setCanSaveImage(bool enabled);
    void saveImage();

protected:
    void updateSceneRectFromItems();
    void updateSceneRect(QRectF rect);

private:
    void setModifiable(bool enable);

private:
    std::unique_ptr<Ui::ImageView> ui;
    ImageViewScene scene;

    ImageEditMenuAction* editMenu = nullptr;

    bool modifiable = false;
    Image* imageToModify = nullptr;

    QMetaObject::Connection positionSelectionModeConnection;
    QMetaObject::Connection positionSelectionConnection;
};
