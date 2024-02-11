#include "pixdriver/ws2812/driver.hpp"
#include "pixtools/config.hpp"
#include "pixtools/sys/logger.hpp"
#include "pixtools/sys/system.hpp"
#include "pixtools/types.hpp"

int main()
{
    Logger logger = Logger::getLogger("Main");

    System::init(true);

    Config config;
    config.set<DWord>("numPixels", 25);
    config.set<uint>("laneBasePin", 16);
    config.set<uint>("portBasePin", 4);
    config.set<float>("xferFrequency", 800e3);
    config.set<float>("resetDuration", 280e-6);

    WS2812 driver(config);

    driver.load();
    driver.run();
    driver.unload();

    System::shutdown();
    System::standby();
    return 0;
}