#pragma once
#include "pixutils/time.hpp"

#include <format>
#include <iostream>
#include <map>
#include <optional>
#include <ostream>
#include <string>

using Streamer = std::optional<std::reference_wrapper<std::ostream>>;

enum LogLevel {
    CRITICAL = (1 << 0),
    ERROR    = (1 << 1),
    WARN     = (1 << 2),
    INFO     = (1 << 3),
    DEBUG    = (1 << 4),
};

class Stream {
  public:
    Stream(const std::string& name, const std::string& level, const Streamer& target = std::nullopt)
        : active(target.has_value())
        , target(target)
    {
        if (active) {
            target.value().get() << std::format("[{}][{}][{}]", Time::stamp(), name, level);
        }
    }

    ~Stream()
    {
        if (active) {
            target.value().get() << std::endl;
        }
    }

    template <typename T> Stream& operator<<(const T& message)
    {
        if (active) {
            target.value().get() << message;
        }

        return *this;
    }

    const bool     active;
    const Streamer target;
};

class Logger {
  public:
    static Logger& getLogger(const std::string& name)
    {
        if (!registry.contains(name)) {
            registry.emplace(name, Logger(name, LogLevel::DEBUG));
        }

        return registry.at(name);
    }

    Stream operator()(const LogLevel& level, bool condition = true)
    {
        return log(level, condition);
    }

    Stream log(const LogLevel& level, bool condition = true)
    {
        const bool relevant = (level <= this->level) && (condition == true);
        return Stream(name, level2string(level), relevant ? Streamer(std::cout) : std::nullopt);
    }

    const std::string name;
    const LogLevel    level;

  private:
    Logger(const std::string& name, const LogLevel& level)
        : name(name)
        , level(level)
    {
    }

    static const std::string level2string(const LogLevel& level)
    {
        switch (level) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARN: return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::CRITICAL: return "CRITICAL";
            default: return "NONE";
        }
    }

    static std::map<std::string, Logger> registry;
};

inline std::map<std::string, Logger> Logger::registry;
