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
    void forceViewPropagation() const;

    void fitViewToScene();

    void setCaption(QString const& caption);

    void clear();
    void addPixmap(QPixmap const& image, double opacity = 1.);

signals:
    void zoomChangedExplicitly();

public slots:
    void zoomIn();
    void zoomOut();

private:
    std::unique_ptr<Ui::ImageView> ui;
    QGraphicsScene scene;
};
