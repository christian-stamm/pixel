#pragma once
#include "hardware/gpio.h"
#include "pixutils/system/logger.hpp"
#include "pixutils/time/time.hpp"
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
        Watch watch(usDuration(1e6), true);

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

    static inline bool isRunning()
    {
        return running;
    }

    static inline bool isConnected()
    {
        return stdio_usb_connected();
    }

    static inline bool isRebooted()
    {
        return watchdog_caused_reboot();
    }

    static inline Stream log(const LogLevel& level = LogLevel::DEBUG)
    {
        return logger.log(level);
    }

  private:
    static bool setup()
    {
        gpio_init_mask(0xFFFFFFFF);
        gpio_set_dir_all_bits(0xFFFFFFFF);
        gpio_set_mask(0x00000000);

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
