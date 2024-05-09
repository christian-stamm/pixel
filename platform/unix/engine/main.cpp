#include "engine/app.hpp"

int main()
{
    PixelCubeApp::init();
    PixelCubeApp app;

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}