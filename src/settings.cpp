#include "settings.h"

#include <QApplication>
#include <QMainWindow>

QSettings applicationSettings(QString const& scope)
{
    return QSettings(QSettings::IniFormat, QSettings::UserScope, QApplication::applicationName(), scope);
}

QSettings windowGeometrySettings()
{
    return applicationSettings("WindowGeometry");
}

void saveWindowGeometry(QMainWindow const* window, QString const& windowName)
{
    auto settings = windowGeometrySettings();

    settings.beginGroup(windowName);
    settings.setValue("geometry", window->saveGeometry());
    settings.setValue("state", window->saveState());
}

void restoreWindowGeometry(QMainWindow* window, QString const& windowName)
{
    auto settings = windowGeometrySettings();

    settings.beginGroup(windowName);
    window->restoreGeometry(settings.value("geometry").toByteArray());
    window->restoreState(settings.value("state").toByteArray());
}
