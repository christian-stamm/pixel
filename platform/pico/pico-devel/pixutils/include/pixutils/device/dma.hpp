#pragma once
#include "pixutils/device/device.hpp"
#include "pixutils/types.hpp"

#include <hardware/dma.h>
#include <string>

class DmaController : public Device {
  public:
    DmaController(
        const Config& config, const uint data_req, const bool read_incr, const bool write_incr,
        const DmaXferSize bitsize)
        : Device("DMA", config)
        , channel_ID(dma_claim_unused_channel(true))
        , dmaCFG(dma_channel_get_default_config(channel_ID))
    {
        dma_channel_unclaim(channel_ID);
        channel_config_set_dreq(&dmaCFG, data_req);
        channel_config_set_read_increment(&dmaCFG, read_incr);
        channel_config_set_write_increment(&dmaCFG, write_incr);
        channel_config_set_transfer_data_size(&dmaCFG, bitsize);
    }

    virtual void load() override
    {
        dma_channel_claim(channel_ID);
        dma_channel_set_config(channel_ID, &dmaCFG, false);
        Device::load();
    }

    virtual void unload() override
    {
        dma_channel_unclaim(channel_ID);
        dma_channel_cleanup(channel_ID);
        Device::unload();
    }

    void transfer(const volatile void* src, volatile void* dest, const Word length) const
    {
        //     logger.log() << "Transfer from" << const_cast<const void*>(src) << "to" << const_cast<const void*>(dest)
        //     << "("
        //                  << length << ")";

        dma_channel_set_read_addr(channel_ID, src, false);
        dma_channel_set_write_addr(channel_ID, dest, false);
        dma_channel_set_trans_count(channel_ID, length, true);
    }

    const uint channel_ID;

  private:
    dma_channel_config dmaCFG;
};
