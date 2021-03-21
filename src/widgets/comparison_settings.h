#pragma once

#include <QColor>
#include <QTimer>
#include <QWidget>

#include <chrono>
#include <memory>

namespace Ui {
class ComparisonSettings;
}

enum class ComparisonMode { HighlightDifferences, BlendImages };

class ComparisonSettings : public QWidget {
    Q_OBJECT

public:
    explicit ComparisonSettings(QWidget* parent = nullptr);
    ~ComparisonSettings() override;

    void setNumberOfImages(size_t _numberOfImages);

    ComparisonMode getComparisonMode() const;

    int getDifferenceTolerance() const;
    bool showMinorDifferences() const;

    double getBlendPosition() const;
    void setBlendPosition(double position);
    QColor blendImage1Color() const;
    QColor blendImage2Color() const;

signals:
    void settingsChanged();

private slots:
    void resetBlendPosition();
    void updateBlendLabelColors();

    void resetBlendSpeed();

    void updateBlendTimerSettings();
    void onBlendTimerTimeout();

private:
    std::chrono::milliseconds currentBlendIntervalBetweenImages() const;

    std::unique_ptr<Ui::ComparisonSettings> ui;
    QPalette originalBlendLabelPalette;

    size_t numberOfImages = 1;

    QTimer blendTimer;
    int currentBlendDirection = 1;
    double blendPositionBeforeAutomaticMode = 0;
};
