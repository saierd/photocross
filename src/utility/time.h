#pragma once

#include <chrono>

using DoubleMilliseconds = std::chrono::duration<double, std::milli>;

template <typename Duration1, typename Duration2>
double durationRatio(Duration1 const& a, Duration2 const& b)
{
    return DoubleMilliseconds(a) / DoubleMilliseconds(b);
}
