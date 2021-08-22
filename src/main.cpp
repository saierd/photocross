#include "command_line.h"
#include "screenshot.h"
#include "screenshot_file_manager.h"
#include "session.h"
#include "windows/main_window.h"

#include <QApplication>

int main(int argc, char** argv)
{
    QApplication application(argc, argv);
    QApplication::setApplicationName("Slika");
    QApplication::setWindowIcon(QIcon(":/icon.svg"));

    QIcon::setThemeName("icons");

    auto options = parseCommandLineOptions(application);
    QStringList filesToOpen = options.files;

    if (options.clearScreenshots || options.takeScreenshot || options.takeDesktopScreenshot) {
        ScreenshotFileManager screenshotFiles;

        if (options.clearScreenshots) {
            screenshotFiles.removeFiles();
        }

        if (options.takeScreenshot) {
            screenshotFiles.addImage(takeScreenshotOfActiveWindow());
        } else if (options.takeDesktopScreenshot) {
            screenshotFiles.addImage(takeScreenshot());
        }

        filesToOpen << screenshotFiles.getFiles();
        if (filesToOpen.size() < 2) {
            // Not enough screenshots collected for comparison yet.
            return 0;
        }
    }

    MainWindow mainWindow;
    mainWindow.getSession().loadImages(filesToOpen);
    mainWindow.show();

    return QApplication::exec();
}
