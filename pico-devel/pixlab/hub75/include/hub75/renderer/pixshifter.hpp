// #pragma once
// #include "hardware/address_mapped.h"
// #include "hub75/renderer/pixshifter.pio.h"
// #include "pixutils/device/pio/machine.hpp"
// #include "pixutils/device/pio/program.hpp"
// #include "pixutils/types.hpp"

// #include <hardware/clocks.h>
// #include <hardware/gpio.h>
// #include <hardware/pio.h>

// class PixShifter {

//   public:
//     PixShifter(const PIO& pioID)
//         : pioID(config.pioID)
//     {
//     }

//     void process(const Word upper, const Word lower) const
//     {
//         pio_sm_put_blocking(pioID, smID, upper);
//         pio_sm_put_blocking(pioID, smID, lower);
//     }

//     Byte result() const
//     {
//         return pio_sm_get_blocking(pioID, smID);
//     }

//     static PioProgram buildPioProgram(const Byte& entryPoint)
//     {
//         pio_sm_config smcfg = shifter_program_get_default_config(entryPoint);
//         sm_config_set_out_shift(&smcfg, true, true, 30);
//         sm_config_set_in_shift(&smcfg, false, false, 6);

//         return PioProgram(shifter_program, smcfg, entryPoint);
//     }

//     const PIO  pioID;
//     const Byte smID;
// };
