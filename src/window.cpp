#include "window.h"

#include "settings.h"

void Window::showEvent(QShowEvent* event)
{
    if (!wasShown) {
        restoreWindowGeometry(this, metaObject()->className());
        wasShown = true;
    }

    QMainWindow::showEvent(event);
}

void Window::closeEvent(QCloseEvent* event)
{
    saveWindowGeometry(this, metaObject()->className());

    QMainWindow::closeEvent(event);
}