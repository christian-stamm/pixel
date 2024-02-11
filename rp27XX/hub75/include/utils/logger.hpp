#pragma once
#include "time.hpp"

#include <iomanip>
#include <iostream>
#include <map>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>

#define BLOCK_INTENT 33

enum class LogLevel
{
    CRITICAL = (1 << 0),
    ERROR    = (1 << 1),
    WARN     = (1 << 2),
    INFO     = (1 << 3),
    DEBUG    = (1 << 4),
};

class LogStream {
  public:
    LogStream(const LogLevel& logLevel, const std::string& stream)
        : ss(std::stringstream())
        , stream(stream)
        , logLevel(logLevel)
    {
    }

    // LogStream& operator<<(std::ios_base& (*manip)(std::ios_base&))
    // {
    //     ss << manip;
    //     return *this;
    // }

    template <typename T> LogStream& operator<<(const T& data)
    {
        ss << data;
        return *this;
    }

    ~LogStream()
    {
        const std::string& msg = ss.str();
        print(msg);
    }

    inline void print(const std::string& msg)
    {
        if (msg.empty()) {
            return;
        }

        std::stringstream  fmt;
        const std::string& a = Time::now().to_string();
        const std::string& b = level2string(logLevel);
        const std::string& c = stream;

        fmt << std::left;
        fmt << std::setfill(' ');
        fmt << std::setw(BLOCK_INTENT) << "[" + a + "]" + "[" + b + "]" + "[" + c + "] ";

        streamLock.lock();
        std::cout << fmt.str() << msg << std::endl;
        streamLock.unlock();
    }

  private:
    static const std::string level2string(const LogLevel& logLevel)
    {
        switch (logLevel) {
            case LogLevel::DEBUG: return "DEBUG";
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARN: return "WARN";
            case LogLevel::ERROR: return "ERROR";
            case LogLevel::CRITICAL: return "CRITICAL";
            default: return "NONE";
        }
    }

    std::stringstream ss;
    const std::string stream;
    const LogLevel    logLevel;

    static std::mutex streamLock;
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

    void setLogLevel(const LogLevel& loglevel)
    {
        this->logLevel = loglevel;
    }

    LogStream log() const
    {
        return LogStream(logLevel, name);
    }

    LogStream debug() const
    {
        return LogStream(LogLevel::DEBUG, name);
    }

    LogStream info() const
    {
        return LogStream(LogLevel::INFO, name);
    }

    LogStream warn() const
    {
        return LogStream(LogLevel::WARN, name);
    }

    LogStream error() const
    {
        return LogStream(LogLevel::ERROR, name);
    }

    LogStream critical() const
    {
        return LogStream(LogLevel::CRITICAL, name);
    }

    const std::string name;

  private:
    Logger(const std::string& name, const LogLevel& logLevel)
        : name(name)
        , logLevel(logLevel)
    {
    }

    LogLevel logLevel;

    static std::map<std::string, Logger> registry;
};

inline std::mutex                    LogStream::streamLock;
inline std::map<std::string, Logger> Logger::registry;
