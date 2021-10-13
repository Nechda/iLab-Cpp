#include "Window.hpp"

WindowInfo *WindowInfo::instance_ptr = nullptr;

void WindowInfo::framebufferResizeCallback(GLFWwindow *window, int width, int height) {
    auto win_info = reinterpret_cast<WindowInfo *>(glfwGetWindowUserPointer(window));
    win_info->framebufferResized = true;
    win_info->width = width;
    win_info->height = height;
}

void WindowInfo::initWindow() {
    if (instance_ptr != nullptr)
        return;

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    descriptor = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
    glfwSetInputMode(descriptor, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetWindowUserPointer(descriptor, this);
    glfwSetFramebufferSizeCallback(descriptor, framebufferResizeCallback);

    instance_ptr = this;
}

void WindowInfo::createWindowSurface(VkInstance instance, VkSurfaceKHR *surface_ptr) {
    if (glfwCreateWindowSurface(instance, descriptor, nullptr, surface_ptr) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}
