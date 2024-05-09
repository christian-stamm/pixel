#pragma once
#include "pixutils/system/logger.hpp"

#include <string>

class Device {
  public:
    Device(const std::string& name)
        : logger(Logger::getLogger(name))
    {
    }

    virtual void load()
    {
        logger.debug() << logger.name << " (Device) loaded.";
    }

    virtual void unload()
    {
        logger.debug() << logger.name << " (Device) unloaded.";
    }

    virtual void run()
    {
        logger.debug() << logger.name << " (Device) running.";
    };

  protected:
    Logger logger;
};
