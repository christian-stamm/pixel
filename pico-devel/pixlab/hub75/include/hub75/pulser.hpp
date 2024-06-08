#pragma once
#include "hardware/pio_instructions.h"
#include "hub75/pulser.pio.h"
#include "pixutils/device/dma.hpp"
#include "pixutils/device/gpio.hpp"
#include "pixutils/device/pio.hpp"
#include "pixutils/memory/buffer.hpp"
#include "pixutils/types.hpp"

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <hardware/structs/clocks.h>
#include <pico/time.h>
#include <pico/types.h>
#include <stdexcept>
#include <string>

struct PulseConfig {
    Pin   muxBase;
    uint  muxLanes;
    Pin   latchPin;
    Pin   powerPin;
    uint  numPlanes;
    float pulseLength;
    float pulseScaler;
};

class Pulser : public PioMachine {

  public:
    Pulser(const PioConfig& piocfg, const PulseConfig& plscfg)
        : PioMachine("Pulser", piocfg)
        , muxBase(PIN_WRAP(plscfg.muxBase))
        , muxLanes(plscfg.muxLanes)
        , latchPin(PIN_WRAP(plscfg.latchPin))
        , powerPin(plscfg.powerPin)
        , numPlanes(plscfg.numPlanes)
        , pulseLength(plscfg.pulseLength)
        , pulseScaler(plscfg.pulseScaler)
        , pulseLoader(setupDmaConfig())
    {
        PioMachine::configure(pulser_program, setupPioConfig());

        if ((latchPin + 1) != powerPin) {
            throw std::runtime_error("Power pin must be immediately consecutive to Latch pin");
        }
    }

    void select(const Word line) const
    {
        pio_sm_put(pioID, stmID, line);
    }

    inline void trigger() const
    {
        const volatile void* src  = &pulseList.front();
        volatile void*       dest = &pioID->txf[stmID];
        pulseLoader.transfer(src, dest, pulseList.size());
    }

    const Pin   muxBase;
    const uint  muxLanes;
    const Pin   latchPin;
    const Pin   powerPin;
    const uint  numPlanes;
    const float pulseLength;
    const float pulseScaler;

  protected:
    void setupPulses()
    {
        float cycles = pulseLength * clock_get_hz(clk_sys);
        pulseList.clear();

        for (int plane = 0; plane < numPlanes; plane++) {
            pulseList.push_back(cycles);
            cycles *= pulseScaler;
        }
    }

    virtual pio_sm_config setupPioConfig() const override
    {
        pio_sm_config cfg = pulser_program_get_default_config(address);

        sm_config_set_out_shift(&cfg, true, true, 32);
        sm_config_set_out_pins(&cfg, muxBase, muxLanes);
        sm_config_set_sideset_pins(&cfg, latchPin);

        return cfg;
    }

    DmaConfig setupDmaConfig() const
    {
        const uint dreq = pio_get_dreq(pioID, stmID, true);
        return DmaConfig{dreq, true, false, DmaXferSize::DMA_SIZE_32};
    }

    void setupPins() const override
    {
        for (Pin pin = 0; pin < muxLanes; pin++) {
            pio_gpio_init(pioID, PIN_WRAP(muxBase + pin));
        }

        pio_gpio_init(pioID, latchPin);
        pio_gpio_init(pioID, powerPin);

        pio_sm_set_consecutive_pindirs(pioID, stmID, latchPin, 1, true);
        pio_sm_set_consecutive_pindirs(pioID, stmID, powerPin, 1, true);
        pio_sm_set_consecutive_pindirs(pioID, stmID, muxBase, muxLanes, true);
    }

    void setupRegs() const override
    {
        pio_sm_put(pioID, stmID, numPlanes - 1);
    }

    void autoReload(bool enabled) override
    {
        if (enabled) {
            setupPulses();
        }
        else {
            pulseList.clear();
        }

        pulseLoader.enable(enabled);
    }

  private:
    DmaDevice    pulseLoader;
    Buffer<Word> pulseList;
};