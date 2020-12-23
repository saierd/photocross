#pragma once

#include <QColor>
#include <QWidget>

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

    ComparisonMode getComparisonMode() const;

    int getDifferenceTolerance() const;
    bool showMinorDifferences() const;

    double getBlendPosition() const;
    QColor blendImage1Color() const;
    QColor blendImage2Color() const;

signals:
    void settingsChanged() const;

private slots:
    void updateBlendLabelColors();

private:
    std::unique_ptr<Ui::ComparisonSettings> ui;
    QPalette originalBlendLabelPalette;
};
