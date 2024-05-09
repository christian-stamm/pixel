cmake_minimum_required(VERSION 3.16)
project(PixelCube)

set(CMAKE_CXX_STANDARD 23)

find_package(Vulkan REQUIRED)
find_package(glm REQUIRED)

add_executable(${PROJECT_NAME} main.cpp)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_SOURCE_DIR}/engine/shaders/compile.sh ${CMAKE_SOURCE_DIR}/engine/shaders ${CMAKE_CURRENT_BINARY_DIR}/shaders
    COMMENT "Compiling Shaders"
)

target_link_libraries(${PROJECT_NAME} ${GLFW_LIBRARIES} glfw Vulkan::Vulkan glm::glm)