#pragma once
#include "pixutils/device/device.hpp"
#include "pixutils/types.hpp"

#include <format>
#include <hardware/dma.h>
#include <string>

using DmaXferSize = dma_channel_transfer_size;

struct DmaConfig {
    uint        data_req;
    bool        read_incr;
    bool        write_incr;
    DmaXferSize bitsize;
};

class DmaDevice : public Device {
  public:
    DmaDevice(const DmaConfig& config)
        : Device(std::format("DMA{}", config))
        , channelNum(dma_claim_unused_channel(true))
        , config(config)
        , channelCfg(dma_channel_get_default_config(channelNum))
    {
        channel_config_set_dreq(&channelCfg, config.data_req);
        channel_config_set_read_increment(&channelCfg, config.read_incr);
        channel_config_set_write_increment(&channelCfg, config.write_incr);
        channel_config_set_transfer_data_size(&channelCfg, config.bitsize);
    }

    ~DmaDevice()
    {
        dma_channel_unclaim(channelNum);
    }

    inline void abort() const
    {
        dma_channel_abort(channelNum);
    }

    inline bool busy() const
    {
        return dma_channel_is_busy(channelNum);
    }

    inline void transfer(const volatile void* src, volatile void* dest, const Word length) const
    {
        if (busy()) {
            abort();
        }

        dma_channel_configure(channelNum, &channelCfg, dest, src, length, true);
    }

    const uint      channelNum;
    const DmaConfig config;

  protected:
    virtual void prepare() override
    {
        dma_channel_set_config(channelNum, &channelCfg, false);
    }

    virtual void cleanup() override
    {
        dma_channel_cleanup(channelNum);
    }

  private:
    dma_channel_config channelCfg;
};
