#pragma once

#include "comparison_settings.h"
#include "session.h"
#include "widgets/image_view.h"

#include <QTimer>

#include <chrono>

class ComparisonView : public ImageView {
    Q_OBJECT

public:
    explicit ComparisonView(QWidget* parent = nullptr);

    void update(Session::Images const& images, ComparisonSettings const& settings);

signals:
    void requestAnimationUpdate();

private:
    void updateAnimationTimerSettings(ComparisonSettings const& settings);
    double currentAnimationStep(ComparisonSettings const& settings);

    QTimer animationUpdateTimer;
    std::chrono::steady_clock::time_point animationStartTime;
    std::chrono::milliseconds previousAnimationTimeBetweenImages;
};
