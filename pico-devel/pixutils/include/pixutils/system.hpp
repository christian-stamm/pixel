#pragma once
#include "hardware/gpio.h"
#include "pixutils/logger.hpp"
#include "pixutils/types.hpp"

#include <hardware/watchdog.h>
#include <pico/bootrom.h>
#include <pico/stdio.h>
#include <pico/stdio_usb.h>
#include <pico/time.h>
#include <pico/types.h>

class System {

  public:
    static void launch()
    {
        gpio_init_mask(0xFFFFFFFF);
        gpio_set_dir_all_bits(0xFFFFFFFF);
        gpio_set_mask(0x00000000);

        stdio_init_all();
        running = true;

        logger(INFO) << "Launched";
    }

    static void shutdown()
    {
        running = false;
        logger(INFO) << "Shutdown";
        reset_usb_boot(PICO_DEFAULT_LED_PIN, 0);
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

    static inline double runtime()
    {
        return 1e-6 * to_us_since_boot(get_absolute_time());
    }

    static Stream log(const LogLevel& level = INFO)
    {
        return logger(level);
    }

  private:
    static Logger logger;
    static bool   running;
};

inline bool   System::running = false;
inline Logger System::logger  = Logger::getLogger("System");
