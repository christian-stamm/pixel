#pragma once
#include "hub75/config.hpp"
#include "hub75/config/panel.hpp"
#include "hub75/renderer/renderer.hpp"
#include "pixutils/buffer.hpp"
#include "pixutils/device/device.hpp"
#include "pixutils/gpio.hpp"
#include "pixutils/system.hpp"
#include "pixutils/types.hpp"
#include "pixutils/watch.hpp"

#include <cmath>
#include <hardware/clocks.h>

class Panel {
  public:
    Panel(const PanelConfig& cfg)
        : Device("Panel")
    {
    }

    inline void clear()
    {
        for (Word led = 0; led < numCols; ++led) {
            shift(0);
        }

        latch();
    }

    inline void shift(Word data) const
    {
        GPIO::setPins(data, rgbMask);
        GPIO::pulsePin(clockPin);
    }

    inline void latch() const
    {
        GPIO::pulsePin(latchPin);
    }

    inline void enable() const
    {
        GPIO::clrPin(powerPin);
    }

    inline void disable() const
    {
        GPIO::setPin(powerPin);
    }

    void setupPins() override
    {
        gpio_init_mask(muxMask);
        gpio_init_mask(rgbMask);
        gpio_init_mask(busMask);

        gpio_set_dir_out_masked(muxMask);
        gpio_set_dir_out_masked(rgbMask);
        gpio_set_dir_out_masked(busMask);

        GPIO::clrPins(rgbMask);
        GPIO::clrPins(muxMask);
        GPIO::clrPin(clockPin);
        GPIO::clrPin(latchPin);
        disable();
    }
};