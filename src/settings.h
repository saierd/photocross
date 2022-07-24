#pragma once

#include <QSettings>

class QMainWindow;

QSettings applicationSettings(QString const& scope);

void saveWindowGeometry(QMainWindow const* window, QString const& windowName);
void restoreWindowGeometry(QMainWindow* window, QString const& windowName);
