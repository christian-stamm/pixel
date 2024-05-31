#pragma once
#include "hub75/panel.hpp"
#include "pixutils/device/device.hpp"
#include "pixutils/types.hpp"

#include <cmath>
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <pico/time.h>

class Driver : public Device {
  public:
    Driver(const PanelConfig& cfg)
        : Device("Driver")
        , panel(cfg)
        , line(0)
    {
    }

    void setPixel(Word col, Word row, Word red, Word green, Word blue) {}

  protected:
    virtual void prepare() override
    {
        panel.enable();
        add_repeating_timer_us(270, Driver::trigger, static_cast<void*>(this), &timer);
    }

    virtual void cleanup() override
    {
        cancel_repeating_timer(&timer);
        panel.disable();
    }

  private:
    static bool trigger(repeating_timer_t* rt)
    {
        static_cast<Driver*>(rt->user_data)->update();
        return true;
    }

    inline void update()
    {
        panel.dump(line);

        line += 1;
        line %= panel.numRows;
    }

    inline void unroll(Word offset, Word color) {}

    Panel panel;
    Word  line;

    repeating_timer_t timer;
};