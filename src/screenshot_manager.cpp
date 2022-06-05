#include "screenshot_manager.h"

#include "screenshot.h"

#include <QHotkey>

ScreenshotManager::ScreenshotManager()
{
    currentApplicationHotkey = std::make_unique<QHotkey>(QKeySequence("Ctrl+Print"), true);
    desktopHotkey = std::make_unique<QHotkey>(QKeySequence("Ctrl+Shift+Print"), true);

    connect(currentApplicationHotkey.get(), &QHotkey::activated, this, [&]() {
        emit newImageAvailable(takeScreenshotOfActiveWindow().toImage());
    });
    connect(desktopHotkey.get(), &QHotkey::activated, this, [&]() {
        emit newImageAvailable(takeScreenshot().toImage());
    });
}

ScreenshotManager::~ScreenshotManager() = default;
