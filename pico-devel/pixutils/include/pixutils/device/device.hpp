#pragma once
#include "pixutils/logger.hpp"
#include "pixutils/uuid.hpp"

#include <string>

class Device {
  public:
    Device(const std::string& name)
        : uuid(UUID::generate())
        , logger(Logger::getLogger(name))
        , loaded(false)
    {
    }

    void start()
    {
        if (!isRunning()) {
            setupPins();
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
            resetPins();
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

    const UUID uuid;

  protected:
    virtual void prepare() = 0;
    virtual void cleanup() = 0;
    virtual void resetState() {};

    virtual void setupPins() {};
    virtual void resetPins() {};

    Logger logger;

  private:
    bool loaded;
};
