#pragma once
#include <chrono>
#include <ratio>

using TimePoint  = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;
using usDuration = std::chrono::duration<double, std::micro>;

class Time {

  public:
    inline static TimePoint now()
    {
        auto t = std::chrono::system_clock::now();
        return std::chrono::time_point_cast<std::chrono::microseconds>(t);
    }

    inline static const std::string stamp()
    {
        return std::format("{:%H:%M:%S}", now());
    }
};
