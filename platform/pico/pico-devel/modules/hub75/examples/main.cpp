#include "hub75/driver.hpp"
#include "hub75/pixel.hpp"
#include "pixutils/system/logger.hpp"
#include "pixutils/system/system.hpp"
#include "pixutils/time/watch.hpp"

#include <Eigen/src/Core/Matrix.h>
#include <cstdlib>
#include <exception>
#include <pico/stdlib.h>

int main()
{
    System::launch();

    try {
        System::log() << "Running";

        Watch       watch(3);
        PixelDriver driver(2);

        driver.load();

        while (System::isRunning()) {
            System::spin();

            if (watch.expired()) {
                pio_sm_put(pio0, 1, 0b00011011);
                pio_sm_put(pio0, 2, 0b01101100);
                pio_sm_put(pio0, 3, 0b10011001);

                driver.update();
                watch.reset();
            }
        }
    }
    catch (const std::exception& e) {
        System::log(LogLevel::CRITICAL) << e.what();
        System::shutdown();
        return EXIT_FAILURE;
    }

    System::standby();
    return EXIT_SUCCESS;
}