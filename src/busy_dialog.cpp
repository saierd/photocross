#include "busy_dialog.h"

#include <QProgressDialog>
#include <QtConcurrent/QtConcurrent>

#include <thread>

using namespace std::literals::chrono_literals;

void runWithBusyDialog(QString const& text, std::function<void()> const& f, QWidget* parent)
{
    QProgressDialog dialog(parent);
    dialog.setLabelText(text);
    dialog.setCancelButton(nullptr);

    // Show the progress dialog on top of the current window, but do not steal the focus. This makes sure that any
    // widgets the user currently interacts with stay as they are. This is important, since an action might be
    // processing in an intermediate state, while the user is not done yet.
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAttribute(Qt::WA_ShowWithoutActivating, true);
    dialog.setWindowFlags(dialog.windowFlags() | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint |
                          Qt::X11BypassWindowManagerHint | Qt::Tool | Qt::WindowStaysOnTopHint |
                          Qt::WindowTransparentForInput | Qt::WindowDoesNotAcceptFocus);

    // Setting all values to 0 puts the progress into busy indicator mode.
    dialog.setRange(0, 0);
    dialog.setValue(0);

    // Only show the busy dialog if execution takes longer than this time.
    dialog.setMinimumDuration(500);

    auto task = QtConcurrent::run(f);
    while (!task.isFinished()) {
        QCoreApplication::processEvents();
        std::this_thread::sleep_for(5ms);
    }
}
