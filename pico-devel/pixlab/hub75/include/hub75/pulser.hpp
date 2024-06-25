#pragma once
#include "config.hpp"
#include "hub75/pulser.pio.h"
#include "pixutils/buffer.hpp"
#include "pixutils/device/dma.hpp"
#include "pixutils/device/pio.hpp"
#include "pixutils/gpio.hpp"
#include "pixutils/types.hpp"

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <hardware/structs/clocks.h>
#include <pico/time.h>
#include <pico/types.h>

class Pulser : public PioMachine {

  public:
    Pulser(const PioConfig& piocfg, const PanelConfig& pnlcfg)
        : PioMachine("Pulser", piocfg)
        , cfg(pnlcfg)
        , pulseLoader(setupDmaConfig())
        , pulseBuffer(setupPulseBuffer())
    {
        PioMachine::configure(pulser_program, setupPioConfig());

        if ((cfg.latchPin + 1) != cfg.powerPin) {
            throw std::runtime_error(std::format(
                "Power pin ({}) must be immediately consecutive to Latch pin ({})", cfg.latchPin, cfg.powerPin));
        }
    }

    inline void trigger() const
    {
        const volatile void* src  = &pulseBuffer.front();
        volatile void*       dest = &pioID->txf[stmID];
        pulseLoader.transfer(src, dest, pulseBuffer.size());
    }

    const PanelConfig cfg;

  protected:
    static Buffer<Word> setupPulseBuffer()
    {
        return Buffer<Word>::build(0);
    }

    virtual pio_sm_config setupPioConfig() const override
    {
        pio_sm_config piocfg = pulser_program_get_default_config(address);

        sm_config_set_out_shift(&piocfg, true, true, 32);
        sm_config_set_out_pins(&piocfg, cfg.muxBase, cfg.numLanes);
        sm_config_set_sideset_pins(&piocfg, cfg.latchPin);

        return piocfg;
    }

    DmaConfig setupDmaConfig() const
    {
        const uint dreq = pio_get_dreq(pioID, stmID, true);
        return DmaConfig{dreq, true, false, DmaXferSize::DMA_SIZE_32};
    }

    void setupPins() override
    {
        for (Pin pin = 0; pin < cfg.numLanes; pin++) {
            pio_gpio_init(pioID, PIN_WRAP(cfg.muxBase + pin));
        }

        pio_gpio_init(pioID, cfg.latchPin);
        pio_gpio_init(pioID, cfg.powerPin);

        pio_sm_set_consecutive_pindirs(pioID, stmID, cfg.latchPin, 1, true);
        pio_sm_set_consecutive_pindirs(pioID, stmID, cfg.powerPin, 1, true);
        pio_sm_set_consecutive_pindirs(pioID, stmID, cfg.muxBase, cfg.numLanes, true);
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
            pulseList.reset();
        }

        pulseLoader.start(enabled);
    }

  private:
    DmaDevice    pulseLoader;
    Buffer<Word> pulseBuffer;
};