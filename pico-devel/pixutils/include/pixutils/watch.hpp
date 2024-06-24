#pragma once

#include "pixutils/system.hpp"

#include <format>
#include <iostream>
#include <optional>
#include <string>

class Watch {

  public:
    Watch(const std::optional<double> timeout = std::nullopt, const bool trigger = true)
        : timeout(timeout)
    {
        this->reset(trigger);
    }

    inline void reset(bool trigger = true)
    {
        tickpoint.reset();
        checkpoint.reset();

        if (trigger) {
            start();
        }
    }

    inline void start()
    {
        tickpoint = checkpoint = System::runtime();
    }

    bool expired() const
    {
        return timeout.has_value() ? (timeout.value() <= elapsed()) : false;
    }

    inline double elapsed() const
    {
        double dt = 0.0;

        if (checkpoint.has_value()) {
            dt = System::runtime() - checkpoint.value();
        }

        return std::max<double>(dt, 0);
    }

    const double remaining() const
    {
        double dt = 0.0;

        if (timeout.has_value()) {
            dt = timeout.value() - elapsed();
        }

        return std::max<double>(dt, 0);
    }

    const std::string to_string() const
    {
        return std::format("Watch(Elapsed={:.6f}s, Remaining={:.6f}s)", elapsed(), remaining());
    }

    friend std::ostream& operator<<(std::ostream& os, const Watch& obj)
    {
        os << obj.to_string();
        return os;
    }

  private:
    std::optional<double> timeout;
    std::optional<double> tickpoint;
    std::optional<double> checkpoint;
};
