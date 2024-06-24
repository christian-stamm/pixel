#include "hub75/driver.hpp"
#include "pixutils/system.hpp"

#include <exception>

int main()
{
    System::launch();

    try {
        Driver driver({6 * 64, 2, 0});
        driver.enable();
        driver.run();
        driver.disable();
    }
    catch (const std::exception& e) {
        System::log(CRITICAL) << e.what();
        System::shutdown();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}