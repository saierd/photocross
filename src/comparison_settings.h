#pragma once

#include <QColor>

#include <chrono>

using namespace std::chrono_literals;

auto const minimumAnimationTimeBetweenImages = 100ms;
auto const maximumAnimationTimeBetweenImages = 1000ms;

enum class ComparisonMode {
    // Compare all images, highlight the pixels where not all of them agree.
    HighlightDifferences,

    // Blend two images by laying making them half transparent and displaying them on top of each other. These modes are
    // only available for two images.
    BlendImagesFalseColors,
    BlendImagesTrueColors,

    // Repeatedly cycle through the images, always blending between two of them as in the true color blending mode.
    BlendImagesAnimated,
};

struct ComparisonSettings {
    ComparisonMode mode = ComparisonMode::HighlightDifferences;

    // Additional settings for the highlight difference mode.
    struct {
        // Ignore minor differences below this number of gray values.
        int tolerance = 10;

        // Toggle the tolerance off or on. The tolerance value gets preserved while application is disabled.
        bool applyTolerance = false;

        // Whether to completely ignore minor differences or show them in a separate color.
        bool showMinorDifferences = false;

        // Transparency of the image layer that contains the highlighted pixels compared to the grayvalue version of the
        // original image in the background.
        double differencesTransparency = 0.7;
    } highlightDifferences;

    // Current blending position for the two-image blending modes (both true and false colors).
    double blendPosition = 0.5;

    // Additional settings for the false color blending mode.
    struct {
        QColor leftColor{255, 0, 0};
        QColor rightColor{0, 0, 255};
    } blendFalseColors;

    // Additional settings for the animated blending mode.
    struct {
        std::chrono::milliseconds timeBetweenImages = 500ms;
        bool continuous = true;
    } animatedBlending;
};
