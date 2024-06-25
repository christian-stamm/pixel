#pragma once
#include "driver.hpp"
#include "hub75/config.hpp"
#include "pixutils/buffer.hpp"
#include "pixutils/device/device.hpp"
#include "pixutils/gpio.hpp"
#include "pixutils/system.hpp"
#include "pixutils/types.hpp"

#include <cmath>
#include <hardware/clocks.h>

class Panel : public Device {
  public:
    Panel(const PanelConfig& cfg)
        : Device("Panel")
        , cfg(cfg)
        , frame(Buffer<Word>::build(3 * cfg.numPixels))
        , driver(pio0, cfg, frame)
    {
    }

    void run() const
    {
        while (System::isRunning()) {
            System::sleep(1);
            System::log() << "Idle";
        }
    }

    const PanelConfig cfg;

  protected:
    void prepare() override
    {
        writeRegister(0b0111111111111111, 12); // set max brightness
        writeRegister(0b0000000001000000, 13); // set panel enabled
        blankRegister();

        enable();
        driver.enable();
    }

    void cleanup() override
    {
        driver.disable();
        disable();
    }

  private:
    void setupPins() override
    {
        gpio_init_mask(cfg.busMask);
        gpio_init_mask(cfg.muxMask);
        gpio_init_mask(cfg.rgbMask);
        gpio_set_dir_out_masked(cfg.busMask);
        gpio_set_dir_out_masked(cfg.muxMask);
        gpio_set_dir_out_masked(cfg.rgbMask);

        GPIO::clrPins(cfg.rgbMask);
        GPIO::clrPins(cfg.muxMask);
        GPIO::clrPin(cfg.clockPin);
        GPIO::clrPin(cfg.latchPin);
        disable();
    }

    void resetPins() override
    {
        setupPins();
    }

    inline void shift(Word data) const
    {
        GPIO::setPins(data, cfg.rgbMask);
        GPIO::pulsePin(cfg.clockPin);
    }

    inline void latch() const
    {
        GPIO::pulsePin(cfg.latchPin);
    }

    inline void enable() const
    {
        GPIO::clrPin(cfg.powerPin);
    }

    inline void disable() const
    {
        GPIO::setPin(cfg.powerPin);
    }

    void blankRegister() const
    {
        for (Word led = 0; led < cfg.numCols; ++led) {
            shift(0);
        }

        latch();
    }

    void writeRegister(Word regValue, Word regNumber) const
    {
        GPIO::clrPin(cfg.clockPin);
        GPIO::clrPin(cfg.latchPin);

        for (Word led = 0; led < cfg.numCols; ++led) {
            Mask bitmask    = (1 << (led % 16));
            bool writeColor = ((regValue & bitmask) != 0);
            bool writeLatch = ((cfg.numCols - regNumber) < led);

            GPIO::setPin(cfg.latchPin, writeLatch);
            shift(writeColor ? 0xFFFFFFFF : 0x00000000);
        }

        GPIO::clrPin(cfg.clockPin);
        GPIO::clrPin(cfg.latchPin);
    }

    Buffer<Word> frame;
    Driver       driver;
};