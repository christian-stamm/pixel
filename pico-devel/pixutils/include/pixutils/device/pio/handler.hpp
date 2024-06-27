#pragma once
#include "pixutils/device/pio/machine.hpp"
#include "pixutils/device/pio/program.hpp"
#include "pixutils/system.hpp"
#include "pixutils/types.hpp"

#include <format>
#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <optional>
#include <pico/time.h>
#include <pico/types.h>
#include <vector>

#define NUM_MACHINES 4

class PioHandler {
  public:
    using MachineList = std::vector<PioMachine>;

    PioHandler(const PIO& pioID)
        : pioID(pioID)
        , machines(buildMachines(pioID))
    {
    }

    void startMachines()
    {
        Mask bootmask = 0;

        for (PioMachine& machine : machines) {
            if (machine.program.has_value()) {
                machine.start();
                bootmask |= BIT_MASK(machine.smID);
            }
        }

        pio_enable_sm_mask_in_sync(pioID, bootmask);
        System::log(INFO) << std::format("Bootbits {:04b}", bootmask);
    }

    void stopMachines() const
    {
        pio_set_sm_mask_enabled(pioID, 0b0000, false);
    }

    void unregisterAllPrograms() const
    {
        pio_clear_instruction_memory(pioID);
    }

    void registerProgram(PioProgram& program)
    {
        pio_add_program_at_offset(pioID, &program, program.base);
    }

    void unregisterProgram(PioProgram& program)
    {
        pio_remove_program(pioID, &program, program.base);
    }

    void registerWorker(Byte workerID, PioProgram& program)
    {
        machines[workerID].program = program;
    }

    void unregisterWorker(Byte workerID)
    {
        machines[workerID].program.reset();
    }

    inline const PioMachine& getWorker(Byte worker) const
    {
        return machines[worker];
    }

    const PIO pioID;

  private:
    static MachineList buildMachines(const PIO& pioID)
    {
        MachineList machines;
        machines.reserve(NUM_MACHINES);

        for (Byte machine = 0; machine < NUM_MACHINES; machine++) {
            machines.emplace_back(PioMachine(pioID, machine, false));
        }

        return machines;
    }

    MachineList machines;
};