#include "comparison_settings.h"

#include "ui_comparison_settings.h"

QColor const blendImage1Color(255, 0, 0);
QColor const blendImage2Color(0, 0, 255);

ComparisonSettings::ComparisonSettings(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::ComparisonSettings>();
    ui->setupUi(this);
    originalBlendLabelPalette = ui->blendLeftLabel->palette();

    ui->comparisonMode->setCurrentIndex(0);
    connect(ui->comparisonMode, &QTabWidget::currentChanged, this, &ComparisonSettings::settingsChanged);

    connect(ui->differenceToleranceEnabled, &QCheckBox::toggled, [this]() {
        ui->differenceToleranceLabel->setNum(getDifferenceTolerance());
        emit(settingsChanged());
    });
    connect(ui->differenceTolerance, &QSlider::valueChanged, [this](int value) {
        if (value > 0) {
            ui->differenceToleranceEnabled->setChecked(true);
        }

        ui->differenceToleranceLabel->setNum(getDifferenceTolerance());
        emit(settingsChanged());
    });
    connect(ui->showMinorDifferences, &QCheckBox::toggled, this, &ComparisonSettings::settingsChanged);

    connect(ui->resetBlendPosition, &QPushButton::clicked, [this]() {
        ui->blendPosition->setValue(ui->blendPosition->maximum() / 2);
    });
    connect(ui->blendPosition, &QSlider::valueChanged, this, &ComparisonSettings::settingsChanged);
    connect(ui->blendTrueColors, &QCheckBox::toggled, [this]() {
        updateBlendLabelColors();
        emit(settingsChanged());
    });
    updateBlendLabelColors();
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
    return static_cast<double>(ui->blendPosition->value() - ui->blendPosition->minimum()) /
           ui->blendPosition->maximum();
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
