#include "screenshot.h"

#include "xcb/xcb.h"

#include <QGuiApplication>
#include <QScopeGuard>
#include <QScreen>

WId findTopLevelWindow(xcb_connection_t* connection, WId window)
{
    while (true) {
        auto* treeReply = xcb_query_tree_reply(connection, xcb_query_tree(connection, window), nullptr);
        if (treeReply == nullptr) {
            return 0;
        }
        auto freeTreeReply = qScopeGuard([&]() {
            free(treeReply);
        });

        if (window == treeReply->root || treeReply->parent == treeReply->root) {
            return window;
        }
        window = treeReply->parent;
    }
}

WId getActiveWindowId()
{
    auto* connection = xcb_connect(nullptr, nullptr);
    if (connection == nullptr) {
        return 0;
    }
    auto closeDisplay = qScopeGuard([&]() {
        xcb_disconnect(connection);
    });

    auto* focusReply = xcb_get_input_focus_reply(connection, xcb_get_input_focus(connection), nullptr);
    xcb_window_t activeWindow = focusReply->focus;
    free(focusReply);

    return findTopLevelWindow(connection, activeWindow);
}

QPixmap grabScreen(QScreen* screen, WId windowId, QRect const& area)
{
    return screen->grabWindow(windowId, area.x(), area.y(), area.width(), area.height());
}

QPixmap takeScreenshot()
{
    auto* screen = QGuiApplication::primaryScreen();
    return grabScreen(screen, 0, screen->virtualGeometry());
}

QPixmap takeScreenshotOfActiveWindow()
{
    auto* screen = QGuiApplication::primaryScreen();
    return screen->grabWindow(getActiveWindowId());
}
