// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ------ //
// pulser //
// ------ //

#define pulser_wrap_target 2
#define pulser_wrap 11

#define pulser_SHIFT_TRIG_IRQ 0
#define pulser_SHIFT_DONE_IRQ 1

static const uint16_t pulser_program_instructions[] = {
    0x90e0, //  0: pull   ifempty block   side 2     
    0x70c0, //  1: out    isr, 32         side 2     
            //     .wrap_target
    0xb046, //  2: mov    y, isr          side 2     
    0x90e0, //  3: pull   ifempty block   side 2     
    0x7000, //  4: out    pins, 32        side 2     
    0x30c1, //  5: wait   1 irq, 1        side 2     
    0xb842, //  6: nop                    side 3     
    0xd000, //  7: irq    nowait 0        side 2     
    0x90e0, //  8: pull   ifempty block   side 2     
    0x7020, //  9: out    x, 32           side 2     
    0x004a, // 10: jmp    x--, 10         side 0     
    0x1085, // 11: jmp    y--, 5          side 2     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program pulser_program = {
    .instructions = pulser_program_instructions,
    .length = 12,
    .origin = -1,
};

static inline pio_sm_config pulser_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + pulser_wrap_target, offset + pulser_wrap);
    sm_config_set_sideset(&c, 2, false, false);
    return c;
}
#endif
