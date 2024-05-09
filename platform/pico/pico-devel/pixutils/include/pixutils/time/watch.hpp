#pragma once
#include "pixutils/math/estimator.hpp"
#include "pixutils/time/time.hpp"

#include <ios>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>

class Watch {

  public:
    Watch(const std::optional<const double> timeout = std::nullopt, const bool trigger = true)
        : timeout(timeout)
        , estimator(Estimator())
        , tickpoint(std::nullopt)
        , checkpoint(std::nullopt)
    {
        this->reset(trigger);
    }

    inline void reset(bool trigger = true)
    {
        estimator.reset();
        tickpoint  = std::nullopt;
        checkpoint = std::nullopt;

        if (trigger) {
            start();
        }
    }

    inline void start()
    {
        checkpoint = Time::runtime();
        tickpoint  = checkpoint;
    }

    bool expired() const
    {
        return timeout.has_value() ? (timeout.value() <= elapsed()) : false;
    }

    inline const double elapsed() const
    {
        return checkpoint.has_value() ? delta(checkpoint.value()) : 0;
    }

    const double remaining() const
    {
        return timeout.has_value() ? std::max(timeout.value() - elapsed(), 0.0) : 0.0;
    }

    const Estimator& tick()
    {
        const double t = tickpoint.value_or(Time::runtime());

        if (tickpoint.has_value()) {
            estimator.update(delta(t));
        }

        tickpoint = t;
        return estimator;
    }

    const std::string to_string() const
    {
        const std::string placeholder = "<undefined>";

        const double start = checkpoint.value_or(0);
        const double end   = start + timeout.value_or(0);

        const Time::Stamp& a = Time::Stamp::convert(start);
        const Time::Stamp& b = Time::Stamp::convert(end);
        const Time::Stamp& c = Time::Stamp::convert(elapsed());
        const Time::Stamp& d = Time::Stamp::convert(remaining());

        std::stringstream ss;

        if (checkpoint.has_value()) {
            ss << "start=" << a << ", ";
            ss << "elapsed=" << c << ", ";
        }
        else {
            ss << "start=" << placeholder << ", ";
            ss << "elapsed=" << placeholder << ", ";
        }

        if (timeout.has_value()) {
            ss << "stop=" << b << ", ";
            ss << "remaining=" << d << ", ";
            ss << "expired=" << std::boolalpha << expired();
        }
        else {
            ss << "stop=" << placeholder << ", ";
            ss << "remaining=" << placeholder << ", ";
            ss << "expired=" << placeholder << ", ";
        }

        return ss.str();
    }

    friend std::ostream& operator<<(std::ostream& os, const Watch& obj)
    {
        os << "Watch(" << obj.to_string() << ")";
        return os;
    }

    const std::optional<const double> timeout;

  private:
    inline const double delta(const double reference) const
    {
        return Time::runtime() - reference;
    }

    Estimator             estimator;
    std::optional<double> tickpoint;
    std::optional<double> checkpoint;
};
