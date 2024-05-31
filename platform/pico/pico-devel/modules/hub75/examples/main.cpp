#include "hub75/driver.hpp"
#include "pixutils/system/logger.hpp"
#include "pixutils/system/system.hpp"

#include <exception>

static inline Word upscale(Byte color)
{
    return color * (color * color * color * UPSCALER + 1);
}

int main()
{
    System::launch();

    try {

        Driver driver({6 * 64, 2, 25e6});

        for (int i = 0; i < 256; i++) {
            System::log() << "i: " << i << " -> " << upscale(i);
        }

        // driver.enable();
        // driver.update(std::vector<Pixel>());

        // while (System::isRunning()) {
        //     System::sleep(1);
        // }

        // driver.disable();
    }
    catch (const std::exception& e) {
        System::log(LogLevel::CRITICAL) << e.what();
        System::shutdown();
        return EXIT_FAILURE;
    }

    System::standby();
    return EXIT_SUCCESS;
}