#include "command_line.h"

#include <QApplication>
#include <QCommandLineParser>

CommandLineOptions parseCommandLineOptions(QApplication const& application)
{
    QCommandLineOption clearScreenshotsOption({"c", "clear-screenshots"}, "Clear screenshot files.");
    QCommandLineOption takeScreenshotOption({"s", "screenshot"}, "Take screenshot of active window.");
    QCommandLineOption takeDesktopScreenshotOption("screenshot-desktop", "Take screenshot of desktop.");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption(clearScreenshotsOption);
    parser.addOption(takeScreenshotOption);
    parser.addOption(takeDesktopScreenshotOption);
    parser.addPositionalArgument("files", "Image files to open.", "[files...]");

    parser.process(application);

    return {parser.isSet(clearScreenshotsOption),
            parser.isSet(takeScreenshotOption),
            parser.isSet(takeDesktopScreenshotOption),
            parser.positionalArguments()};
}
