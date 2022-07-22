#include "comparison_settings_widget.h"

#include "utility/slider.h"
#include "utility/time.h"

#include "ui_comparison_settings_widget.h"

namespace {

void showShortcutInTooltip(QAbstractButton* button)
{
    button->setToolTip(button->text() + " (" + button->shortcut().toString() + ")");
}

}  // namespace

ComparisonSettingsWidget::ComparisonSettingsWidget(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::ComparisonSettings>();
    ui->setupUi(this);
    originalBlendLabelPalette = ui->blendLeftLabel->palette();

    reset();
    updateMode();

    connect(ui->reset, &QToolButton::clicked, this, &ComparisonSettingsWidget::reset);

    showShortcutInTooltip(ui->modeHighlightDifferences);
    showShortcutInTooltip(ui->modeBlendImagesFalseColors);
    showShortcutInTooltip(ui->modeBlendImagesTrueColors);
    showShortcutInTooltip(ui->modeBlendImagesAnimated);

    auto modeChangedIfToggledOn = [this](bool checked) {
        if (checked) {
            emit modeChanged();
        }
    };

    connect(ui->modeHighlightDifferences, &QToolButton::toggled, this, modeChangedIfToggledOn);
    connect(ui->modeBlendImagesFalseColors, &QToolButton::toggled, this, modeChangedIfToggledOn);
    connect(ui->modeBlendImagesTrueColors, &QToolButton::toggled, this, modeChangedIfToggledOn);
    connect(ui->modeBlendImagesAnimated, &QToolButton::toggled, this, modeChangedIfToggledOn);

    connect(this, &ComparisonSettingsWidget::modeChanged, this, &ComparisonSettingsWidget::updateMode);
    connect(this, &ComparisonSettingsWidget::modeChanged, this, &ComparisonSettingsWidget::settingsChanged);

    connect(ui->differenceToleranceEnabled, &QGroupBox::toggled, this, &ComparisonSettingsWidget::settingsChanged);
    connect(ui->differenceTolerance, &QSlider::valueChanged, this, [this]() {
        ui->differenceToleranceLabel->setNum(getComparisonSettings().highlightDifferences.tolerance);
        emit settingsChanged();
    });
    connect(ui->showMinorDifferences, &QCheckBox::toggled, this, &ComparisonSettingsWidget::settingsChanged);

    connect(ui->resetBlendPosition, &QToolButton::clicked, this, &ComparisonSettingsWidget::resetBlendPosition);
    connect(ui->blendPosition, &QSlider::valueChanged, this, &ComparisonSettingsWidget::settingsChanged);
    connect(ui->resetLeftBlendColor, &QToolButton::clicked, this, &ComparisonSettingsWidget::resetLeftBlendColor);
    connect(ui->leftBlendColor, &ColorPicker::colorChanged, this, &ComparisonSettingsWidget::updateBlendLabelColors);
    connect(ui->leftBlendColor, &ColorPicker::colorChanged, this, &ComparisonSettingsWidget::settingsChanged);
    connect(ui->resetRightBlendColor, &QToolButton::clicked, this, &ComparisonSettingsWidget::resetRightBlendColor);
    connect(ui->rightBlendColor, &ColorPicker::colorChanged, this, &ComparisonSettingsWidget::updateBlendLabelColors);
    connect(ui->rightBlendColor, &ColorPicker::colorChanged, this, &ComparisonSettingsWidget::settingsChanged);

    connect(ui->resetAnimationSpeed, &QToolButton::clicked, this, &ComparisonSettingsWidget::resetAnimationSpeed);
    connect(ui->animationSpeed, &QSlider::valueChanged, this, &ComparisonSettingsWidget::settingsChanged);
    connect(ui->animationContinuous, &QCheckBox::toggled, this, &ComparisonSettingsWidget::settingsChanged);
}

ComparisonSettingsWidget::~ComparisonSettingsWidget() = default;

void ComparisonSettingsWidget::setNumberOfImages(size_t numberOfImages)
{
    bool showBlendingModes = (numberOfImages <= 2);
    ui->modeBlendImagesFalseColors->setVisible(showBlendingModes);
    ui->modeBlendImagesTrueColors->setVisible(showBlendingModes);

    if (!showBlendingModes) {
        if (getComparisonMode() == ComparisonMode::BlendImagesFalseColors ||
            getComparisonMode() == ComparisonMode::BlendImagesTrueColors) {
            ui->modeHighlightDifferences->setChecked(true);
        }
    }
}

ComparisonMode ComparisonSettingsWidget::getComparisonMode() const
{
    if (ui->modeHighlightDifferences->isChecked()) {
        return ComparisonMode::HighlightDifferences;
    }
    if (ui->modeBlendImagesFalseColors->isChecked()) {
        return ComparisonMode::BlendImagesFalseColors;
    }
    if (ui->modeBlendImagesTrueColors->isChecked()) {
        return ComparisonMode::BlendImagesTrueColors;
    }
    if (ui->modeBlendImagesAnimated->isChecked()) {
        return ComparisonMode::BlendImagesAnimated;
    }

    assert(false);
    return ComparisonMode::HighlightDifferences;
}

ComparisonSettings ComparisonSettingsWidget::getComparisonSettings() const
{
    ComparisonSettings result;
    result.mode = getComparisonMode();

    result.highlightDifferences.tolerance = ui->differenceTolerance->value();
    result.highlightDifferences.applyTolerance = ui->differenceToleranceEnabled->isChecked();
    result.highlightDifferences.showMinorDifferences = ui->showMinorDifferences->isChecked();

    result.blendPosition = getBlendPosition();
    result.blendFalseColors.leftColor = ui->leftBlendColor->getColor();
    result.blendFalseColors.rightColor = ui->rightBlendColor->getColor();

    result.animatedBlending.timeBetweenImages = getAnimationTimeBetweenImages();
    result.animatedBlending.continuous = ui->animationContinuous->isChecked();

    return result;
}

void ComparisonSettingsWidget::setComparisonSettings(ComparisonSettings const& settings)
{
    QSignalBlocker blockSignals(this);

    switch (settings.mode) {
        case ComparisonMode::HighlightDifferences:
            ui->modeHighlightDifferences->setChecked(true);
            break;
        case ComparisonMode::BlendImagesFalseColors:
            ui->modeBlendImagesFalseColors->setChecked(true);
            break;
        case ComparisonMode::BlendImagesTrueColors:
            ui->modeBlendImagesTrueColors->setChecked(true);
            break;
        case ComparisonMode::BlendImagesAnimated:
            ui->modeBlendImagesAnimated->setChecked(true);
            break;
    }
    updateMode();

    ui->differenceTolerance->setValue(settings.highlightDifferences.tolerance);
    ui->differenceToleranceEnabled->setChecked(settings.highlightDifferences.applyTolerance);
    ui->showMinorDifferences->setChecked(settings.highlightDifferences.showMinorDifferences);

    setBlendPosition(settings.blendPosition);
    ui->leftBlendColor->setColor(settings.blendFalseColors.leftColor);
    ui->rightBlendColor->setColor(settings.blendFalseColors.rightColor);

    setAnimationTimeBetweenImages(settings.animatedBlending.timeBetweenImages);
    ui->animationContinuous->setChecked(settings.animatedBlending.continuous);

    blockSignals.unblock();
    emit settingsChanged();
}

void ComparisonSettingsWidget::reset()
{
    setComparisonSettings({});
}

void ComparisonSettingsWidget::updateMode()
{
    switch (getComparisonMode()) {
        case ComparisonMode::HighlightDifferences:
            ui->modeOptions->setCurrentWidget(ui->highlightDifferencesOptionPage);
            break;
        case ComparisonMode::BlendImagesFalseColors:
        case ComparisonMode::BlendImagesTrueColors:
            ui->modeOptions->setCurrentWidget(ui->blendImagesOptionPage);
            break;
        case ComparisonMode::BlendImagesAnimated:
            ui->modeOptions->setCurrentWidget(ui->blendImagesAnimatedOptionPage);
            break;
    }

    updateBlendLabelColors();
    ui->blendColors->setVisible(getComparisonMode() == ComparisonMode::BlendImagesFalseColors);
}

void ComparisonSettingsWidget::updateBlendLabelColors()
{
    QPalette leftPalette = originalBlendLabelPalette;
    QPalette rightPalette = originalBlendLabelPalette;

    if (getComparisonMode() == ComparisonMode::BlendImagesFalseColors) {
        auto settings = getComparisonSettings();
        leftPalette.setColor(ui->blendLeftLabel->foregroundRole(), settings.blendFalseColors.leftColor);
        rightPalette.setColor(ui->blendRightLabel->foregroundRole(), settings.blendFalseColors.rightColor);
    }

    ui->blendLeftLabel->setPalette(leftPalette);
    ui->blendRightLabel->setPalette(rightPalette);
}

void ComparisonSettingsWidget::resetBlendPosition()
{
    setBlendPosition(ComparisonSettings().blendPosition);
}

void ComparisonSettingsWidget::resetLeftBlendColor()
{
    ui->leftBlendColor->setColor(ComparisonSettings().blendFalseColors.leftColor);
}

void ComparisonSettingsWidget::resetRightBlendColor()
{
    ui->rightBlendColor->setColor(ComparisonSettings().blendFalseColors.rightColor);
}

void ComparisonSettingsWidget::resetAnimationSpeed()
{
    setAnimationTimeBetweenImages(ComparisonSettings().animatedBlending.timeBetweenImages);
}

double ComparisonSettingsWidget::getBlendPosition() const
{
    return relativeSliderPosition(*ui->blendPosition);
}

void ComparisonSettingsWidget::setBlendPosition(double position)
{
    setRelativeSliderPosition(*ui->blendPosition, position);
}

std::chrono::milliseconds ComparisonSettingsWidget::getAnimationTimeBetweenImages() const
{
    double relativeInterval = 1 - relativeSliderPosition(*ui->animationSpeed);
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        minimumAnimationTimeBetweenImages +
        relativeInterval * (maximumAnimationTimeBetweenImages - minimumAnimationTimeBetweenImages));
}

void ComparisonSettingsWidget::setAnimationTimeBetweenImages(std::chrono::milliseconds time)
{
    double relativePosition = durationRatio(time - minimumAnimationTimeBetweenImages,
                                            maximumAnimationTimeBetweenImages - minimumAnimationTimeBetweenImages);
    setRelativeSliderPosition(*ui->animationSpeed, relativePosition);
}
