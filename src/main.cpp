#include "session.h"
#include "windows/main_window.h"

#include <QApplication>

int main(int argc, char** argv)
{
    QApplication application(argc, argv);
    QIcon::setThemeName("icons");

    MainWindow mainWindow;

    if (argc > 1) {
        QStringList filenames;
        for (int i = 1; i < argc; i++) {
            filenames.push_back(QString(argv[i]));
        }

        mainWindow.getSession().loadImages(filenames);
    }

    mainWindow.show();
    return QApplication::exec();
}
