#pragma once
#include "hardware/pio.h"
#include "pixutils/device/pio/machine.hpp"
#include "pixutils/types.hpp"

#define NUM_RGB_PINS 6
#define NUM_MUX_PINS 5
#define NUM_CTL_PINS 3

#define NUM_WORKER 4

struct Config {

  public:
    Word numCols;
    Word numRows;
    Byte bitDepth;
    Word numPixels;

    Pin rgbBase;
    Pin muxBase;
    Pin ctlBase;

    Pin clockPin;
    Pin latchPin;
    Pin powerPin;

    Mask rgbMask;
    Mask muxMask;
    Mask ctlMask;

    Byte numLanes;
    Byte numWorker;

    PioMachineConfig workerConfig;

    Config(
        Word numCols, Word numRows, Byte bitDepth, Pin pinBase, Byte numWorker = 4, PIO workerPID = pio0,
        Byte workerBase = 0, PioMachineConfig workerConfig)
        : numCols(numCols)
        , numRows(numRows)
        , bitDepth(bitDepth)
        , numPixels(numCols * numRows)

        , rgbBase(PIN_WRAP(pinBase + 0))
        , muxBase(PIN_WRAP(pinBase + NUM_RGB_PINS))
        , ctlBase(PIN_WRAP(pinBase + NUM_RGB_PINS + NUM_MUX_PINS))

        , clockPin(PIN_WRAP(ctlBase + 0))
        , latchPin(PIN_WRAP(ctlBase + 1))
        , powerPin(PIN_WRAP(ctlBase + 2))

        , rgbMask(FILL_MASK(NUM_RGB_PINS) << rgbBase)
        , muxMask(FILL_MASK(NUM_MUX_PINS) << muxBase)
        , ctlMask(FILL_MASK(NUM_CTL_PINS) << ctlBase)

        , numLanes(1 << NUM_MUX_PINS)
        , numWorker(NUM_WORKER)
        , workerPID(workerPID)
        , workerBase(workerBase)
    {
    }
};
