#include "command_line.h"

#include <QApplication>
#include <QCommandLineParser>

CommandLineOptions parseCommandLineOptions(QApplication const& application)
{
    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addPositionalArgument("files", "Image files to open.", "[files...]");

    parser.process(application);

    return {parser.positionalArguments()};
}
