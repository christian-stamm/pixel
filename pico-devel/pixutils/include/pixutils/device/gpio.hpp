// #pragma once
// #include "pixutils/system/logger.hpp"
// #include "pixutils/types.hpp"

// #include <format>
// #include <hardware/gpio.h>

// #define UART_MASK (0x00000003)
// #define GPIO_MASK (0x1FFFFFFF & ~UART_MASK)

// #define PIN_MASK(x) (BitMask(x))
// #define PIN_WRAP(x) (x % 32)

// class GPIO {
//   public:
//     static inline void setPin(Pin pin, bool enabled = true)
//     {
//         setPins(enabled << pin, 1 << pin);
//     }

//     static inline void clrPin(Pin pin)
//     {
//         setPin(pin, false);
//     }

//     static inline void pulsePin(Pin pin, bool invPulse = false)
//     {
//         setPin(pin, !invPulse);
//         setPin(pin, invPulse);
//     }

//     static inline void setPins(Pin pins, Mask mask = GPIO_MASK)
//     {
//         gpio_put_masked(mask & GPIO_MASK, pins);
//     }

//     static inline void clrPins(Pin pins, Mask mask = GPIO_MASK)
//     {
//         setPins(~pins, mask & pins);
//     }

//     static inline bool getPin(Pin pin)
//     {
//         return gpio_get(pin);
//     }

//     static inline Mask getPins()
//     {
//         return gpio_get_all();
//     }

//     static inline Message status()
//     {
//         return Message().extend("PinStatus: {:b}", getPins());
//     }

//   private:
//     static Logger logger;
// };

// inline Logger GPIO::logger = Logger::getLogger("GPIO");