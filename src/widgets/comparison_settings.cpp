#include "comparison_settings.h"

#include <chrono>
#include <cmath>

#include "ui_comparison_settings.h"

using namespace std::chrono_literals;

QColor const blendImage1Color(255, 0, 0);
QColor const blendImage2Color(0, 0, 255);

auto const minimumBlendInterval = 200ms;
auto const maximumBlendInterval = 1500ms;
auto const continuousBlendTimerInterval = 50ms;

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

ComparisonSettings::ComparisonSettings(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::ComparisonSettings>();
    ui->setupUi(this);
    originalBlendLabelPalette = ui->blendLeftLabel->palette();

    ui->comparisonMode->setCurrentIndex(0);
    connect(ui->comparisonMode, &QTabWidget::currentChanged, this, &ComparisonSettings::settingsChanged);
    connect(ui->comparisonMode, &QTabWidget::currentChanged, this, &ComparisonSettings::updateBlendTimerSettings);

    connect(ui->differenceToleranceEnabled, &QGroupBox::toggled, [this]() {
        ui->differenceToleranceLabel->setNum(getDifferenceTolerance());
        emit(settingsChanged());
    });
    connect(ui->differenceTolerance, &QSlider::valueChanged, [this]() {
        ui->differenceToleranceLabel->setNum(getDifferenceTolerance());
        emit(settingsChanged());
    });
    connect(ui->showMinorDifferences, &QCheckBox::toggled, this, &ComparisonSettings::settingsChanged);

    connect(ui->resetBlendPosition, &QPushButton::clicked, this, &ComparisonSettings::resetBlendPosition);
    connect(ui->blendPosition, &QSlider::valueChanged, this, &ComparisonSettings::settingsChanged);
    connect(ui->blendTrueColors, &QCheckBox::toggled, [this]() {
        updateBlendLabelColors();
        emit(settingsChanged());
    });
    updateBlendLabelColors();

    connect(ui->resetBlendSpeed, &QPushButton::clicked, this, &ComparisonSettings::resetBlendSpeed);
    connect(ui->blendSwitchAutomatically, &QGroupBox::toggled, this, &ComparisonSettings::updateBlendTimerSettings);
    connect(ui->blendSwitchSpeed, &QSlider::valueChanged, this, &ComparisonSettings::updateBlendTimerSettings);
    connect(ui->blendSwitchContinuous, &QCheckBox::toggled, this, &ComparisonSettings::updateBlendTimerSettings);
    updateBlendTimerSettings();

    connect(&blendTimer, &QTimer::timeout, this, &ComparisonSettings::onBlendTimerTimeout);
}

ComparisonSettings::~ComparisonSettings() = default;

ComparisonMode ComparisonSettings::getComparisonMode() const
{
    if (ui->comparisonMode->currentWidget() == ui->comparisonModeHighlightDifferences) {
        return ComparisonMode::HighlightDifferences;
    }
    return ComparisonMode::BlendImages;
}

int ComparisonSettings::getDifferenceTolerance() const
{
    if (!ui->differenceToleranceEnabled->isChecked()) {
        return 0;
    }
    return ui->differenceTolerance->value();
}

bool ComparisonSettings::showMinorDifferences() const
{
    return ui->showMinorDifferences->isChecked();
}

double ComparisonSettings::getBlendPosition() const
{
    return relativeSliderPosition(*ui->blendPosition);
}

void ComparisonSettings::setBlendPosition(double position)
{
    setRelativeSliderPosition(*ui->blendPosition, position);
}

QColor ComparisonSettings::blendImage1Color() const
{
    if (ui->blendTrueColors->isChecked()) {
        return QColor();
    }
    return ::blendImage1Color;
}

QColor ComparisonSettings::blendImage2Color() const
{
    if (ui->blendTrueColors->isChecked()) {
        return QColor();
    }
    return ::blendImage2Color;
}

void ComparisonSettings::resetBlendPosition()
{
    setBlendPosition(0.5);
}

void ComparisonSettings::updateBlendLabelColors()
{
    QPalette leftPalette = originalBlendLabelPalette;
    QPalette rightPalette = originalBlendLabelPalette;

    if (!ui->blendTrueColors->isChecked()) {
        leftPalette.setColor(ui->blendLeftLabel->foregroundRole(), ::blendImage1Color);
        rightPalette.setColor(ui->blendRightLabel->foregroundRole(), ::blendImage2Color);
    }

    ui->blendLeftLabel->setPalette(leftPalette);
    ui->blendRightLabel->setPalette(rightPalette);
}

void ComparisonSettings::resetBlendSpeed()
{
    setRelativeSliderPosition(*ui->blendSwitchSpeed, 0.5);
}

void ComparisonSettings::updateBlendTimerSettings()
{
    if (getComparisonMode() != ComparisonMode::BlendImages) {
        blendTimer.stop();
        return;
    }

    bool enableTimer = ui->blendSwitchAutomatically->isChecked();
    ui->blendPosition->setDisabled(enableTimer);
    ui->resetBlendPosition->setDisabled(enableTimer);

    if (!enableTimer) {
        if (blendTimer.isActive()) {
            blendTimer.stop();
            setBlendPosition(blendPositionBeforeAutomaticMode);
        }
        return;
    }

    bool continuousBlending = ui->blendSwitchContinuous->isChecked();

    if (continuousBlending) {
        blendTimer.setInterval(continuousBlendTimerInterval);
    } else {
        blendTimer.setInterval(currentBlendIntervalBetweenImages());
    }

    if (!blendTimer.isActive()) {
        // Remember the current blending position. It will be restored when automatic mode gets deactivated.
        blendPositionBeforeAutomaticMode = getBlendPosition();

        // Always start at the same position in the cycle when activating automatic mode.
        if (continuousBlending) {
            setBlendPosition(0.5);
        } else {
            setBlendPosition(0);
        }
        currentBlendDirection = 1;

        blendTimer.start();
    }

    if (!continuousBlending) {
        // When switching from continuous to non-continuous blending, round the position to the nearest image
        // immediately.
        setBlendPosition(std::round(getBlendPosition() - currentBlendDirection));
    }
}

void ComparisonSettings::onBlendTimerTimeout()
{
    double blendPositionIncrement = 1;
    if (ui->blendSwitchContinuous->isChecked()) {
        auto interval = currentBlendIntervalBetweenImages();
        blendPositionIncrement = (1. * continuousBlendTimerInterval) / interval;
    }

    double const currentBlendPosition = getBlendPosition();
    double newBlendPosition = currentBlendPosition + currentBlendDirection * blendPositionIncrement;
    if (newBlendPosition <= 0.) {
        newBlendPosition = 0.;
        currentBlendDirection = 1;
    } else if (newBlendPosition >= 1.) {
        newBlendPosition = 1.;
        currentBlendDirection = -1;
    }

    setBlendPosition(newBlendPosition);
}

std::chrono::milliseconds ComparisonSettings::currentBlendIntervalBetweenImages() const
{
    double relativeInterval = 1 - relativeSliderPosition(*ui->blendSwitchSpeed);
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        minimumBlendInterval + relativeInterval * (maximumBlendInterval - minimumBlendInterval));
}
