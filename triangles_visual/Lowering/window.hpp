#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

struct WindowInfo {
    public:
        WindowInfo(int w, int h) : width(w), height(h) {initWindow();};
        static WindowInfo win_info;
        static WindowInfo& Instance() { return win_info; }


        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface_ptr);

        VkExtent2D getExtent() { return {width, height}; }
        bool shouldClose() { return glfwWindowShouldClose(descriptor); }
        bool wasWindowResized() { return framebufferResized; }
        void resetWindowResizedFlag() { framebufferResized = false; } 

        GLFWwindow* descriptor = nullptr;
        unsigned height = 0;
        unsigned width = 0;
        bool framebufferResized = false;
    private:
        void initWindow();
        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
};
