#include "hardware/pio.h"
#include "hub75/renderer/renderer.hpp"
#include "pixutils/system.hpp"
#include "pixutils/watch.hpp"

#include <exception>

int main()
{
    System::launch();

    try {
        Watch watch;

        Buffer<Word> input  = Buffer<Word>::build(6 * 4096);
        Buffer<Byte> output = Buffer<Byte>::build(6 * 2048 * 10);

        Renderer renderer(pio0, 0, 4);

        renderer.start();

        while (System::isRunning()) {
            watch.reset();
            renderer.render(input, output);

            System::log() << watch;
            System::sleep(1);
        }

        renderer.stop();

        // Panel panel = Panel::build(PanelConfig::getDefault());

        // panel.start();
        // panel.run();
        // panel.stop();
    }
    catch (const std::exception& e) {
        System::log(CRITICAL) << e.what();
        System::shutdown();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}