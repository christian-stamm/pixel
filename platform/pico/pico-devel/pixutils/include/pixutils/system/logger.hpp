#pragma once
#include "pixutils/time/time.hpp"

#include <ios>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

#define BLOCK_INTENT 30

enum class LogLevel {
    CRITICAL = (1 << 0),
    ERROR    = (1 << 1),
    WARN     = (1 << 2),
    INFO     = (1 << 3),
    DEBUG    = (1 << 4),
};

class Stream {
  public:
    Stream(const std::string& level, const std::string& name, const bool flush = true)
        : ss(std::stringstream())
        , name(name)
        , level(level)
        , flush(flush)
    {
    }

    ~Stream()
    {
        print(ss.str());
    }

    template <typename T> Stream& operator<<(const T& data)
    {
        if (flush) {
            ss << data;
        }

        return *this;
    }

    inline void print(const std::string& msg)
    {
        if (!msg.empty() && flush) {
            std::stringstream fmt;
            const std::string time(Time::now().to_string());

            fmt << std::setw(BLOCK_INTENT) << std::left;
            fmt << std::setfill(' ') << std::uppercase;
            fmt << ("[" + time + "]" + "[" + level + "]" + "[" + name + "] ");

            std::cout << fmt.str() << msg << std::endl;
        }
    }

    const std::string name;
    const bool        flush;

  private:
    std::stringstream ss;
    const std::string level;
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

    Stream debug() const
    {
        return log(LogLevel::DEBUG);
    }

    Stream info() const
    {
        return log(LogLevel::INFO);
    }

    Stream warn() const
    {
        return log(LogLevel::WARN);
    }

    Stream error() const
    {
        return log(LogLevel::ERROR);
    }

    Stream critical() const
    {
        return log(LogLevel::CRITICAL);
    }

    Stream log(const LogLevel& request) const
    {
        bool flush = request <= this->level;
        return Stream(level2string(request), name, flush);
    }

    const std::string name;
    volatile LogLevel level;

  private:
    Logger(const std::string& name, const LogLevel& logLevel)
        : name(name)
        , level(logLevel)
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
