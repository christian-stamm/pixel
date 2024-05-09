#pragma once
#include "hardware/platform_defs.h"
#include "hub75/driver.pio.h"
#include "pico/types.h"
#include "pixutils/device/device.hpp"

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <pico/time.h>
#include <stdexcept>

#define NUM_RGB_PINS 6
#define NUM_MUX_PINS 5

const uint OFFSETS[4] = {
    static_cast<uint>((0 * dumper_program.length) + (0 * shifter_program.length)), //
    static_cast<uint>((1 * dumper_program.length) + (0 * shifter_program.length)), //
    static_cast<uint>((1 * dumper_program.length) + (1 * shifter_program.length)), //
    static_cast<uint>((1 * dumper_program.length) + (2 * shifter_program.length)), //
};

class Machine : public Device {
  public:
    Machine(
        const PIO&  pioID,   //
        const uint& stmID,   //
        const uint& basePin, //
        const uint& address  //
        )
        : Device("Machine")
        , pioID(pioID)
        , stmID(stmID)
        , basePin(basePin)
        , address(address)
        , config(pio_get_default_sm_config())
        , program({nullptr, 0, 0})
    {
    }

    void setup(const pio_program& program, const pio_sm_config& config)
    {
        this->config  = config;
        this->program = program;
    }

    virtual void load() override
    {
        logger.debug() << "MACHINE LOAD";

        pio_sm_claim(pioID, stmID);
        pio_add_program_at_offset(pioID, &program, address);
        pio_sm_init(pioID, stmID, address, &config);

        Device::load();
    }

    virtual void unload() override
    {
        pio_sm_clear_fifos(pioID, stmID);
        pio_remove_program(pioID, &program, address);
        pio_sm_unclaim(pioID, stmID);

        Device::unload();
    }

    const PIO  pioID;
    const uint stmID;
    const uint basePin;
    const uint address;

  protected:
    virtual pio_sm_config buildConfig() = 0;

  private:
    pio_sm_config config;
    pio_program   program;
};

class ShiftMachine : public Machine {

  public:
    ShiftMachine(const PIO& pioID, const uint& stmID, const uint& address, const uint& basePin)
        : Machine(pioID, stmID, basePin, address)
    {
        setup(shifter_program, buildConfig());
    }

    virtual pio_sm_config buildConfig() override
    {
        logger.debug() << "SHIFT CONFIG";

        pio_sm_config cfg = shifter_program_get_default_config(address);

        sm_config_set_out_shift(&cfg, true, true, 32);
        sm_config_set_in_shift(&cfg, true, true, 32);
        sm_config_set_out_pins(&cfg, basePin, 2);
        sm_config_set_fifo_join(&cfg, PIO_FIFO_JOIN_TX);

        return cfg;
    }
};

class DumpMachine : public Machine {

  public:
    DumpMachine(
        const PIO&   pioID,    //
        const uint&  stmID,    //
        const uint&  address,  //
        const uint&  clockPin, //
        const uint&  xferBits, //
        const float& xferFreq  //
        )
        : Machine(pioID, stmID, clockPin, address)
        , xferBits(xferBits)
        , xferFreq(xferFreq)
    {
        setup(dumper_program, buildConfig());

        if (xferBits < 1) {
            throw std::runtime_error("xferBits must be greater than zero.");
        }

        if (xferFreq > clock_get_hz(clk_sys)) {
            throw std::runtime_error("xferFreq must be slower than System Clock.");
        }
    }

    virtual void load() override
    {
        logger.debug() << "DUMP LOAD";

        Machine::load();
        pio_sm_put_blocking(pioID, 0, xferBits - 1);
    }

    void dump()
    {
        pio_interrupt_clear(pioID, dumper_SYNC_IRQ);
    }

    const uint  xferBits;
    const float xferFreq;

  protected:
    virtual pio_sm_config buildConfig() override
    {
        logger.debug() << "DUMP CONFIG";

        pio_sm_config cfg = dumper_program_get_default_config(address);

        sm_config_set_clkdiv(&cfg, clock_get_hz(clk_sys) / (2.0 * xferFreq));
        sm_config_set_out_shift(&cfg, true, true, 32);
        sm_config_set_in_shift(&cfg, true, true, 32);
        sm_config_set_sideset_pins(&cfg, basePin);
        sm_config_set_fifo_join(&cfg, PIO_FIFO_JOIN_TX);

        return cfg;
    }
};

class PixelDriver : public Device {
  public:
    PixelDriver(const uint pinBase)
        : PixelDriver(pinBase, pinBase + NUM_RGB_PINS, pinBase + NUM_RGB_PINS + NUM_MUX_PINS)
    {
    }

    PixelDriver(const uint rgbBase, const uint muxBase, const uint ctrlBase)
        : Device("PIO")
        , pioID(pio0)
        , rgbBase(rgbBase)
        , muxBase(muxBase)
        , ctrlBase(ctrlBase)
        , clockPin(ctrlBase + 0)
        , latchPin(ctrlBase + 1)
        , powerPin(ctrlBase + 2)
        , dumper(pioID, 0, OFFSETS[0], clockPin, 8, 25e6)
        , rShift(pioID, 1, OFFSETS[1], rgbBase + 0)
        , gShift(pioID, 2, OFFSETS[2], rgbBase + 2)
        , bShift(pioID, 3, OFFSETS[3], rgbBase + 4)
    {
    }

    virtual void load() override
    {
        configurePins();

        rShift.load();
        gShift.load();
        bShift.load();
        dumper.load();
        Device::load();

        pio_enable_sm_mask_in_sync(pioID, 0xFF);
    }

    virtual void unload() override
    {
        pio_enable_sm_mask_in_sync(pioID, 0x00);

        rShift.unload();
        gShift.unload();
        bShift.unload();
        dumper.unload();
        Device::unload();
    }

    void update()
    {
        dumper.dump();
    }

    const PIO  pioID;
    const uint rgbBase;
    const uint muxBase;
    const uint ctrlBase;
    const uint clockPin;
    const uint latchPin;
    const uint powerPin;

  private:
    DumpMachine  dumper;
    ShiftMachine rShift;
    ShiftMachine gShift;
    ShiftMachine bShift;

    void configurePins() const
    {
        gpio_init_mask(0b00000110 << ctrlBase);
        gpio_init_mask(0b00011111 << muxBase);
        gpio_set_dir_out_masked(0b00000110 << ctrlBase);
        gpio_set_dir_out_masked(0b00011111 << muxBase);

        pio_gpio_init(pioID, clockPin % 32);

        for (Word pin = 0; pin < NUM_RGB_PINS; pin++) {
            pio_gpio_init(pioID, (rgbBase + pin) % 32);
        }

        pio_sm_set_consecutive_pindirs(pioID, 0, clockPin, 1, true);
        pio_sm_set_consecutive_pindirs(pioID, 1, rgbBase + 0, 2, true);
        pio_sm_set_consecutive_pindirs(pioID, 2, rgbBase + 2, 2, true);
        pio_sm_set_consecutive_pindirs(pioID, 3, rgbBase + 4, 2, true);
    }
};