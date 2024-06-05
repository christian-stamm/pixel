#pragma once
#include "hardware/pio.h"
#include "pixutils/device/device.hpp"
#include "pixutils/device/gpio.hpp"
#include "pixutils/memory/buffer.hpp"
#include "pixutils/system/system.hpp"
#include "pixutils/time/watch.hpp"
#include "pixutils/types.hpp"

#include <hardware/clocks.h>

#define NUM_BCM_BITS  12
#define NUM_RGB_PINS  6
#define NUM_MUX_PINS  5
#define NUM_CTRL_PINS 3

struct PanelConfig {
    uint  size;
    uint  pinBase;
    float xferFreq;
};

class Panel : public Device {
  public:
    Panel(const PanelConfig& panelcfg)
        : Panel(
              panelcfg.size,                   //
              panelcfg.pinBase,                //
              panelcfg.pinBase + NUM_RGB_PINS, //
              panelcfg.pinBase + NUM_RGB_PINS + NUM_MUX_PINS,
              panelcfg.xferFreq //
          )
    {
    }

    Panel(uint size, Pin rgbBase, Pin muxBase, Pin ctrlBase, float xferFreq)
        : Device("Panel")
        , pioID(pio0)

        , rgbBase(rgbBase)
        , muxBase(muxBase)
        , ctrlBase(ctrlBase)

        , rgbMask(FillMask(NUM_RGB_PINS) << rgbBase)
        , muxMask(FillMask(NUM_MUX_PINS) << muxBase)
        , ctrlmask(FillMask(NUM_MUX_PINS) << ctrlBase)

        , clockPin(ctrlBase + 0)
        , latchPin(ctrlBase + 1)
        , powerPin(ctrlBase + 2)

        , numCols(size)
        , numRows(1 << NUM_MUX_PINS)
        , numPixels(size * numRows)

        , xferFreq(xferFreq)
        , xferTime(numCols / xferFreq)
        , buffer(3 * numPixels)
    {
    }

    inline void run()
    {
        while (System::isRunning()) {

            for (Word row = 0; row < numRows; row++) {
                const Word nextRow   = (row + 1) % numRows;
                const Word nextPlane = 0;

                select(row);

                for (Word plane = 1; plane < NUM_BCM_BITS; plane++) {
                    latch();
                    power(true);
                    shift(row, plane);
                    power(false);
                }

                latch();
                power(true);
                shift(nextRow, nextPlane);
                power(false);
            }
        }
    }

    const PIO pioID;

    const Pin rgbBase;
    const Pin muxBase;
    const Pin ctrlBase;

    const Mask rgbMask;
    const Mask muxMask;
    const Mask ctrlmask;

    const Pin clockPin;
    const Pin latchPin;
    const Pin powerPin;

    const uint numCols;
    const uint numRows;
    const uint numPixels;

    const float xferFreq;
    const float xferTime;

  protected:
    void prepare() override
    {
        setupPins();
        // unlockPanel();
    }

    void cleanup() override {}

  private:
    void setupPins() const
    {
        gpio_init_mask(ctrlmask);
        gpio_init_mask(muxMask);
        gpio_init_mask(rgbMask);
        gpio_set_dir_out_masked(ctrlmask);
        gpio_set_dir_out_masked(muxMask);
        gpio_set_dir_out_masked(rgbMask);
    }

    void unlockPanel() const
    {
        const DByte c11 = 0b0111111111111111; // full bright
        const DByte c12 = 0b0000000001000000; // panel on.

        writeRegister(c11, 12);
        writeRegister(c12, 13);
    }

    void writeRegister(Word value, Word configReg) const
    {
        GPIO::clrPin(clockPin);
        GPIO::clrPin(latchPin);
        std::bitset<16> configBits(value);

        for (Word led = 0; led < numCols; ++led) {
            Word position   = 16 - (led % 16) - 1;
            bool writeColor = configBits.test(position);
            bool writeLatch = ((numCols - configReg) < led);

            GPIO::setPin(latchPin, writeLatch);
            GPIO::setPins(writeColor ? rgbMask : 0x0, rgbMask);
            GPIO::pulsePin(clockPin);
        }

        GPIO::clrPin(clockPin);
        GPIO::clrPin(latchPin);
    }

    inline void power(bool enabled) const
    {
        GPIO::setPin(powerPin, !enabled);
    }

    inline void select(const Byte& line) const
    {
        GPIO::setPins(line << muxBase, muxMask);
    }

    inline void latch() const
    {
        GPIO::pulsePin(latchPin);
    }

    inline void shift(Word line, Word plane)
    {
        const Word length  = (2 * 3 * numCols);
        const Word source  = (line + 0) * length;
        const Word target  = (line + 1) * length;
        const Word bitMask = BitMask(plane);

        for (Word index = source; index < target; index += 6) {
            Word rgb = 0;

            rgb |= ((buffer[index + 0] & bitMask) != 0) << 0; // r1
            rgb |= ((buffer[index + 1] & bitMask) != 0) << 1; // g1
            rgb |= ((buffer[index + 2] & bitMask) != 0) << 2; // b1
            rgb |= ((buffer[index + 3] & bitMask) != 0) << 3; // r2
            rgb |= ((buffer[index + 4] & bitMask) != 0) << 4; // g2
            rgb |= ((buffer[index + 5] & bitMask) != 0) << 5; // b2

            GPIO::setPins(rgb << rgbBase, rgbMask);
            GPIO::pulsePin(clockPin);
        }
    }

    Buffer<Byte> buffer;
};