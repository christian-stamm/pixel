cmake_minimum_required(VERSION 3.19)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include($ENV{PICO_EXTRAS_PATH}/external/pico_extras_import.cmake)

set(PICO_CXX_ENABLE_EXCEPTIONS 1)

project(pico_devel 
    VERSION 0.0.1
    LANGUAGES CXX ASM
    DESCRIPTION "RP2040 Projects"
)

add_dependencies(${PROJECT_NAME} pixtools)

find_package(pixtools REQUIRED)

# Initialize the SDK
pico_sdk_init()

# Add projects
add_subdirectory(pico-devel)
# add_subdirectory(pico-examples)
# add_subdirectory(pico-extras)
# add_subdirectory(pico-playground)

