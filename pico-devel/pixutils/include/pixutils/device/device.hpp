#pragma once
#include "pixutils/logger.hpp"

#include <string>

class Device {
  public:
    Device(const std::string& name)
        : logger(Logger::getLogger(name))
        , loaded(false)
    {
    }

    ~Device() = default;

    void start()
    {
        if (!isRunning()) {
            resetState();
            prepare();
            loaded = true;
            logger(DEBUG) << "Device started.";
        }
    }

    void stop()
    {
        if (isRunning()) {
            cleanup();
            resetState();
            loaded = false;
            logger(DEBUG) << "Device stopped.";
        }
    }

    void restart()
    {
        stop();
        start();
    }

    bool isRunning() const
    {
        return loaded;
    }

  protected:
    virtual void prepare() = 0;
    virtual void cleanup() = 0;
    virtual void resetState() {};

    Logger logger;

  private:
    bool loaded;
};
