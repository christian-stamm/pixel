#pragma once
#include "pixtools/config.hpp"
#include "pixtools/sys/logger.hpp"

#include <string>

class Device {
  public:
    Device(const std::string& name, const Config& config)
        : logger(Logger::getLogger(name))
        , config(config)

    {
    }

    virtual void load()
    {
        logger.log() << logger.name << "Device loaded.";
    }

    virtual void unload()
    {
        logger.log() << logger.name << "Device unloaded.";
    }

    virtual void run()
    {
        logger.log() << logger.name << "Device running.";
    };

  protected:
    Logger logger;
    Config config;
};
