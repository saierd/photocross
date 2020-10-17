#pragma once

#include <functional>
#include <utility>

class Finally {
public:
    template <typename T>
    explicit Finally(T&& f)
      : callback(std::forward<T>(f))
    {
    }

    ~Finally()
    {
        if (callback) {
            try {
                callback();
            } catch (...) {
            }
        }
    }

private:
    std::function<void()> callback;
};
