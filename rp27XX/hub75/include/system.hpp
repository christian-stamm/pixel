#pragma once
#include "utils/logger.hpp"

#include <signal.h>
#include <stdlib.h>
#include <thread>
#include <unistd.h>

class System {

  public:
    static void init(bool waitForConnection = false)
    {
        if (!isRunning()) {
            running = true;
            logger.log() << "Launching...";
        }

        signal(SIGINT, System::shutdown);
        signal(SIGTERM, System::shutdown);
    }

    static void shutdown(int code = EXIT_SUCCESS)
    {
        if (isRunning()) {
            running = false;
            logger.log() << "Shutdown...";
        }

        exit(code);
    }

    inline static void sleep(double secs)
    {
        if (isRunning()) {
            std::this_thread::sleep_for(std::chrono::microseconds(static_cast<uint64_t>(1e6 * secs)));
        }
    }

    static bool isRunning()
    {
        return running;
    }

  private:
    static Logger logger;
    static bool   running;
};

inline bool   System::running = false;
inline Logger System::logger  = Logger::getLogger("System");
