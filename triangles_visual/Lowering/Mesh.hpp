#pragma once

#include "device.hpp"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <array>

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
            Mesh &operator=(const Mesh&) = delete;

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);
        private:
            void createVertexBuffers(const std::vector<Vertex> &vertices);

            Device &device_;
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            uint32_t vertexCount;
    };
}