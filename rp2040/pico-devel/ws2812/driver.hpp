#pragma once
#include "pixdriver/ws2812/config.hpp"
#include "pixdriver/ws2812/controller/bcm.hpp"
#include "pixdriver/ws2812/types.hpp"
#include "pixtools/dev/device.hpp"
#include "pixtools/dev/spi.hpp"
#include "pixtools/mem/buffer.hpp"
#include "pixtools/sys/system.hpp"
#include "pixtools/types.hpp"
#include "pixtools/utils/color.hpp"

class WS2812 : public Device {
  public:
    WS2812(const Config& config)
        : Device("LED", config)
        , numPixels(config.get<DWord>("numPixels"))
        , rgbBuffer(Buffer<Byte>(3 * numPixels))
        , bcmBuffer(Buffer<Word>(1 * numPixels * WS2812_BITS_PER_PIXEL * WS2812_NUM_BCM_PLANES))
        , spiDev(SpiController(config, rgbBuffer))
        , bcmDev(BcmController(config, bcmBuffer))
    {
    }

    virtual void load() override
    {
        logger.log() << "Loading Devices...";

        this->clear();
        spiDev.load();
        bcmDev.load();
    }

    virtual void unload() override
    {
        logger.log() << "Cleanup Devices...";

        this->clear();
        spiDev.unload();
        bcmDev.unload();
    }

    virtual void run() override
    {
        logger.log() << "Running PixelDriver...";
        this->clear();

        while (true) {
            for (DWord b = 1; b < 0xFF; b++) {
                for (DWord s = 0; s < numPixels; s++) {
                    for (DWord l = 0; l < numPixels; l++) {
                        const float h = (45.0f / numPixels) * l;
                        const float v = (b / 255.0f);

                        const Color::RGB color = Color::convert(Color::HSV{h, 1.0f, v});
                        this->update(0, (l + s) % numPixels, color.r, color.g, color.b);
                    }

                    bcmDev.render(rgbBuffer, RenderMode::FULL);

                    System::spin();
                    System::sleep(0.1);
                }
            }
        }
    }

    inline void clear()
    {
        rgbBuffer.fill(0);
        bcmBuffer.fill(0);
    }

    inline void update(Word strip, Word index, Byte r, Byte g, Byte b)
    {
        const Word rgbBase = 3 * (numPixels * strip + index);

        rgbBuffer[rgbBase + 0] = r;
        rgbBuffer[rgbBase + 1] = g;
        rgbBuffer[rgbBase + 2] = b;
    }

  private:
    const DWord numPixels;

    Buffer<Byte> rgbBuffer;
    Buffer<Word> bcmBuffer;

    SpiController spiDev;
    BcmController bcmDev;
};
