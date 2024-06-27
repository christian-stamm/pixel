#pragma once

#include "hardware/pio.h"
#include "pixutils/types.hpp"
#include "pixutils/uuid.hpp"

struct PioProgram : public pio_program {
    pio_sm_config pcfg;
    Byte          base;
    UUID          uuid;

    PioProgram(const pio_program& program, const pio_sm_config& config, const Byte& base)
        : pio_program(program)
        , pcfg(config)
        , base(base)
        , uuid(UUID::generate())
    {
    }
};
