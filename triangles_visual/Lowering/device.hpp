#pragma once

#include "window.hpp"

#include <vector>
#include <optional>

namespace Vulkan {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    class Device {
    public:
        Device(WindowInfo &window);
        ~Device();

        VkDebugUtilsMessengerEXT debugMessenger;
        static constexpr bool enableValidationLayers = false;
        void setupDebugMessenger();
        bool checkValidationLayerSupport();
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

        // Not copyable or movable
        Device(const Device &) = delete;
        Device &operator=(const Device &) = delete;
        Device(Device &&) = delete;
        Device &operator=(Device &&) = delete;

        VkCommandPool getCommandPool() { return commandPool; }
        VkDevice device() { return device_; }
        VkSurfaceKHR surface() { return surface_; }
        VkQueue graphicsQueue() { return graphicsQueue_; }
        VkQueue presentQueue() { return presentQueue_; }

        SwapChainSupportDetails getSwapChainSupport() { return querySwapChainSupport(physicalDevice); }
        QueueFamilyIndices findPhysicalQueueFamilies() { return findQueueFamilies(physicalDevice); }
        
        VkFormat findSupportedFormat(
            const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        
        // Buffer Helper Functions
        void createBuffer(
            VkDeviceSize size,
            VkBufferUsageFlags usage,
            VkMemoryPropertyFlags properties,
            VkBuffer &buffer,
            VkDeviceMemory &bufferMemory);
        VkCommandBuffer beginSingleTimeCommands();
        void endSingleTimeCommands(VkCommandBuffer commandBuffer);
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

        void createImageWithInfo(
            const VkImageCreateInfo &imageInfo,
            VkMemoryPropertyFlags properties,
            VkImage &image,
            VkDeviceMemory &imageMemory
        );

        /*
        void copyBufferToImage(
            VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        VkPhysicalDeviceProperties properties;
        */

    private:
        void createInstance(); // done
        void createSurface(); // done
        void pickPhysicalDevice(); // done
        void createLogicalDevice(); // done
        void createCommandPool(); // done

        
        // helper functions
        bool isDeviceSuitable(VkPhysicalDevice device); // done
        std::vector<const char *> getRequiredExtensions(); // done
        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        bool checkDeviceExtensionSupport(VkPhysicalDevice device); // done
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device); // done
        
        VkInstance instance;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        WindowInfo &window_;
        VkCommandPool commandPool;

        VkDevice device_;
        VkSurfaceKHR surface_;
        VkQueue graphicsQueue_;
        VkQueue presentQueue_;

        const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };
} // namespace Vulkan
