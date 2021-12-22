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

    void synchronizeViews(ImageView const& other) const;
    void forceViewPropagation() const;
    void rememberView();
    void restoreView();

    void fitViewToScene();

    void setCaption(QString const& caption);

    ImageViewScene& getScene();
    ImageViewScene const& getScene() const;
    QGraphicsView& getGraphicsView();

    void clear();
    void addPixmap(QPixmap const& image, double opacity = 1.);

signals:
    void zoomChangedExplicitly();

public slots:
    void zoomIn();
    void zoomOut();

protected:
    void updateSceneRect();

private:
    void setModifiable(bool enable);

private:
    std::unique_ptr<Ui::ImageView> ui;
    ImageViewScene scene;

    ImageEditMenuAction* editMenu = nullptr;

    bool modifiable = false;
    Image* imageToModify = nullptr;
};
