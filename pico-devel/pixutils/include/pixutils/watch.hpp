#pragma once
#include "pixutils/time.hpp"

#include <format>
#include <iostream>
#include <optional>
#include <string>

using usMoment   = std::chrono::time_point<std::chrono::system_clock, std::chrono::microseconds>;
using usDuration = std::chrono::duration<double, std::micro>;

class Watch {

  public:
    Watch(const std::optional<usDuration> timeout = std::nullopt, const bool trigger = true)
        : timeout(timeout)
    {
        this->reset(trigger);
    }

    inline void reset(bool trigger = true)
    {
        tickpoint  = std::nullopt;
        checkpoint = std::nullopt;

        if (trigger) {
            start();
        }
    }

    inline void start()
    {
        checkpoint = Time::now();
        tickpoint  = checkpoint;
    }

    bool expired() const
    {
        return timeout.has_value() ? (timeout.value() <= elapsed()) : false;
    }

    inline usDuration elapsed() const
    {
        if (!checkpoint.has_value()) {
            return usDuration(0);
        }

        return Time::now() - checkpoint.value();
    }

    const usDuration remaining() const
    {
        if (!timeout.has_value()) {
            return usDuration(0);
        }

        return timeout.value() - elapsed();
    }

    const std::string to_string() const
    {
        return std::format("Watch(Elapsed={}, Remaining={})", elapsed().count(), remaining().count());
    }

    friend std::ostream& operator<<(std::ostream& os, const Watch& obj)
    {
        os << obj.to_string();
        return os;
    }

  private:
    std::optional<usDuration>  timeout;
    std::optional<usTimePoint> tickpoint;
    std::optional<usTimePoint> checkpoint;
};
