#pragma once
#include "hardware/pio_instructions.h"
#include "hub75/renderer/pixshifter.pio.h"
#include "pixutils/buffer.hpp"
#include "pixutils/device/device.hpp"
#include "pixutils/device/pio/handler.hpp"
#include "pixutils/device/pio/machine.hpp"
#include "pixutils/types.hpp"

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>

class Renderer : public Device {
  public:
    Renderer(const PIO& pioID, Byte pioBase, Byte numWorker)
        : Device("Renderer")
        , numWorker(numWorker)
        , handler(pioID)
        , program(buildPioProgram(pioBase))
    {
    }

    void render(const Buffer<Word>& input, Buffer<Byte>& output) const
    {
        const Word halfPixels = input.size() / 2;

        for (Byte bcmBit = 0; bcmBit < 10; bcmBit++) {
            for (Byte workerID = 0; workerID < numWorker; workerID++) {
                const PioMachine& worker = handler.getWorker(workerID);
                worker.exec(pio_encode_nop());
            }

            for (Word pixel = 0; pixel < halfPixels; pixel += numWorker) {
                for (Byte workerID = 0; workerID < numWorker; workerID++) {
                    const PioMachine& worker = handler.getWorker(workerID);
                    // worker.write(input[pixel + workerID], false);
                    // worker.write(input[pixel + workerID + halfPixels], false);
                    worker.exec(pio_encode_nop());
                }

                // for (Byte workerID = 0; workerID < numWorker; workerID++) {
                //     const PioMachine& worker = handler.getWorker(workerID);
                //     output[pixel + workerID] = worker.read(true);
                // }
            }
        }
    }

  protected:
    virtual void prepare() override
    {
        handler.registerProgram(program);

        for (Byte worker = 0; worker < numWorker; worker++) {
            handler.registerWorker(worker, program);
        }

        handler.startMachines();
    }

    virtual void cleanup() override
    {
        handler.stopMachines();

        for (Byte worker = 0; worker < numWorker; worker++) {
            handler.unregisterWorker(worker);
        }

        handler.unregisterProgram(program);
    }

  private:
    static PioProgram buildPioProgram(const Byte& entryPoint)
    {
        pio_sm_config smcfg = shifter_program_get_default_config(entryPoint);
        sm_config_set_out_shift(&smcfg, true, true, 30);
        sm_config_set_in_shift(&smcfg, false, true, 6);
        return PioProgram(shifter_program, smcfg, entryPoint);
    }

    void configurePioRegister(Byte bcmBit)
    {
        for (Byte worker = 0; worker < numWorker; worker++) {}
    }

    Byte       numWorker;
    PioHandler handler;
    PioProgram program;
};