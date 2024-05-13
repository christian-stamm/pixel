#pragma once
#include "hardware/pio.h"
#include "hub75/pulser.hpp"
#include "hub75/shifter.hpp"
#include "pixutils/device/device.hpp"
#include "pixutils/device/gpio.hpp"
#include "pixutils/device/pio.hpp"
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

        , numLines(1 << NUM_MUX_PINS)
        , numPixels(size * numLines)

        , shifter(
              {
                  pioID,
                  0,
                  0,
                  false,
              },
              {
                  rgbBase,
                  NUM_RGB_PINS,
                  clockPin,
                  size,
                  xferFreq,
                  numPixels * NUM_BCM_BITS,
              })
        , pulser(
              {
                  pioID,
                  1,
                  16,
                  false,
              },
              {
                  muxBase,
                  NUM_MUX_PINS,
                  latchPin,
                  powerPin,
                  NUM_BCM_BITS,
                  1e-6,
                  1.45f,
              })

    {
    }

    inline void dump(Word line) const
    {
        const bool newFrame = line == 0;

        pulser.select(line);

        if (newFrame) {
            shifter.trigger();
        }

        pulser.trigger();
    }

    inline Buffer<Byte>& getFramebuffer()
    {
        return shifter.getBuffer();
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

    const uint numLines;
    const uint numPixels;

  protected:
    void prepare() override
    {
        unlockPanel();
        shifter.enable();
        pulser.enable();

        PioMachine::run(pioID, 0b1111);
    }

    void cleanup() override
    {
        PioMachine::run(pioID, 0b0000);

        shifter.disable();
        pulser.disable();
    }

  private:
    void unlockPanel() const
    {
        gpio_init_mask(ctrlmask);
        gpio_init_mask(muxMask);
        gpio_init_mask(rgbMask);
        gpio_set_dir_out_masked(ctrlmask);
        gpio_set_dir_out_masked(muxMask);
        gpio_set_dir_out_masked(rgbMask);

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

        for (Word led = 0; led < shifter.xferBits; ++led) {
            Word position   = 16 - (led % 16) - 1;
            bool writeColor = configBits.test(position);
            bool writeLatch = ((shifter.xferBits - configReg) < led);

            GPIO::setPin(latchPin, writeLatch);
            GPIO::setPins(writeColor ? 0xFFFFFFFF : 0x00000000, rgbMask);
            GPIO::pulsePin(clockPin);
        }

        GPIO::clrPin(clockPin);
        GPIO::clrPin(latchPin);
    }

    Shifter shifter;
    Pulser  pulser;
};