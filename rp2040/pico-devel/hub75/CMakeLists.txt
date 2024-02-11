project(hub75
    VERSION 0.0.1
    LANGUAGES ASM CXX
    DESCRIPTION "HUB75 Driver"  
)

add_executable(${PROJECT_NAME}
    examples/main.cpp
)

# pico_generate_pio_header(${PROJECT_NAME} 
#     ${CMAKE_CURRENT_LIST_DIR}/controller/pio/pio_controller.pio OUTPUT_DIR 
#     ${CMAKE_CURRENT_LIST_DIR}/controller/pio/
# )

# Link to pico_stdlib (gpio, time, etc. functions)
target_link_libraries(${PROJECT_NAME}
    pixtools
    pico_stdlib
    pico_multicore
    pico_time
    pico_sync
    pico_bit_ops
    hardware_irq
    hardware_dma
    hardware_spi
    hardware_pio
)

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)

# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})