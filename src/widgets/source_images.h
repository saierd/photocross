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

private:
    void numWidgetsChanged();

private:
    QBoxLayout layout;
    EmptyImage emptyImage;

    Session* session = nullptr;
};
