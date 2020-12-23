#include "comparison_settings.h"

#include "ui_comparison_settings.h"

ComparisonSettings::ComparisonSettings(QWidget* parent)
  : QWidget(parent)
{
    ui = std::make_unique<Ui::ComparisonSettings>();
    ui->setupUi(this);

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
