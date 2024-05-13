#pragma once
#include <math.h>

template <typename T> static const T& clamp(const T& value, const T& lower, const T& upper)
{
    return std::min<T>(std::max<T>(value, lower), upper);
}