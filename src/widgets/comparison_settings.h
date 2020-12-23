#pragma once

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

signals:
    void settingsChanged() const;

private:
    std::unique_ptr<Ui::ComparisonSettings> ui;
};
