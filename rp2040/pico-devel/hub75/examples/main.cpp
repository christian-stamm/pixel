#include "pixtools/sys/system.hpp"

int main()
{
    System::init(false);
    System::shutdown();
    System::standby();
    return 0;
}