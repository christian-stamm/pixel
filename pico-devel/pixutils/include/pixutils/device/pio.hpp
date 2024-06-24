#pragma once
#include "pixutils/device/device.hpp"

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <pico/time.h>
#include <string>

struct PioConfig {
    PIO  pioID;
    uint stmID;
    uint address;
    bool autoStart = true;
};

class PioMachine : public Device {
  public:
    PioMachine(const std::string& name, const PioConfig& piocfg)
        : Device(name)
        , pioID(piocfg.pioID)
        , stmID(piocfg.stmID)
        , address(piocfg.address)
        , autoStart(piocfg.autoStart)
        , smcfg(pio_get_default_sm_config())
        , program({nullptr, 0, -1})
    {
    }

    void configure(const pio_program& program, const pio_sm_config& smcfg)
    {
        this->program = program;
        this->smcfg   = smcfg;
    }

    static void run(const PIO& pioID, const Mask& mask)
    {
        pio_enable_sm_mask_in_sync(pioID, mask);
    }

    virtual void reset() override
    {
        pio_sm_init(pioID, stmID, address, &smcfg);
        setupRegs();
    }

    const PIO  pioID;
    const uint stmID;
    const uint address;
    const bool autoStart;

  protected:
    virtual void prepare() override
    {
        setupPins();

        pio_sm_claim(pioID, stmID);
        pio_add_program_at_offset(pioID, &program, address);

        reset();
        autoReload(true);

        pio_sm_set_enabled(pioID, stmID, autoStart);
    }

    virtual void cleanup() override
    {
        pio_sm_set_enabled(pioID, stmID, false);

        reset();
        autoReload(false);

        pio_remove_program(pioID, &program, address);
        pio_sm_unclaim(pioID, stmID);
    }

    virtual void setupPins() const {}
    virtual void setupRegs() const {}
    virtual void autoReload(bool enabled) {}

    virtual pio_sm_config setupPioConfig() const = 0;

    pio_sm_config smcfg;
    pio_program   program;
};