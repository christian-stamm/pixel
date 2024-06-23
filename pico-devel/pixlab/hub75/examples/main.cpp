#include "pixutils/buffer.hpp"
#include "pixutils/logger.hpp"
#include "pixutils/system.hpp"
#include "pixutils/types.hpp"

#include <exception>

int main()
{
    System::launch();

    try {
        // Watch watch(usDuration(1e6));

        Buffer<Word> b1 = Buffer<Word>::build(25e3);

        while (System::isRunning()) {

            System::log(INFO) << "RESATART";
            System::log(INFO) << b1.subrange(0, 100);

            auto b2 = b1.subrange(35000, 100);

            b2.fill(0xFF00FF00);

            auto b3 = b1;
            auto b4 = std::move(b3);

            System::log(WARN) << b1.subrange(35000, 100);

            System::sleep(0.1);
        }
    }
    catch (const std::exception& e) {
        System::log(CRITICAL) << e.what();
        System::shutdown();
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}