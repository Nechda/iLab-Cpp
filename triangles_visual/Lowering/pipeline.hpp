#pragma once

#include "device.hpp"

#include <vector>
#include <string>

namespace Vulkan {
    struct PipelineConfigInfo{
        VkViewport viewport;
        VkRect2D scissor;
        VkPipelineInputAssemblyStateCreateInfo inputAssembly;
        VkPipelineRasterizationStateCreateInfo rasterizer;
        VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlending;
        VkPipelineDepthStencilStateCreateInfo depthStencil;
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;

        void init_default(uint32_t width, uint32_t height);
    };

    class Pipeline {
        public:
            Pipeline(
                Device& device,
                const std::string& vertFilePath,
                const std::string& fragFilePath,
                const PipelineConfigInfo& configInfo
            );
            ~Pipeline();

            Pipeline(const Pipeline&) = delete;
            Pipeline& operator=(const Pipeline&) = delete;

            void bind(VkCommandBuffer commandBuffer);

            static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);
        private:
            static std::vector<char> readFile(const std::string& filepath);

            void createGraphicsPipeline(
                const std::string& vertFilePath,
                const std::string& fragFilePath,
                const PipelineConfigInfo& configInfo
            );

            void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

            Device& device_;
            VkPipeline graphicsPipeline;

            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
} // namespace Vulkan