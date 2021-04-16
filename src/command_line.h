#pragma once

#include <QStringList>

class QApplication;

struct CommandLineOptions {
    bool clearScreenshots;
    bool takeScreenshot;
    bool takeDesktopScreenshot;

    QStringList files;
};

CommandLineOptions parseCommandLineOptions(QApplication const& application);
