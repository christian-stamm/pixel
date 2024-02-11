cmake_minimum_required(VERSION 3.19)

project(Pixel 
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "Pixel"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "-std=c++20")


add_compile_options(
    -Wall
    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't called
    -fexceptions
)

# add_compile_definitions(SIMULATION)

add_library(paneltools
    lib/matrix.cpp
)

target_include_directories(paneltools PUBLIC "${CMAKE_SOURCE_DIR}/include")

add_executable(panel_gpio
    examples/demo.cpp
)

target_link_libraries(panel_gpio paneltools)