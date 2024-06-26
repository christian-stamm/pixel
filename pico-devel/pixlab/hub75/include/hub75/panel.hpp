#pragma once
#include "hub75/renderer.hpp"
#include "pixutils/buffer.hpp"
#include "pixutils/config.hpp"
#include "pixutils/device/device.hpp"
#include "pixutils/gpio.hpp"
#include "pixutils/system.hpp"
#include "pixutils/types.hpp"
#include "pixutils/watch.hpp"

#include <cmath>
#include <hardware/clocks.h>

class Panel : public Device {
  public:
    static Panel build(const Config& config)
    {
        const Config pixCfg = config.get<Config>("pixConfig");
        const Config pinCfg = config.get<Config>("pinConfig");

        return Panel(
            config,                           //
            pixCfg.get<Word>("numCols", 64),  //
            pixCfg.get<Word>("numRows", 64),  //
            pixCfg.get<Word>("bitDepth", 10), //

            pinCfg.get<Pin>("rgbBase", 2), //
            pinCfg.get<Pin>("muxBase", 8), //
            pinCfg.get<Pin>("busBase", 13) //
        );
    }

    void run()
    {
        Watch watch;

        while (System::isRunning()) {
            watch.reset();
            renderer.render(input, output);

            logger(INFO) << watch;
            System::sleep(1);
        }
    }

    const Word numLanes;
    const Word bitDepth;
    const Word numCols;
    const Word numRows;
    const Word numPixels;

    const Pin rgbBase;
    const Pin muxBase;
    const Pin busBase;

    const Pin clockPin;
    const Pin latchPin;
    const Pin powerPin;

    const Mask rgbMask;
    const Mask muxMask;
    const Mask busMask;

  protected:
    Panel(
        const Config& config,                                           //
        const Word& numCols, const Word& numRows, const Word& bitDepth, //
        const Pin& rgbBase, const Pin& muxBase, const Pin& busBase      //
        )
        : Device("Panel")
        , numLanes(NUM_MUX_PINS)
        , bitDepth(bitDepth)
        , numCols(numCols)
        , numRows(numRows)
        , numPixels(numCols * numRows)

        , rgbBase(PIN_WRAP(rgbBase))
        , muxBase(PIN_WRAP(muxBase))
        , busBase(PIN_WRAP(busBase))

        , clockPin(PIN_WRAP(busBase + 0))
        , latchPin(PIN_WRAP(busBase + 1))
        , powerPin(PIN_WRAP(busBase + 2))

        , rgbMask(FILL_MASK(NUM_RGB_PINS) << rgbBase)
        , muxMask(FILL_MASK(NUM_MUX_PINS) << muxBase)
        , busMask(FILL_MASK(NUM_BUS_PINS) << busBase)

        , input(Buffer<Word>::build(numPixels))
        , output(Buffer<Byte>::build(numCols * numLanes * bitDepth))
        , renderer(Renderer::build(config))
    {
    }

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

    Buffer<Word> input;
    Buffer<Byte> output;
    Renderer     renderer;
};