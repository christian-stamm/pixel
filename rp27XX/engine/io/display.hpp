#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <limits>
#include <array>
#include <optional>
#include <set>

class Display
{
public:
    Display(int width, int height, VkInstance &instance, VkSurfaceKHR &surface)
        : width(width), height(height), resized(false), instance(instance), surface(surface), window(glfwCreateWindow(width, height, "Vulkan Display", nullptr, nullptr))
    {
        glfwSetWindowUserPointer(window, this);
        glfwSetWindowCloseCallback(window, Display::closeCallback);
        glfwSetFramebufferSizeCallback(window, Display::resizeCallback);
    }

    static void init()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    }

    void setup()
    {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create window!");
        }
    }

    void update()
    {
        glfwPollEvents();
    }

    void destroy()
    {
        glfwDestroyWindow(window);
        glfwTerminate();
    }

    bool isResized()
    {
        return resized;
    }

    bool isClosed()
    {
        return closed;
    }

    int getWidth()
    {
        return width;
    }

    int getHeight()
    {
        return height;
    }

private:
    int width;
    int height;
    bool closed;
    bool resized;

    VkInstance &instance;
    VkSurfaceKHR &surface;

    GLFWwindow *window;

    static void closeCallback(GLFWwindow *window)
    {
        reinterpret_cast<Display *>(glfwGetWindowUserPointer(window))->closed = true;
    }

    static void resizeCallback(GLFWwindow *window, int width, int height)
    {
        reinterpret_cast<Display *>(glfwGetWindowUserPointer(window))->resized = true;
    }
};
