#pragma once
#include "pixutils/gpio.hpp"
#include "pixutils/types.hpp"

#define NUM_RGB_PINS 6
#define NUM_BUS_PINS 3

class PanelConfig {

  public:
    PanelConfig(Word pixWidth, Byte numLanes, Pin pinBase)
        : rgbBase(PIN_WRAP(pinBase))
        , muxBase(PIN_WRAP(pinBase + NUM_RGB_PINS))
        , busBase(PIN_WRAP(pinBase + NUM_RGB_PINS + numLanes))

        , rgbMask(FillMask(NUM_RGB_PINS) << rgbBase)
        , muxMask(FillMask(numLanes) << muxBase)
        , busMask(FillMask(NUM_BUS_PINS) << busBase)

        , clockPin(PIN_WRAP(busBase + 0))
        , latchPin(PIN_WRAP(busBase + 1))
        , powerPin(PIN_WRAP(busBase + 2))

        , numLanes(numLanes)
        , numCols(pixWidth)
        , numRows(2 * numLanes)
        , numPixels(numCols * numRows)
    {
    }

    const Pin rgbBase;
    const Pin muxBase;
    const Pin busBase;

    const Mask rgbMask;
    const Mask muxMask;
    const Mask busMask;

    const Pin clockPin;
    const Pin latchPin;
    const Pin powerPin;

    const uint numLanes;
    const uint numCols;
    const uint numRows;
    const uint numPixels;
};
