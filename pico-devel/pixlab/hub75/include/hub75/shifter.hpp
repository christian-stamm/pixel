#pragma once
#include "hub75/shifter.pio.h"
#include "pixutils/device/dma.hpp"
#include "pixutils/device/gpio.hpp"
#include "pixutils/device/pio.hpp"
#include "pixutils/memory/buffer.hpp"
#include "pixutils/types.hpp"

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>

struct ShiftConfig {
    Pin   rgbBase;
    uint  rgbLanes;
    Pin   clockPin;
    uint  xferBits;
    float xferFreq;
    uint  bufferSize;
};

class Shifter : public PioMachine {

  public:
    Shifter(const PioConfig& piocfg, const ShiftConfig& shiftcfg)
        : PioMachine("Shifter", piocfg)
        , rgbBase(PIN_WRAP(shiftcfg.rgbBase))
        , rgbLanes(shiftcfg.rgbLanes)
        , clockPin(PIN_WRAP(shiftcfg.clockPin))
        , xferBits(shiftcfg.xferBits)
        , xferFreq(shiftcfg.xferFreq)
        , xferTime(shiftcfg.xferBits / shiftcfg.xferFreq)
        , rgbLoader(setupDmaConfig())
        , rgbBuffer(shiftcfg.bufferSize)
    {
        Shifter::configure(shifter_program, setupPioConfig());

        if (xferFreq > clock_get_hz(clk_sys)) {
            throw std::runtime_error("xferFreq must be slower than System Clock.");
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

    inline Buffer<Byte>& getBuffer()
    {
        return rgbBuffer;
    }

    const Pin   rgbBase;
    const uint  rgbLanes;
    const Pin   clockPin;
    const uint  xferBits;
    const float xferFreq;
    const float xferTime;

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

    virtual void autoReload(bool enabled) override
    {
        rgbLoader.enable(enabled);
    }

    void setupPins() const override
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
        logger.debug() << "XFERBITS: " << xferBits;
    }

  private:
    DmaDevice    rgbLoader;
    Buffer<Byte> rgbBuffer;
};