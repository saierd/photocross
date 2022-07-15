#pragma once

#include <QWidgetAction>

#include <memory>

namespace Ui {
class ImageEditMenu;
}

class Image;

class ImageEditMenu : public QWidget {
    Q_OBJECT

public:
    explicit ImageEditMenu(Image* image, QWidget* parent = nullptr);
    ~ImageEditMenu() override;

    QPoint getOffset() const;

private:
    std::unique_ptr<Ui::ImageEditMenu> ui;
};

class ImageEditMenuAction : public QWidgetAction {
    Q_OBJECT

public:
    explicit ImageEditMenuAction(QWidget* parent = nullptr);

    void setImage(Image* _image);

protected:
    QWidget* createWidget(QWidget* parent) override;

private:
    Image* image;
};
