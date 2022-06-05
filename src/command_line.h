#pragma once

#include <QStringList>

class QApplication;

struct CommandLineOptions {
    QStringList files;
};

CommandLineOptions parseCommandLineOptions(QApplication const& application);
