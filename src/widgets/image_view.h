#pragma once

#include <QGraphicsScene>
#include <QWidget>

#include <memory>

namespace Ui {
class ImageView;
}

class ImageView : public QWidget {
    Q_OBJECT

public:
    explicit ImageView(QWidget* parent = nullptr);
    ~ImageView() override;

    void synchronizeViews(ImageView const& other) const;

    void fitViewToScene();

    void addPixmap(QPixmap const& image, double opacity = 1.);

private:
    std::unique_ptr<Ui::ImageView> ui;
    QGraphicsScene scene;
};
