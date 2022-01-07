#include "settings.h"

#include <QApplication>
#include <QMainWindow>

namespace {

QString windowGeometryGroup(QString const& windowName)
{
    return windowName + "Geometry";
}

}  // namespace

QSettings applicationSettings()
{
    auto const& applicationName = QApplication::applicationName();
    return QSettings(QSettings::IniFormat, QSettings::UserScope, applicationName, applicationName);
}

void saveWindowGeometry(QMainWindow const* window, QString const& windowName)
{
    auto settings = applicationSettings();

    settings.beginGroup(windowGeometryGroup(windowName));
    settings.setValue("geometry", window->saveGeometry());
    settings.setValue("state", window->saveState());
}

void restoreWindowGeometry(QMainWindow* window, QString const& windowName)
{
    auto settings = applicationSettings();

    settings.beginGroup(windowGeometryGroup(windowName));
    window->restoreGeometry(settings.value("geometry").toByteArray());
    window->restoreState(settings.value("state").toByteArray());
}
