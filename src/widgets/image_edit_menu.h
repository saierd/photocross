#pragma once

#include <QWidgetAction>

#include <memory>

namespace Ui {
class ImageEditMenu;
}

class ImageEditMenu : public QWidget {
    Q_OBJECT

public:
    explicit ImageEditMenu(QWidget* parent = nullptr);
    ~ImageEditMenu() override;

signals:
    void resetRotation();
    void rotateLeft();
    void rotateRight();

private:
    std::unique_ptr<Ui::ImageEditMenu> ui;
};

class ImageEditMenuAction : public QWidgetAction {
    Q_OBJECT

public:
    explicit ImageEditMenuAction(QWidget* parent = nullptr);

signals:
    void resetRotation();
    void rotateLeft();
    void rotateRight();

protected:
    QWidget* createWidget(QWidget* parent) override;
};
