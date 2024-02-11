#pragma once
#include "pixdriver/ws2812/config.hpp"
#include "pixdriver/ws2812/controller/pio.hpp"
#include "pixdriver/ws2812/types.hpp"
#include "pixtools/dev/device.hpp"
#include "pixtools/mem/buffer.hpp"
#include "pixtools/types.hpp"
#include "pixtools/utils/matrix.hpp"

inline const float timings[WS2812_NUM_BCM_PLANES] = {1.0, 1.5, 2.5, 4.5};

class BcmController : public Device {
  public:
    BcmController(const Config& config, Buffer<Word>& bcmBuffer)
        : Device("BCM", config)
        , numPixels(config.get<DWord>("numPixels"))
        , xfer_bits(numPixels * WS2812_BITS_PER_PIXEL)
        , xfer_time(xfer_bits / config.get<float>("xferFrequency") + config.get<float>("resetDuration"))
        , pioDev(PioController(config))
        , bcmBuffer(bcmBuffer)
        , timerID(0)
        , planeID(0)
    {
    }

    virtual void load() override
    {
        const float trigger = 1e-3;

        pioDev.load();

        timerID = add_alarm_in_us(trigger * 1e6, BcmController::timer_cb, reinterpret_cast<void*>(this), false);

        Device::load();
    }

    virtual void unload() override
    {
        cancel_alarm(timerID);

        pioDev.unload();

        Device::unload();
    }

    void render(const Buffer<Byte>& rgbBuffer, const RenderMode& mode)
    {
        Word bcmAddress, rgbAddress, flipAddress;

        const Word halfPixels = numPixels / 2.0;
        const Word lowerPixel = (mode & RenderMode::LOWER_HALF) ? 0 : halfPixels;
        const Word upperPixel = (mode & RenderMode::UPPER_HALF) ? numPixels : halfPixels;

        for (Word colorIndex = 0; colorIndex < 3; colorIndex++) {
            for (Word ledIndex = lowerPixel; ledIndex < upperPixel; ledIndex++) {
                for (Word stripIndex = 0; stripIndex < WS2812_NUM_STRIPS; stripIndex++) {
                    rgbAddress = 3 * (stripIndex * numPixels + ledIndex) + colorIndex;
                    unroll(rgbBuffer[rgbAddress], flipBuffer[stripIndex]);
                }

                flipBuffer.transpose();
                flipBuffer.reverse();

                for (Byte plane = 0; plane < WS2812_NUM_BCM_PLANES; plane++) {
                    flipAddress = WS2812_BITS_PER_COLOR * (1 * (plane));
                    bcmAddress  = WS2812_BITS_PER_COLOR * (3 * (plane * numPixels + ledIndex) + colorIndex);

                    SubBuffer<Word> src = flipBuffer.subrange(flipAddress, WS2812_BITS_PER_COLOR);
                    SubBuffer<Word> dst = bcmBuffer.subrange(bcmAddress, WS2812_BITS_PER_COLOR);

                    std::copy(src.begin(), src.end(), dst.begin());
                }
            }
        }
    }

    const DWord  numPixels;
    const DWord  xfer_bits;
    const double xfer_time;

  private:
    static int64_t timer_cb(alarm_id_t id, void* user_data)
    {
        const double  delay    = reinterpret_cast<BcmController*>(user_data)->streamNextPlane();
        const int64_t delay_us = static_cast<int64_t>(1e6 * delay);
        return delay_us;
    }

    const double streamNextPlane()
    {
        const Word   length = xfer_bits;
        const Word   offset = xfer_bits * (0 + planeID);
        const double delay  = xfer_time * timings[planeID];

        pioDev.flush(bcmBuffer.subrange(offset, length));

        planeID += 1;
        planeID %= WS2812_NUM_BCM_PLANES;

        return delay;
    }

    inline Word& unroll(const Byte& color, Word& dither) const
    {
        bool add1 = false;
        Byte mask = 0;

        const double gammaMul   = (4095.0 - 255.0) / (255.0 * 255.0);
        const Word   brightness = gammaMul * (color * color) + color;

        const Byte virtBits = (brightness >> 0) & 0x00F;
        const Byte physBits = (brightness >> 4) & 0x0FF;

        dither = 0;

        for (Word plane = 0; plane < WS2812_NUM_BCM_PLANES; plane++) {
            mask   = (1 << plane);
            add1   = (mask & virtBits) && (physBits != 255);
            dither = (dither << 8) | (physBits + (add1 ? 1 : 0));
        }

        return dither;
    }

    PioController pioDev;
    Buffer<Word>& bcmBuffer;
    Matrix32      flipBuffer;

    alarm_id_t timerID;
    Word       planeID;
};
