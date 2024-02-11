cmake_minimum_required(VERSION 3.19)

set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 23)

project(pixel_devel 
    VERSION 0.0.1
    LANGUAGES C CXX ASM
    DESCRIPTION "Pixel Projects"
)

# Add projects
add_subdirectory(pixtools)
# add_subdirectory(rp2040)
# add_subdirectory(rp27XX)
