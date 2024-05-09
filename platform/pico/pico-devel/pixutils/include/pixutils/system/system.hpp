#pragma once
#include "pixutils/system/logger.hpp"
#include "pixutils/time/watch.hpp"
#include "pixutils/types.hpp"

#include <hardware/watchdog.h>
#include <pico/stdio.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>

class System {

  public:
    static void launch()
    {
        logger.info() << "Launch: " << (setup() ? "Success" : "Failure");
    }

    static void shutdown()
    {
        logger.info() << "Shutdown: " << (cleanup() ? "Success" : "Failure");
    }

    static void standby()
    {
        Watch watch(1.0, true);

        logger.info() << "Standby";

        while (true) {
            spin();

            if (watch.expired()) {
                watch.reset();
                logger.debug() << "Heartbeat";
            }
        }
    }

    static inline void spin()
    {
        tight_loop_contents();
    }

    static inline void sleep(double secs)
    {
        if (isRunning()) {
            sleep_us(static_cast<DWord>(1e6 * secs));
        }
    }

    static bool isRunning()
    {
        return running;
    }

    static bool isConnected()
    {
        return stdio_usb_connected();
    }

    static bool isRebooted()
    {
        return watchdog_caused_reboot();
    }

    static Stream log(const LogLevel& level = LogLevel::DEBUG)
    {
        return logger.log(level);
    }

  private:
    static bool setup()
    {
        stdio_init_all();
        running = true;
        return isRunning();
    }

    static bool cleanup()
    {
        running = false;
        return !isRunning();
    }

    static Logger logger;
    static bool   running;
};

inline bool   System::running = false;
inline Logger System::logger  = Logger::getLogger("System");
