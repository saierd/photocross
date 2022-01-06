#include "slider_helpers.h"

#include <QSlider>

#include <cmath>

double relativeSliderPosition(QSlider const& slider)
{
    int min = slider.minimum();
    int max = slider.maximum();
    int value = slider.value();

    return static_cast<double>(value - min) / (max - min);
}

void setRelativeSliderPosition(QSlider& slider, double position)
{
    int min = slider.minimum();
    int max = slider.maximum();
    int value = static_cast<int>(std::round(position * (max - min) + min));

    slider.setValue(value);
}
