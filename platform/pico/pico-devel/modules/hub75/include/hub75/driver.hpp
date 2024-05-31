#pragma once
#include "hub75/panel.hpp"
#include "hub75/pixel.hpp"
#include "pixutils/device/device.hpp"
#include "pixutils/memory/buffer.hpp"
#include "pixutils/types.hpp"

#include <cmath>
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <pico/time.h>
#include <vector>

class Driver : public Device {
  public:
    Driver(const PanelConfig& cfg)
        : Device("Driver")
        , panel(cfg)
        , buffer(panel.getFramebuffer())
        , line(0)
    {
    }

    void setPixel(Word col, Word row, Word red, Word green, Word blue)
    {

        const uint range = 6 * 64 * 12;

        for (int row = 0; row < panel.numRows; row++) {
            Byte color = 0;

            if (row % 3 == 0) {
                color = BitMask(0) | BitMask(3) | BitMask(4);
            }
            else if (row % 3 == 1) {
                color = BitMask(1) | BitMask(4) | BitMask(5);
            }
            else if (row % 3 == 2) {
                color = BitMask(2) | BitMask(5) | BitMask(3);
            }

            for (Byte& v : buffer.subrange(row * range, range)) {
                v = color;
            }
        }
    }

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

    Buffer<Byte>& buffer;

    repeating_timer_t timer;
};