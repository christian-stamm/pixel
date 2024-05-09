#pragma once
#include "pixutils/system/logger.hpp"
#include "pixutils/system/system.hpp"
#include "pixutils/types.hpp"

#include <bitset>
#include <hardware/gpio.h>

#define UART_MASK (0x00000003)
#define GPIO_MASK (0x1FFFFFFF & ~UART_MASK)

#define PIN_MASK(x) (static_cast<Word>(1 << x))

class GPIO {
  public:
    static inline void setPin(Word pin, bool enabled = true)
    {
        setPins(enabled << pin, 1 << pin);
    }

    static inline void clrPin(Word pin)
    {
        setPin(pin, false);
    }

    static inline void pulsePin(Word pin, Word numPulses = 1, bool invPulse = false)
    {
        for (Word pulse = 0; pulse < numPulses; pulse++) {
            setPin(pin, !invPulse);
            setPin(pin, invPulse);
        }
    }

    static inline void setPins(Word pins, Word mask = GPIO_MASK)
    {
        gpio_put_masked(mask & GPIO_MASK, pins);
        System::sleep(100e-9);
    }

    static inline void clrPins(Word pins, Word mask = GPIO_MASK)
    {
        setPins(~pins, mask & pins);
    }

    static inline bool getPin(Word pin)
    {
        return gpio_get(pin);
    }

    static inline Word getPins()
    {
        return gpio_get_all();
    }

    static inline void status()
    {
        logger.debug() << "PinStatus: 0b" << std::bitset<32>(getPins());
    }

  private:
    static Logger logger;
};

inline Logger GPIO::logger = Logger::getLogger("GPIO");