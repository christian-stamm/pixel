project(hub75_pio
    VERSION 0.0.1
    LANGUAGES ASM CXX
    DESCRIPTION "HUB75 PIO Driver"  
)

find_package (Eigen3 3.3 REQUIRED NO_MODULE)

add_executable(${PROJECT_NAME}
    examples/main.cpp
)

add_dependencies(${PROJECT_NAME} pixutils)

target_link_libraries(${PROJECT_NAME} 
    Eigen3::Eigen
    pixutils 
)

target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>/
)

pico_generate_pio_header(${PROJECT_NAME} 
    ${CMAKE_CURRENT_LIST_DIR}/include/hub75/shifter.pio OUTPUT_DIR ${CMAKE_CURRENT_LIST_DIR}/include/hub75/
)

# enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

# create map/bin/hex/uf2 file etc.
pico_add_extra_outputs(${PROJECT_NAME})
