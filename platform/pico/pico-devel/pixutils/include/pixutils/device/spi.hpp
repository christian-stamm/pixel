#pragma once

#include "pixutils/device/device.hpp"
#include "pixutils/memory/buffer.hpp"
#include "pixutils/system/config.hpp"
#include "pixutils/types.hpp"

class SpiController : public Device {
  public:
    SpiController(const Config& config, Buffer<Byte>& spiBuffer)
        : Device("SPI", config)
        , spiBuffer(spiBuffer)
    {
    }

    Buffer<Byte>& spiBuffer;
};