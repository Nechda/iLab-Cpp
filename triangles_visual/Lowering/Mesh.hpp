#pragma once

#include "Buffer.hpp"
#include "Device.hpp"

#define GLM_FORCE_RADIANS
#include <array>
#include <glm/glm.hpp>
#include <memory>

namespace Vulkan {
class Mesh {
public:
    struct Vertex {
        glm::vec3 pos;
        glm::vec3 color;
        glm::vec3 normal;

        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
    };

    Mesh(Device &device, const std::vector<Vertex> &vertices);
    ~Mesh();

    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    void bind(VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

private:
    void createVertexBuffers(const std::vector<Vertex> &vertices);

    Device &device_;
    std::unique_ptr<Buffer> vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t vertexCount;
};
} // namespace Vulkan
