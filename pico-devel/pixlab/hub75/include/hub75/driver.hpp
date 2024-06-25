#pragma once
#include "hub75/config.hpp"
#include "hub75/pulser.hpp"
#include "hub75/shifter.hpp"
#include "pixutils/buffer.hpp"
#include "pixutils/device/device.hpp"

#include <hardware/clocks.h>
#include <hardware/gpio.h>
#include <hardware/pio.h>

class Driver : public Device {
  public:
    Driver(const PIO pioID, const PanelConfig& cfg, Buffer<Word>& frame)
        : Device("Driver")
        , cfg(cfg)
        , shifter({pioID, 0, 0, false}, cfg, frame)
        , pulser({pioID, 1, 21, false}, cfg)
    {
    }

    const PanelConfig cfg;

  protected:
    virtual void prepare() override
    {
        shifter.start();
        pulser.start();
    }

    virtual void cleanup() override
    {
        pulser.stop();
        shifter.stop();
    }

  private:
    Shifter shifter;
    Pulser  pulser;
};