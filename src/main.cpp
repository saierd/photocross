#include "windows/main_window.h"

#include <QApplication>

int main(int argc, char** argv)
{
    QApplication application(argc, argv);
    QIcon::setThemeName("icons");

    MainWindow mainWindow;
    mainWindow.show();

    return QApplication::exec();
}
