#pragma once

#include "comparison_settings.h"

#include <QWidget>

#include <memory>

namespace Ui {
class ComparisonSettings;
}

class ComparisonSettingsWidget : public QWidget {
    Q_OBJECT

public:
    explicit ComparisonSettingsWidget(QWidget* parent = nullptr);
    ~ComparisonSettingsWidget() override;

    void setNumberOfImages(size_t numberOfImages);

    ComparisonMode getComparisonMode() const;
    ComparisonSettings getComparisonSettings() const;

    void setComparisonSettings(ComparisonSettings const& settings);

signals:
    void modeChanged();
    void settingsChanged();

public slots:
    void reset();

private slots:
    void updateMode();

    void updateBlendLabelColors();
    void resetBlendPosition();

    void resetAnimationSpeed();

private:
    double getBlendPosition() const;
    void setBlendPosition(double position);

    std::chrono::milliseconds getAnimationTimeBetweenImages() const;
    void setAnimationTimeBetweenImages(std::chrono::milliseconds time);

private:
    std::unique_ptr<Ui::ComparisonSettings> ui;
    QPalette originalBlendLabelPalette;
};
