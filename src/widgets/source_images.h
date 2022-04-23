#pragma once

#include "empty_image.h"

#include <QBoxLayout>
#include <QWidget>

class Session;

class SourceImages : public QWidget {
    Q_OBJECT

public:
    explicit SourceImages(QWidget* parent = nullptr);

    void setSession(Session* session);
    void setDirection(QBoxLayout::Direction direction);

    int numImageWidgets() const;
    void addImageWidget(QWidget* widget);
    void popImageWidget();

protected:
    bool eventFilter(QObject* object, QEvent* event) override;

    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dragMoveEvent(QDragMoveEvent* event) override;
    void dropEvent(QDropEvent* event) override;

private:
    void updateEmptyImage();

    ImageDropWidget* hoveredWidget(QDragMoveEvent* event);
    void forwardDragMoveEvent(ImageDropWidget* hoveredChild, QDragMoveEvent* event);
    void leaveLastDropWidget();
    void finishDropEvent();

private:
    QBoxLayout layout;
    EmptyImage emptyImage;

    Session* session = nullptr;

    bool dropping = false;
    ImageDropWidget* lastDropWidget = nullptr;
};
