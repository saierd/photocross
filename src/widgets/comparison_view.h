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

    void update(Session const& session, ComparisonSettings const& _settings);

private slots:
    void update();
    void animationUpdate();

private:
    Session::Images images;
    ComparisonSettings settings;

    QTimer animationUpdateTimer;
    std::chrono::steady_clock::time_point animationStartTime;
};
