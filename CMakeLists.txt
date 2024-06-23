cmake_minimum_required(VERSION 3.0...3.27)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_EXTRAS_PATH}/external/pico_extras_import.cmake)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)
set(PICO_CXX_ENABLE_EXCEPTIONS ON)

project(rp2040 
    VERSION 0.0.1
    LANGUAGES ASM C CXX
    DESCRIPTION "RP2040 Projects"
)

# Initialize the SDK
pico_sdk_init()

add_compile_options(-Wall -Wno-psabi)

# Add projects
add_subdirectory(pico-devel)
# add_subdirectory(pico-examples)
# add_subdirectory(pico-playground)


