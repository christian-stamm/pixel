#include "hub75/panel.hpp"
#include "pixutils/system.hpp"

#include <exception>

int main()
{
    System::launch();

    try {
        Panel panel(PanelConfig(6 * 64, 5, 2));
        panel.start();
        panel.run();
        panel.stop();
    }
    catch (const std::exception& e) {
        System::log(CRITICAL) << e.what();
        System::shutdown();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}