project(pixutils
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "Pico Utils"
)

add_library(${PROJECT_NAME} INTERFACE)

target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_link_libraries(${PROJECT_NAME} INTERFACE
    pico_time
    pico_sync
    pico_stdlib
    pico_bit_ops
    pico_multicore
    hardware_irq
    hardware_dma
    hardware_pwm
    hardware_spi
    hardware_pio
    hardware_gpio
)