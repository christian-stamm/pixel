#include "hub75/driver.hpp"
#include "pixel.hpp"
#include "pixutils/system/logger.hpp"
#include "pixutils/system/system.hpp"

#include <exception>
#include <vector>

int main()
{
    System::launch();

    try {

        Driver driver({6 * 64, 2, 25e6});
        driver.enable();
        driver.update(std::vector<Pixel>());

        while (System::isRunning()) {
            System::sleep(1);
        }

        driver.disable();
    }
    catch (const std::exception& e) {
        System::log(LogLevel::CRITICAL) << e.what();
        System::shutdown();
        return EXIT_FAILURE;
    }

    System::standby();
    return EXIT_SUCCESS;
}