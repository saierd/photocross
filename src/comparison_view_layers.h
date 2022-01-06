#pragma once

#include "session.h"

#include <QPixmap>

#include <vector>

struct ComparisonSettings;

struct ComparisonViewLayer {
    QPixmap pixmap;
    double opacity = 1.;
};

using ComparisonViewLayers = std::vector<ComparisonViewLayer>;

ComparisonViewLayers computeComparisonViewLayers(Session::Images const& images,
                                                 ComparisonSettings const& settings,
                                                 double animationStep = 0.);
