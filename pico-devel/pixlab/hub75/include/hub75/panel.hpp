#pragma once
#include "hub75/pulser.hpp"
#include "hub75/shifter.hpp"
#include "pixutils/device/device.hpp"
#include "pixutils/device/gpio.hpp"
#include "pixutils/device/pio.hpp"
#include "pixutils/math/matrix.hpp"
#include "pixutils/memory/buffer.hpp"
#include "pixutils/time/watch.hpp"
#include "pixutils/types.hpp"

#include <cmath>
#include <hardware/clocks.h>

#define NUM_BCM_BITS  8
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

        , numLanes(1 << NUM_MUX_PINS)
        , numCols(size)
        , numRows(2 * numLanes)
        , numPixels(numCols * numRows)

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
                  numCols * numLanes * NUM_BCM_BITS,
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

        , input(3 * numPixels)
    {
    }

    void render()
    {
        Matrix8 mat;
        Watch   watch;

        const Word    bcmOffset  = numCols;
        const Word    halfPixels = numCols * numLanes;
        Buffer<Byte>& rgbBuffer  = shifter.getBuffer();

        for (Word pixel = 0; pixel < halfPixels; pixel++) {
            const Word col  = (pixel % numCols);
            const Word lane = (pixel / numCols) % numLanes;

            const Word upperBase = 3 * (col + numCols * lane);
            const Word lowerBase = 3 * (col + numCols * lane + halfPixels);
            const Word bcmBase   = col + (lane * NUM_BCM_BITS) * numCols;

            mat.at(0) = input[upperBase + 0];
            mat.at(1) = input[upperBase + 1];
            mat.at(2) = input[upperBase + 2];
            mat.at(3) = input[lowerBase + 0];
            mat.at(4) = input[lowerBase + 1];
            mat.at(5) = input[lowerBase + 2];

            mat.transpose();

            rgbBuffer[bcmBase + 0 * bcmOffset] = mat.at(0);
            rgbBuffer[bcmBase + 1 * bcmOffset] = mat.at(1);
            rgbBuffer[bcmBase + 2 * bcmOffset] = mat.at(2);
            rgbBuffer[bcmBase + 3 * bcmOffset] = mat.at(3);
            rgbBuffer[bcmBase + 4 * bcmOffset] = mat.at(4);
            rgbBuffer[bcmBase + 5 * bcmOffset] = mat.at(5);
            rgbBuffer[bcmBase + 6 * bcmOffset] = mat.at(6);
            rgbBuffer[bcmBase + 7 * bcmOffset] = mat.at(7);

            // logger.debug() << "INDEX: " << bcmBase + 0 * bcmOffset;
            // logger.debug() << "INDEX: " << bcmBase + 1 * bcmOffset;
            // logger.debug() << "INDEX: " << bcmBase + 2 * bcmOffset;
            // logger.debug() << "INDEX: " << bcmBase + 3 * bcmOffset;
            // logger.debug() << "INDEX: " << bcmBase + 4 * bcmOffset;
            // logger.debug() << "INDEX: " << bcmBase + 5 * bcmOffset;
            // logger.debug() << "INDEX: " << bcmBase + 6 * bcmOffset;
            // logger.debug() << "INDEX: " << bcmBase + 7 * bcmOffset;
            // logger.debug() << "----------------------------------";
        }

        logger.debug() << watch.elapsed();
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

    const uint numLanes;
    const uint numCols;
    const uint numRows;
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

        for (Word led = 0; led < numCols; ++led) {
            Mask bitmask    = (1 << (led % 16));
            bool writeColor = ((value & bitmask) != 0);
            bool writeLatch = ((numCols - configReg) < led);

            GPIO::setPin(latchPin, writeLatch);
            GPIO::setPins(writeColor ? 0xFFFFFFFF : 0x00000000, rgbMask);
            GPIO::pulsePin(clockPin);
        }

        GPIO::clrPin(clockPin);
        GPIO::clrPin(latchPin);
    }

    Shifter shifter;
    Pulser  pulser;

    Buffer<Byte> input;
};