#pragma once

#include "image_drop_widget.h"

#include <QGraphicsScene>
#include <QWidget>

#include <memory>

namespace Ui {
class ImageView;
}

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

    QGraphicsScene& getScene() &;

    void clear();
    void addPixmap(QPixmap const& image, double opacity = 1.);

signals:
    void imageClosed();
    void imageReplaced(QStringList const& files);

    void zoomChangedExplicitly();

public slots:
    void zoomIn();
    void zoomOut();

private:
    std::unique_ptr<Ui::ImageView> ui;
    QGraphicsScene scene;

    bool modifiable = false;
};
