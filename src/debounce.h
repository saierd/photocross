#pragma once

#include <QTimer>

#include <chrono>
#include <memory>

using namespace std::literals::chrono_literals;

namespace detail {

class DebounceTimer {
public:
    explicit DebounceTimer(std::chrono::milliseconds const& delay)
    {
        timer.setSingleShot(true);
        timer.setInterval(delay);
    }

    void stop()
    {
        timer.stop();
        QObject::disconnect(connection);
    }

    template <typename F>
    void start(F&& f)
    {
        connection = QObject::connect(&timer, &QTimer::timeout, f);
        timer.start();
    }

    template <typename F>
    void restart(F&& f)
    {
        stop();
        start(std::forward<F>(f));
    }

private:
    QTimer timer;
    QMetaObject::Connection connection;
};

}  // namespace detail

template <typename F>
auto debounceSlot(F&& f, std::chrono::milliseconds const& delay)
{
    // The timer is stored in a shared pointer to allow copying of the resulting debounced function (e.g. to connect it
    // to multiple signals). Note that this means that all copies will use the same debouncing timer, which limits the
    // execution of all of them together.
    auto timer = std::make_shared<detail::DebounceTimer>(delay);

    return [timer, f = std::forward<F>(f)]() mutable {
        timer->restart(f);
    };
}
