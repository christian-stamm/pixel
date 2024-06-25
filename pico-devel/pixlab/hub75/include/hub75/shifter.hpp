#pragma once
#include "config.hpp"
#include "hub75/shifter.pio.h"
#include "pixutils/buffer.hpp"
#include "pixutils/device/dma.hpp"
#include "pixutils/device/pio.hpp"
#include "pixutils/gpio.hpp"
#include "pixutils/logger.hpp"
#include "pixutils/types.hpp"

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>

class Shifter : public PioMachine {

  public:
    Shifter(const PioConfig& piocfg, const PanelConfig& pnlcfg, Buffer<Word>& frame)
        : PioMachine("Shifter", piocfg)

        , rgbLoader(setupDmaConfig())
        , rgbBuffer(Buffer<Byte>::build(shiftcfg.bufferSize))
    {
        Shifter::configure(shifter_program, setupPioConfig());

        const Word sysclk = clock_get_hz(clk_sys);

        if (sysclk < xferFreq) {
            throw std::runtime_error(std::format("xferFreq {} must be slower than System Clock {}.", xferFreq, sysclk));
        }
    }

    inline bool busy() const
    {
        return rgbLoader.busy();
    }

    inline void trigger() const
    {
        const volatile void* src  = &rgbBuffer.front();
        volatile void*       dest = &pioID->txf[stmID];
        rgbLoader.transfer(src, dest, rgbBuffer.size());
    }

  protected:
    DmaConfig setupDmaConfig() const
    {
        const uint dreq = pio_get_dreq(pioID, stmID, true);
        return DmaConfig{dreq, true, false, DmaXferSize::DMA_SIZE_8};
    }

    virtual pio_sm_config setupPioConfig() const override
    {
        pio_sm_config cfg = shifter_program_get_default_config(address);

        sm_config_set_clkdiv(&cfg, clock_get_hz(clk_sys) / (2.0 * xferFreq));
        sm_config_set_out_shift(&cfg, true, true, 8);
        sm_config_set_out_pins(&cfg, rgbBase, rgbLanes);
        sm_config_set_sideset_pins(&cfg, clockPin);

        return cfg;
    }

    void setupPins() override
    {
        pio_gpio_init(pioID, clockPin);

        for (Word pin = 0; pin < rgbLanes; pin++) {
            pio_gpio_init(pioID, PIN_WRAP(rgbBase + pin));
        }

        pio_sm_set_consecutive_pindirs(pioID, stmID, clockPin, 1, true);
        pio_sm_set_consecutive_pindirs(pioID, stmID, rgbBase, rgbLanes, true);
    }

    void setupRegs() const override
    {
        pio_sm_put(pioID, stmID, xferBits - 1);
        logger(DEBUG) << "XFERBITS: " << xferBits;
    }

  private:
    DmaDevice    rgbLoader;
    Buffer<Byte> rgbBuffer;
};