#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <array>
#include <optional>
#include <set>

#include "io/file.hpp"
#include "io/logger.hpp"
#include "io/display.hpp"
#include "types.hpp"
#include "engine.hpp"

const int WIDTH = 800;
const int HEIGHT = 600;

class PixelCubeApp
{
public:
    PixelCubeApp() : display(Display(WIDTH, HEIGHT)), engine(Engine()), logger(Logger::getLogger("App")){};

    static void init()
    {
        Display::init();
    }

    void run()
    {
        setup();
        mainLoop();
        cleanup();
    }

private:
    void setup()
    {
        display.setup();
        engine.setup();
        logger.info() << "Setup done.";
    };

    void mainLoop()
    {
        logger.info() << "Launch mainloop.";

        while (!display.isClosed())
        {
            display.update();
            engine.update();
        }

        logger.info() << "Shutdown mainloop.";
    };

    void cleanup()
    {
        engine.cleanup();
        display.destroy();

        logger.info() << "Cleanup done.";
    };

    Display display;
    Engine engine;
    Logger logger;
};