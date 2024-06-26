#pragma once
#include "pixutils/config.hpp"
#include "pixutils/device/device.hpp"
#include "pixutils/types.hpp"

#include <format>
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <optional>
#include <pico/time.h>
#include <pico/types.h>
#include <stdexcept>
#include <vector>

class PioCommand {
  public:
    PioCommand(DByte instr, std::optional<Word> data = std::nullopt)
        : instr(instr)
        , data(data)
    {
    }

    void exec(const PIO& pioID, const uint& smID) const
    {
        if (data.has_value()) {
            pio_sm_put(pioID, smID, data.value());
        }

        pio_sm_exec(pioID, smID, instr);
    }

    const DByte               instr;
    const std::optional<Word> data;
};

using PioCommands = std::vector<PioCommand>;

struct PioProgram : public pio_program {

    pio_sm_config           config;
    uint                    address;
    std::vector<PioCommand> commands;
    bool                    autostart;

    PioProgram(
        const pio_program& base, const pio_sm_config& config, const PioCommands& commands, uint address, bool autostart)
        : pio_program(base)
        , config(config)
        , address(address)
        , commands(commands)
        , autostart(autostart)
    {
    }
};

class SmDevice : public Device {
  public:
    static SmDevice build(const Config& config, const PIO& pioID, const PioProgram& program)
    {
        int  instance = pio_claim_unused_sm(pioID, false);
        bool validID  = (0 <= instance);

        if (!validID) {
            throw std::runtime_error(std::format("No Statemachine available for PIO {}.", pio_get_index(pioID)));
        }
        else {
            return SmDevice(pioID, static_cast<Byte>(instance), program);
        }
    }

    ~SmDevice()
    {
        pio_sm_unclaim(pioID, smID);
    }

    const PIO        pioID;
    const Byte       smID;
    const PioProgram program;

  protected:
    SmDevice(PIO pioID, Byte smID, const PioProgram& program)
        : Device(std::format("Machine", smID))
        , pioID(pioID)
        , smID(smID)
        , program(program)
    {
    }

    virtual void prepare() override
    {
        resetState();
        pio_sm_set_enabled(pioID, smID, program.autostart);
    }

    virtual void cleanup() override
    {
        pio_sm_set_enabled(pioID, smID, false);
    }

    virtual void resetState() override
    {
        pio_sm_init(pioID, smID, program.address, &program.config);

        for (const PioCommand& cmd : program.commands) {
            cmd.exec(pioID, smID);
        }
    }
};

class PioDevice : public Device {
  public:
    PioDevice(const PIO pioID)
        : Device(std::format("PIO{}", pio_get_index(pioID)))
        , pioID(pioID)
        , machines(0)
    {
    }

    void addProgram(PioProgram& program)
    {
        program.address = pio_add_program(pioID, &program);
    }

    void removeProgram(PioProgram& program)
    {
        pio_remove_program(pioID, &program, program.address);
    }

    const PIO pioID;

  protected:
    virtual void prepare() override
    {
        pio_enable_sm_mask_in_sync(pioID, machines);
    }

    virtual void cleanup() override
    {
        pio_enable_sm_mask_in_sync(pioID, 0b0000);
    }

    virtual void resetState() override
    {
        pio_clear_instruction_memory(pioID);
    }

  private:
    Mask machines;
};