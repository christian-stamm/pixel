#pragma once
#include "pico/types.h"
#include "pixutils/types.hpp"

#include <iomanip>
#include <iostream>
#include <optional>
#include <pico/time.h>
#include <sstream>
#include <string>

class Time {

  public:
    struct Stamp {

        DWord hours   = 0;
        DWord minutes = 0;
        DWord seconds = 0;
        DWord millis  = 0;
        DWord micros  = 0;

        inline static const Stamp convert(const double abs_secs)
        {
            Stamp       stamp;
            const DWord abs_ts = abs_secs * 1.0;
            const DWord abs_ms = abs_secs * 1e3;
            const DWord abs_us = abs_secs * 1e6;

            stamp.hours   = (abs_ts / 3600);
            stamp.minutes = (abs_ts % 3600) / 60.0;
            stamp.seconds = (abs_ts % 60);
            stamp.millis  = (abs_ms % 1000);
            stamp.micros  = (abs_us % 1000);

            return stamp;
        }

        inline static double convert(const Stamp& stamp)
        {
            double time = 0;

            time += 3600.0 * stamp.hours;
            time += 60.0 * stamp.minutes;
            time += 1.0 * stamp.seconds;
            time += 1e-3 * stamp.millis;
            time += 1e-6 * stamp.micros;

            return time;
        }

        const std::string to_string() const
        {
            std::stringstream ss;
            ss << std::setfill('0');
            ss << std::setw(2) << hours << ":";
            ss << std::setw(2) << minutes << ":";
            ss << std::setw(2) << seconds << ":";
            ss << std::setw(3) << millis << ":";
            ss << std::setw(3) << micros;
            return ss.str();
        }

        friend std::ostream& operator<<(std::ostream& os, const Stamp& obj)
        {
            os << "Timestamp(" << obj.to_string() << ")";
            return os;
        }
    };

    inline static double runtime()
    {
        return 1e-6 * to_us_since_boot(get_absolute_time());
    }

    inline static Stamp now()
    {
        return Stamp::convert(runtime());
    }
};
