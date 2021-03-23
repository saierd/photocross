#pragma once

#include <QWidget>

#include <memory>

namespace Ui {
class EmptyImage;
}

class EmptyImage : public QWidget {
    Q_OBJECT

public:
    explicit EmptyImage(QWidget* parent = nullptr);
    ~EmptyImage() override;

signals:
    void imagesDropped(QStringList const& files);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    std::unique_ptr<Ui::EmptyImage> ui;
};
