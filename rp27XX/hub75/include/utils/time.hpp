#pragma once
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <stdint.h>
#include <chrono>
#include <optional>

class Time
{

public:
    struct Stamp
    {
        uint64_t hours = 0;
        uint64_t minutes = 0;
        uint64_t seconds = 0;
        uint64_t millis = 0;
        uint64_t micros = 0;

        inline static const Stamp convert(const double abs_secs)
        {
            Stamp stamp;
            const uint64_t abs_ts = abs_secs * 1.0;
            const uint64_t abs_ms = abs_secs * 1e3;
            const uint64_t abs_us = abs_secs * 1e6;

            stamp.hours = (abs_ts / 3600);
            stamp.minutes = (abs_ts % 3600) / 60.0;
            stamp.seconds = (abs_ts % 60);
            stamp.millis = (abs_ms % 1000);
            stamp.micros = (abs_us % 1000);

            return stamp;
        }

        inline static double convert(const Stamp &stamp)
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

        friend std::ostream &operator<<(std::ostream &os, const Stamp &obj)
        {
            os << "Timestamp(" << obj.to_string() << ")";
            return os;
        }
    };

    inline static double runtime()
    {
        if (!launchTime.has_value())
        {
            launchTime = std::chrono::high_resolution_clock::now();
        }

        return 1e-6 * std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - launchTime.value()).count();
    }

    inline static Stamp now()
    {
        return Stamp::convert(runtime());
    }

private:
    static std::optional<std::chrono::high_resolution_clock::time_point> launchTime;
};

inline std::optional<std::chrono::high_resolution_clock::time_point> Time::launchTime;
