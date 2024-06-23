#pragma once
#include <chrono>

using TimePoint = std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>;
using Duration  = std::chrono::duration<double>;

class Time {

  public:
    inline static TimePoint now()
    {
        auto sysclk = std::chrono::system_clock::now();
        return std::chrono::time_point_cast<std::chrono::seconds>(sysclk);
    }

    inline static const std::string stamp()
    {
        return std::format("{:%H:%M:%S}", now());
    }
};
