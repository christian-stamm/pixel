#pragma once
#include "pixutils/device/device.hpp"
#include "pixutils/device/pio/program.hpp"
#include "pixutils/logger.hpp"
#include "pixutils/types.hpp"

#include <cstdint>
#include <format>
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <optional>
#include <pico/time.h>
#include <pico/types.h>

struct PioMachineConfig {
    PIO  pioID;
    Byte pioEntry;
    bool autostart;
};

class PioMachine : public Device {
  public:
    PioMachine(PIO pioID, bool autostart = true)
        : PioMachine(pioID, getUnusedID(pioID), autostart)
    {
    }

    PioMachine(PIO pioID, Byte smID, bool autostart = true)
        : Device(std::format("PIOSM{}", smID))
        , pioID(pioID)
        , smID(smID)
        , autostart(autostart)
        , program(std::nullopt)
    {
        pio_sm_claim(pioID, smID);
    }

    virtual ~PioMachine()
    {
        if (pio_sm_is_claimed(pioID, smID)) {
            pio_sm_unclaim(pioID, smID);
        }
        else {
            logger(WARN) << std::format("CIAO {}!", smID);
        }
    }

    static PioMachine build(const PioMachineConfig& config)
    {
        return PioMachine(config.pioID, config.pioEntry, config.autostart);
    }

    inline void exec(uint16_t instr, std::optional<Word> data = std::nullopt) const
    {
        if (data.has_value()) {
            pio_sm_put(pioID, smID, data.value());
        }

        pio_sm_exec(pioID, smID, instr);
    }

    inline void write(Word& data, bool block = false) const
    {
        auto fn = block ? pio_sm_put_blocking : pio_sm_put;
        fn(pioID, smID, data);
    }

    inline Word read(bool block = false) const
    {
        auto fn = block ? pio_sm_get_blocking : pio_sm_get;
        return fn(pioID, smID);
    }

    const PIO  pioID;
    const Byte smID;
    const bool autostart;

    std::optional<PioProgram> program;

  protected:
    virtual void prepare() override
    {
        resetState();
        pio_sm_set_enabled(pioID, smID, autostart);
    }

    virtual void cleanup() override
    {
        pio_sm_set_enabled(pioID, smID, false);
    }

    virtual void resetState() override
    {
        if (program.has_value()) {
            PioProgram pioCode = program.value();
            pio_sm_init(pioID, smID, pioCode.base, &pioCode.pcfg);
            logger(DEBUG) << "Reset done.";
        }
        else {
            logger(WARN) << "Empty Reset.";
        }
    }

  private:
    static Byte getUnusedID(const PIO& pioID)
    {
        auto id = pio_claim_unused_sm(pioID, true);
        pio_sm_unclaim(pioID, id);
        return id;
    }
};