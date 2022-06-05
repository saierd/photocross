#pragma once

#include <QObject>

#include <memory>

class QHotkey;

class ScreenshotManager : public QObject {
    Q_OBJECT

public:
    ScreenshotManager();
    ~ScreenshotManager() override;

signals:
    void newImageAvailable(QImage image);

private:
    std::unique_ptr<QHotkey> currentApplicationHotkey;
    std::unique_ptr<QHotkey> desktopHotkey;
};
