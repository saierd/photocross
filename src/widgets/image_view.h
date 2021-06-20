#pragma once

#include "image_drop_widget.h"
#include "image_view_scene.h"

#include <QWidget>

#include <memory>

namespace Ui {
class ImageView;
}

class QGraphicsView;

class ImageView : public ImageDropWidget {
    Q_OBJECT

public:
    explicit ImageView(QWidget* parent = nullptr);
    ~ImageView() override;

    void setModifiable(bool enable = true);

    void synchronizeViews(ImageView const& other) const;
    void forceViewPropagation() const;

    void fitViewToScene();

    void setCaption(QString const& caption);

    ImageViewScene& getScene() &;
    QGraphicsView& getGraphicsView() &;

    void clear();
    void addPixmap(QPixmap const& image, double opacity = 1.);

signals:
    void resetRotation();
    void rotateLeft();
    void rotateRight();

    void imageClosed();
    void imageReplaced(QStringList const& files);

    void zoomChangedExplicitly();

public slots:
    void zoomIn();
    void zoomOut();

protected:
    void updateSceneRect();

private:
    std::unique_ptr<Ui::ImageView> ui;
    ImageViewScene scene;

    bool modifiable = false;
};
