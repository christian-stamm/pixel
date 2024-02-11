#pragma once
#include "pixtools/logger.hpp"
#include "pixtools/types.hpp"
#include "pixtools/utils/watch.hpp"

#include <ios>
#include <iostream>
#include <sstream>
#include <string>

class System {

  public:
    void init(bool waitForConnection = false)
    {
        logger.log() << "Poweron:" << (setup() ? "Success" : "Failure");
    }

    void shutdown()
    {
        logger.log() << "Shutdown:" << (cleanup() ? "Success" : "Failure");
    }

    void standby()
    {
        Watch watch(1.0, true);

        logger.log() << "Standby";

        while (true) {
            spin();

            if (watch.expired()) {
                watch.reset();
                logger.log() << "Heartbeat";
            }
        }
    }

    virtual inline void spin() {}
    virtual inline void sleep(double secs) = 0;

  private:
    virtual bool setup() {}
    virtual bool cleanup() {}

    static Logger logger;
};

inline Logger System::logger = Logger::getLogger("System");
