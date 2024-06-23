// #pragma once
// #include "pixutils/device/device.hpp"
// #include "pixutils/device/gpio.hpp"
// #include "pixutils/system/logger.hpp"

// #include <algorithm>
// #include <hardware/clocks.h>
// #include <hardware/gpio.h>
// #include <hardware/pio.h>
// #include <hardware/pwm.h>
// #include <pico/time.h>

// class PwmDevice : public Device {
//   public:
//     PwmDevice(const Pin& pin)
//         : Device("PWM")
//         , pin(pin)
//         , cycles(1e-6 * clock_get_hz(clk_sys))
//         , slice(pwm_gpio_to_slice_num(pin))
//         , channel(pwm_gpio_to_channel(pin))
//         , pwmConfig(pwm_get_default_config())
//     {
//         pwm_config_set_wrap(&pwmConfig, cycles);
//     }

//     inline void setDutyCycle(float dutyCycle) const
//     {
//         dutyCycle = std::clamp<float>(dutyCycle, 0.0f, 1.0f);
//         pwm_set_gpio_level(pin, dutyCycle * cycles);
//     }

//     const Pin   pin;
//     const float cycles;

//   private:
//     virtual void prepare() override
//     {
//         setupPins();
//         pwm_init(slice, &pwmConfig, true);
//     }

//     virtual void cleanup() override
//     {
//         pwm_set_enabled(slice, false);
//     }

//     void setupPins() const
//     {
//         gpio_set_function(pin, GPIO_FUNC_PWM);
//     }

//     const uint slice;
//     const uint channel;
//     pwm_config pwmConfig;
// };