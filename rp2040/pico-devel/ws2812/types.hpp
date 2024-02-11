#pragma once
#include "pixtools/config.hpp"

enum RenderMode {
    LOWER_HALF = (1 << 0),
    UPPER_HALF = (1 << 1),
    FULL       = LOWER_HALF | UPPER_HALF
};

enum Params {
    numPixels,
    laneBasePin,
    portBasePin,
    xferFrequency,
    resetDuration,
};