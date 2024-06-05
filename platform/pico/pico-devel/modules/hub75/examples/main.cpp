#include "hub75/panel.hpp"
#include "pixutils/system/logger.hpp"
#include "pixutils/system/system.hpp"

#include <exception>

int main()
{
    System::launch();

    try {
        Panel panel(PanelConfig{6 * 64, 2, 25e6});
        panel.enable();
        panel.run();
        panel.disable();
    }
    catch (const std::exception& e) {
        System::log(LogLevel::CRITICAL) << e.what();
        System::shutdown();
        return EXIT_FAILURE;
    }

    System::standby();
    return EXIT_SUCCESS;
}