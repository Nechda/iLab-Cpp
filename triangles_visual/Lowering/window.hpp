#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <stdexcept>

struct WindowInfo {
    public:
        WindowInfo(int w, int h) : width(w), height(h) {initWindow();};
        
        static WindowInfo& Instance() { return *instance_ptr; }

        void createWindowSurface(VkInstance instance, VkSurfaceKHR *surface_ptr);

        VkExtent2D getExtent() { return {width, height}; }
        bool shouldClose() {    
            bool res = 0;
            res |= glfwWindowShouldClose(descriptor);
            res |= glfwGetKey(descriptor, GLFW_KEY_Q) == GLFW_PRESS;
            res |= glfwGetKey(descriptor, GLFW_KEY_ESCAPE) == GLFW_PRESS;
            return res;
        }
        bool wasWindowResized() { return framebufferResized; }
        void resetWindowResizedFlag() { framebufferResized = false; } 

        GLFWwindow* descriptor = nullptr;
        unsigned height = 0;
        unsigned width = 0;
        bool framebufferResized = false;
    private:
        void initWindow();
        static void framebufferResizeCallback(GLFWwindow *window, int width, int height);
        static WindowInfo* instance_ptr;
};
