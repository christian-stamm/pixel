#pragma once
#include "hardware/pio_instructions.h"
#include "hub75/shifter.pio.h"
#include "pixutils/device/gpio.hpp"
#include "pixutils/device/pio.hpp"
#include "pixutils/types.hpp"

#include <cstdio>
#include <hardware/pio.h>

struct ShiftConfig {
    Pin  rgbBase;
    uint rgbLanes;
    Pin  clockPin;
};

class Shifter : public PioMachine {

  public:
    Shifter(const PioConfig& piocfg, const ShiftConfig& shiftcfg)
        : PioMachine("Shifter", piocfg)
        , rgbBase(PIN_WRAP(shiftcfg.rgbBase))
        , rgbLanes(shiftcfg.rgbLanes)
        , clockPin(PIN_WRAP(shiftcfg.clockPin))
    {
        Shifter::configure(shifter_program, setupPioConfig());
    }

    inline void shift(const Byte* base, const Byte& bit) const
    {
        Word cmd = 0;

        if (bit == 0) {
            cmd = pio_encode_pull(false, false);
        }
        else {
            cmd = pio_encode_out(pio_isr, bit);
        }

        cmd |= pio_encode_sideset(1, 0);

        modify(shifter_offset_loop, cmd);
        pio_sm_put_blocking(pioID, stmID, *(base + 0));
        pio_sm_put_blocking(pioID, stmID, *(base + 1));
        pio_sm_put_blocking(pioID, stmID, *(base + 2));
        pio_sm_put_blocking(pioID, stmID, *(base + 3));
        pio_sm_put_blocking(pioID, stmID, *(base + 4));
        pio_sm_put_blocking(pioID, stmID, *(base + 5));
    }

    const Pin  rgbBase;
    const uint rgbLanes;
    const Pin  clockPin;

  protected:
    virtual pio_sm_config setupPioConfig() const override
    {
        pio_sm_config cfg = shifter_program_get_default_config(address);

        sm_config_set_out_shift(&cfg, true, true, 8);
        sm_config_set_out_pins(&cfg, rgbBase, rgbLanes);
        sm_config_set_sideset_pins(&cfg, clockPin);
        sm_config_set_fifo_join(&cfg, PIO_FIFO_JOIN_TX);

        return cfg;
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
};