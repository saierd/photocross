#include "color_picker.h"

#include <QColorDialog>

ColorPicker::ColorPicker(QWidget* parent)
  : QPushButton(parent)
{
    connect(this, &QPushButton::clicked, this, [this]() {
        QColor newColor = QColorDialog::getColor(color, parentWidget());
        if (newColor != color) {
            setColor(newColor);
        }
    });
}

void ColorPicker::setColor(const QColor& newColor)
{
    color = newColor;
    setStyleSheet("background-color: " + color.name());

    emit colorChanged();
}

const QColor& ColorPicker::getColor()
{
    return color;
}
