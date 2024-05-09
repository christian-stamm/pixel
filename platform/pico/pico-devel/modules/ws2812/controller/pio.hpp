#pragma once
#include "pixdriver/ws2812/pio/io.pio.h"
#include "pixtools/dev/device.hpp"
#include "pixtools/dev/dma.hpp"
#include "pixtools/mem/buffer.hpp"
#include "pixtools/types.hpp"

#include <hardware/clocks.h>
#include <hardware/pio.h>
#include <pico/time.h>

class PixelDriver : public Device {
  public:
    PixelDriver(const Config& config)
        : Device("PIO", config)
        , pioInstance(pio0)
        , smInstance(0)
        , memAddress(0)
        , laneBasePin(config.get<uint>("laneBasePin"))
        , portBasePin(config.get<uint>("portBasePin"))
        , dmaDev(configureDMA())
        , pioCFG(ws2812_program_get_default_config(memAddress))
    {
        const uint  cycles_per_bit = ws2812_T1 + ws2812_T2 + ws2812_T3;
        const float xferFreq       = config.get<float>("xferFrequency");
        const float sysclk         = clock_get_hz(clk_sys);
        const float clkdiv         = sysclk / (xferFreq * cycles_per_bit);

        sm_config_set_clkdiv(&pioCFG, clkdiv);
        sm_config_set_out_shift(&pioCFG, true, true, 32);
        sm_config_set_in_shift(&pioCFG, true, true, 32);
        sm_config_set_fifo_join(&pioCFG, PIO_FIFO_JOIN_TX);
        sm_config_set_out_pins(&pioCFG, laneBasePin, ws2812_NUM_LANES);
        sm_config_set_set_pins(&pioCFG, laneBasePin, ws2812_NUM_LANES);
        sm_config_set_sideset_pins(&pioCFG, portBasePin);
    }

    virtual void load() override
    {
        configurePins();

        pio_sm_claim(pioInstance, smInstance);
        pio_add_program_at_offset(pioInstance, &ws2812_program, memAddress);
        pio_sm_init(pioInstance, smInstance, memAddress, &pioCFG);
        pio_sm_set_enabled(pioInstance, smInstance, true);

        dmaDev.load();

        Device::load();
    }

    virtual void unload() override
    {
        dmaDev.unload();

        pio_sm_set_enabled(pioInstance, smInstance, false);
        pio_sm_clear_fifos(pioInstance, smInstance);
        pio_clear_instruction_memory(pioInstance);
        pio_sm_unclaim(pioInstance, smInstance);

        Device::unload();
    }

    void flush(const SubBuffer<Word>& buffer) const
    {
        const Word     length = buffer.size();
        volatile void* src    = &buffer.front();
        volatile void* dest   = &pioInstance->txf[smInstance];

        // logger.log() << "Flushing" << length << "bits.";
        dmaDev.transfer(src, dest, length);
    }

    const PIO  pioInstance;
    const uint smInstance;
    const uint memAddress;
    const uint laneBasePin;
    const uint portBasePin;

  private:
    void configurePins() const
    {

        for (uint pin = laneBasePin; pin < laneBasePin + ws2812_NUM_LANES; pin++) {
            pio_gpio_init(pioInstance, pin % 32);
        }

        for (uint pin = portBasePin; pin < portBasePin + ws2812_NUM_PORTS; pin++) {
            pio_gpio_init(pioInstance, pin % 32);
        }

        pio_sm_set_consecutive_pindirs(pioInstance, smInstance, laneBasePin, ws2812_NUM_LANES, true);
        pio_sm_set_consecutive_pindirs(pioInstance, smInstance, portBasePin, ws2812_NUM_PORTS, true);
    }

    DmaController configureDMA()
    {
        const uint dreq = pio_get_dreq(pioInstance, smInstance, true);
        return DmaController(config, dreq, true, false, DmaXferSize::DMA_SIZE_32);
    }

    DmaController dmaDev;
    pio_sm_config pioCFG;
};