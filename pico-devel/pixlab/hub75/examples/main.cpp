// #include "hub75/driver.hpp"
// #include "pixutils/system/logger.hpp"
#include "pixutils/system/system.hpp"
#include "pixutils/time/time.hpp"

#include <exception>

int main()
{
    System::launch();

    try {

        // Driver driver({6 * 64, 2, 25e6});

        // driver.enable();

        Watch watch(usDuration(1e6));

        while (System::isRunning()) {

            System::log() << watch;

            if (watch.expired()) {
                watch.reset();
                System::log() << "EXPIRED!";
            }

            System::sleep(1);
        }

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