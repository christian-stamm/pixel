#pragma once

#include "pixtools/config.hpp"
#include "pixtools/dev/device.hpp"
#include "pixtools/mem/buffer.hpp"
#include "pixtools/types.hpp"

class SpiController : public Device {
  public:
    SpiController(const Config& config, Buffer<Byte>& spiBuffer)
        : Device("SPI", config)
        , spiBuffer(spiBuffer)
    {
    }

    Buffer<Byte>& spiBuffer;
};