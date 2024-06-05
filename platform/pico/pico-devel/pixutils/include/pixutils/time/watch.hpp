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
    Watch(const std::optional<double>& timeout = std::nullopt, const bool trigger = true)
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

    inline bool expired() const
    {
        return timeout.has_value() ? (timeout.value() <= elapsed()) : false;
    }

    inline const double elapsed() const
    {
        return checkpoint.has_value() ? delta(checkpoint.value()) : 0;
    }

    inline double remaining() const
    {
        return timeout.has_value() ? std::max(timeout.value() - elapsed(), 0.0) : 0.0;
    }

    inline double tick()
    {
        const double t  = Time::runtime();
        const double dt = tickpoint.has_value() ? t - tickpoint.value() : 0;

        tickpoint = t;
        return dt;
    }

    inline const Estimator& capture()
    {
        estimator.update(tick());
        return estimator;
    }

    inline const std::string to_string() const
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

  private:
    inline const double delta(const double reference) const
    {
        return Time::runtime() - reference;
    }

    Estimator             estimator;
    std::optional<double> timeout;
    std::optional<double> tickpoint;
    std::optional<double> checkpoint;
};
