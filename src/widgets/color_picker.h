#pragma once

#include <QColor>
#include <QPushButton>

class ColorPicker : public QPushButton {
    Q_OBJECT

public:
    explicit ColorPicker(QWidget* parent = nullptr);

    void setColor(QColor const& newColor);
    QColor const& getColor();

signals:
    void colorChanged();

private:
    QColor color;
};
