#include "command_line.h"
#include "screenshot_manager.h"
#include "session.h"
#include "windows/main_window.h"

#include <QApplication>

int main(int argc, char** argv)
{
    QApplication application(argc, argv);
    QApplication::setApplicationName("Photocross");
    QApplication::setWindowIcon(QIcon(":/icon.svg"));

    QIcon::setThemeName("icons");

    auto options = parseCommandLineOptions(application);
    QStringList filesToOpen = options.files;

    MainWindow mainWindow;
    mainWindow.getSession().loadImages(filesToOpen);

    ScreenshotManager screenshotManager;
    QObject::connect(&screenshotManager, &ScreenshotManager::newImageAvailable, &mainWindow, [&](QImage image) {
        mainWindow.getSession().addImage(std::move(image));
    });

    mainWindow.show();
    return QApplication::exec();
}
