#pragma once
#include <stdint.h>

#define BCM2708_PERI_BASE 0x20000000
#define BCM2709_PERI_BASE 0x3F000000
#define BCM2711_PERI_BASE 0xFE000000

#define GPIO_REG_OFFSET 0x200000
#define REG_BLOCK_SIZE  (1024 * sizeof(uint32_t))
