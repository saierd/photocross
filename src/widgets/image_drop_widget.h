#pragma once

#include <QWidget>

class ImageDropWidget : public QWidget {
    Q_OBJECT

public:
    explicit ImageDropWidget(QWidget* parent = nullptr);

signals:
    void dropping(bool dropping);
    void imagesDropped(QStringList const& files);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};
