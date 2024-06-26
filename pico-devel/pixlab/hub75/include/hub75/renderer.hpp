#pragma once
#include "hardware/pio_instructions.h"
#include "hub75/renderer.pio.h"
#include "pixutils/buffer.hpp"
#include "pixutils/config.hpp"
#include "pixutils/device/device.hpp"
#include "pixutils/device/pio.hpp"
#include "pixutils/types.hpp"

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>
#include <vector>

class Shifter : public SmDevice {

  public:
    static Shifter build(const Config& config)
    {
        Config shiftCfg = config.get<Config>("renderConfig");
        return Shifter(
            config,                     //
            shiftCfg.get<PIO>("pioID"), //
            shiftCfg.get<Address>("pioBase", 0));
    }

    void process(const Word upper, const Word lower) const
    {
        pio_sm_put_blocking(pioID, smID, upper);
        pio_sm_put_blocking(pioID, smID, lower);
    }

    Byte result() const
    {
        return pio_sm_get_blocking(pioID, smID);
    }

  protected:
    Shifter(const Config& config, const PIO& pioID, const Byte& pioBase)
        : SmDevice(SmDevice::build(config, pioID, buildPioProgram(pioBase)))
    {
    }

    virtual PioProgram buildPioProgram(const Address& pioBase)
    {
        pio_sm_config smcfg = shifter_program_get_default_config(pioBase);
        sm_config_set_out_shift(&smcfg, true, true, 30);
        sm_config_set_in_shift(&smcfg, false, false, NUM_RGB_PINS);

        PioCommands commands;
        commands.push_back(PioCommand(pio_encode_out(pio_y, 32), pio_encode_pull(true, true)));

        return PioProgram(shifter_program, smcfg, commands, pioBase, true);
    }
};

class Renderer : public Device {
  public:
    static Renderer build(const Config& config)
    {
        Config renderCfg = config.get<Config>("renderConfig");
        return Renderer(
            config,                           //
            renderCfg.get<Byte>("numWorker"), //
            Shifter::build(config)            //
        );
    }

    void render(const Buffer<Word>& input, Buffer<Byte>& output) const
    {
        const Word halfPixels = input.size() / 2;

        for (Word pixel = 0; pixel < halfPixels; pixel += 4) {
            workers[0].process(input[pixel + 0], input[pixel + halfPixels + 0]);
            workers[1].process(input[pixel + 1], input[pixel + halfPixels + 1]);
            workers[2].process(input[pixel + 2], input[pixel + halfPixels + 2]);
            workers[3].process(input[pixel + 3], input[pixel + halfPixels + 3]);

            output[pixel + 0] = workers[0].result();
            output[pixel + 1] = workers[1].result();
            output[pixel + 2] = workers[2].result();
            output[pixel + 3] = workers[3].result();
        }
    }

  protected:
    Renderer(const Config& config, const Byte numWorker, const Shifter& worker)
        : Device("Renderer")
        , workers(4, worker)
    {
    }

    virtual void prepare() override
    {
        for (Shifter& worker : workers) {
            worker.start();
        }
    }

    virtual void cleanup() override
    {
        for (Shifter& worker : workers) {
            worker.stop();
        }
    }

  private:
    std::vector<Shifter> workers;
};