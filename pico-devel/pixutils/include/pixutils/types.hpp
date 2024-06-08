#pragma once
#include <pico/stdlib.h>

using Byte  = uint8_t;
using DByte = uint16_t;
using Word  = uint32_t;
using DWord = uint64_t;

using Pin  = uint;
using Mask = Word;

#define BitMask(x)  ((1 << x))
#define FillMask(x) (BitMask(x) - 1)
