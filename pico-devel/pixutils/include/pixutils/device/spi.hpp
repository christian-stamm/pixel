#pragma once

#include "pixutils/device/device.hpp"

class SpiDevice : public Device {
  public:
    SpiDevice()
        : Device("SPI")
    {
    }
};