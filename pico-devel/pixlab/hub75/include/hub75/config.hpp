#pragma once
#include "hardware/pio.h"
#include "pixutils/config.hpp"
#include "pixutils/types.hpp"

#define NUM_RGB_PINS   6
#define NUM_MUX_PINS   5
#define NUM_BUS_PINS   3
#define NUM_BIT_PLANES 10

class PanelConfig : Config {

  public:
    static Config build(Word pixWidth, Pin pinBase)
    {
        Config pixCfg;
        pixCfg.set<Word>("numCols", pixWidth);
        pixCfg.set<Word>("numRows", NUM_MUX_PINS << 1);
        pixCfg.set<Word>("bitDepth", NUM_BIT_PLANES);

        Config pinCfg;
        pinCfg.set<Pin>("rgbBase", pinBase);
        pinCfg.set<Pin>("muxBase", pinBase + NUM_RGB_PINS);
        pinCfg.set<Pin>("busBase", pinBase + NUM_RGB_PINS + NUM_MUX_PINS);

        Config renderCfg;
        renderCfg.set<PIO>("pioID", pio0);
        renderCfg.set<Address>("pioBase", 0);
        renderCfg.set<Byte>("numWorker", 4);

        Config streamCfg;
        streamCfg.set<PIO>("pioID", pio1);
        streamCfg.set<Address>("pioBase", 0);

        Config defaultCfg;
        defaultCfg.set<Config>("pixConfig", pixCfg);
        defaultCfg.set<Config>("pinConfig", pinCfg);
        defaultCfg.set<Config>("renderConfig", renderCfg);
        defaultCfg.set<Config>("streamConfig", streamCfg);

        return defaultCfg;
    }

    static Config getDefault()
    {
        return build(64, 2);
    }

    static bool validate(const Config& config)
    {
        return true;
    }
};
