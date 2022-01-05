#pragma once

#include "session.h"

#include <QPixmap>

#include <vector>

class ComparisonSettings;

struct ComparisonViewLayer {
    QPixmap pixmap;
    double opacity = 1.;
};

using ComparisonViewLayers = std::vector<ComparisonViewLayer>;

ComparisonViewLayers computeComparisonViewLayers(std::vector<Session::ImageHandle> const& images,
                                                 ComparisonSettings const& settings);
