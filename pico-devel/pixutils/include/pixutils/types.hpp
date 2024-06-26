#pragma once
#include "pico/types.h"

#include <pico/stdlib.h>

using Byte  = uint8_t;
using DByte = uint16_t;
using Word  = uint32_t;
using DWord = uint64_t;

using Pin     = uint;
using Mask    = Word;
using Address = Word;

#define BIT_MASK(x)  (1 << x)
#define FILL_MASK(x) (BIT_MASK(x) - 1)

#define PIN_MASK(x) (BIT_MASK(x))
#define PIN_WRAP(x) (x % 32)