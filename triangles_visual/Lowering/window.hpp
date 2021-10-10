#pragma once

#ifndef GLFW_INCLUDE_VULKAN
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#endif

#include <stdexcept>

struct WindowInfo {
        WindowInfo() :
                descriptor(nullptr),
                height(0),
                width(0)
            {};
        static WindowInfo win_info;
        static WindowInfo& Instance() {
            return win_info;
        }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface_ptr) {
            if (glfwCreateWindowSurface(instance, descriptor, nullptr, surface_ptr) != VK_SUCCESS) {
                throw std::runtime_error("failed to create window surface!");
            }
        }

        VkExtent2D getExtent() {
            return {width, height};
        }

        bool shouldClose() {
            return glfwWindowShouldClose(descriptor);
        }

        GLFWwindow* descriptor;
        unsigned height;
        unsigned width;
};
