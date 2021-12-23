#pragma once

#include <QScrollArea>

// Scroll area that can only scroll in one direction. In the other one, it adapts the size hint of its child widget and
// thus should never need scrolling.
class SingleDirectionScrollArea : public QScrollArea {
    Q_OBJECT

public:
    explicit SingleDirectionScrollArea(QWidget* parent = nullptr);

    void setOrientation(Qt::Orientation _orientation);

    bool eventFilter(QObject* object, QEvent* event) override;

private:
    void updateMinimumSize();

    Qt::Orientation orientation = Qt::Horizontal;
};
