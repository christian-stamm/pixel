#include "../include/matrix.hpp"
#include "../include/system.hpp"
#include "../include/utils/logger.hpp"
#include "buffer.hpp"
#include "memory.hpp"

#include <cstdint>
#include <cstdlib>
#include <stdlib.h>
#include <unistd.h>

void clearGPIO()
{
    for (int i = 0; i < 30; i++) {
        GPIO::setFunc(i, GPIO::IOMode::IO_OUTPUT);
    }

    GPIO::clrPins(0xFFFFFFFF);
}

int main()
{
    Logger logger = Logger::getLogger("Demo");
#ifdef SIMULATION
    logger.info() << "Running in Simulation.";
#else
    logger.info() << "Running on RPi Hardware.";
#endif

    try {

        System::init();

        clearGPIO();

        FrameBuffer buffer(64, 64, 1);
        Matrix      matrix(Matrix::ADAFRUIT_CFG);

        while (System::isRunning()) {
            matrix.dump(buffer);
        }
    }
    catch (const std::exception& e) {
        logger.critical() << e.what();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}