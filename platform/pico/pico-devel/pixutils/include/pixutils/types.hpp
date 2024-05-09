#pragma once

#include <cstdint>
#include <hardware/dma.h>
#include <hardware/irq.h>

using Byte  = uint8_t;
using DByte = uint16_t;
using Word  = uint32_t;
using DWord = uint64_t;

using IRQ_Callback = irq_handler_t;
using DmaXferSize  = dma_channel_transfer_size;
