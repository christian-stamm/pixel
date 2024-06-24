#pragma once
#include "pixutils/logger.hpp"

#include <string>

class Device {
  public:
    Device(const std::string& name)
        : logger(Logger::getLogger(name))
        , enabled(false)
    {
    }

    void enable(bool enabled = true)
    {
        const bool modifyState = (enabled != isEnabled());

        if (modifyState) {
            this->enabled = enabled;

            if (enabled) {
                prepare();
            }
            else {
                cleanup();
            }

            logger(DEBUG) << "Device " << (enabled ? "enabled" : "disabled") << ".";
        }
    }

    void disable()
    {
        enable(false);
        reset();
    }

    void restart()
    {
        disable();
        enable();
    }

    bool isEnabled() const
    {
        return enabled;
    }

    virtual void reset() {}

  protected:
    virtual void prepare() = 0;
    virtual void cleanup() = 0;

    Logger logger;

  private:
    bool enabled;
};
