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

class Panel : public Device {
  public:
    Panel(const PanelConfig& cfg)
        : Device("Panel")
        , renderer(cfg)
        , receive(Buffer<Word>::build(cfg.numPixels))
        , transmit(Buffer<Byte>::build(cfg.numCols * cfg.numLanes * cfg.bitDepth))
    {
    }

    void run()
    {
        Watch watch;

        while (System::isRunning()) {
            watch.reset();
            renderer.render(receive, transmit);

            logger(INFO) << watch;
            System::sleep(1);
        }
    }

  protected:
    void prepare() override
    {
        writeRegister(0b0111111111111111, 12); // set max brightness
        writeRegister(0b0000000001000000, 13); // set panel enabled
        blankRegister();

        enable();
        renderer.start();
    }

    void cleanup() override
    {
        renderer.stop();
        disable();
    }

  private:
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

    void resetPins() override
    {
        setupPins();
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

    void blankRegister() const
    {
        for (Word led = 0; led < numCols; ++led) {
            shift(0);
        }

        latch();
    }

    void writeRegister(Word regValue, Word regNumber) const
    {
        GPIO::clrPin(clockPin);
        GPIO::clrPin(latchPin);

        for (Word led = 0; led < numCols; ++led) {
            Mask bitmask    = (1 << (led % 16));
            bool writeColor = ((regValue & bitmask) != 0);
            bool writeLatch = ((numCols - regNumber) < led);

            GPIO::setPin(latchPin, writeLatch);
            shift(writeColor ? 0xFFFFFFFF : 0x00000000);
        }

        GPIO::clrPin(clockPin);
        GPIO::clrPin(latchPin);
    }

    Renderer     renderer;
    Buffer<Word> receive;
    Buffer<Byte> transmit;
};