#pragma once

#include "Device.hpp"

#include <string>
#include <vector>

namespace Vulkan {
struct PipelineConfigInfo {
    PipelineConfigInfo() = default;
    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    VkPipelineViewportStateCreateInfo viewportInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
    VkPipelineRasterizationStateCreateInfo rasterizationInfo;
    VkPipelineMultisampleStateCreateInfo multisampleInfo;
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlendInfo;
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    std::vector<VkDynamicState> dynamicStateEnables;
    VkPipelineDynamicStateCreateInfo dynamicStateInfo;
    VkPipelineLayout pipelineLayout = 0;
    VkRenderPass renderPass = 0;
    uint32_t subpass = 0;
};

class Pipeline {
public:
    Pipeline(Device &device, const std::string &vertFilePath, const std::string &fragFilePath,
             const PipelineConfigInfo &configInfo);
    ~Pipeline();

    Pipeline(const Pipeline &) = delete;
    Pipeline &operator=(const Pipeline &) = delete;

    void bind(VkCommandBuffer commandBuffer);

    static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

private:
    static std::vector<char> readFile(const std::string &filepath);

    void createGraphicsPipeline(const std::string &vertFilePath, const std::string &fragFilePath,
                                const PipelineConfigInfo &configInfo);

    void createShaderModule(const std::vector<char> &code, VkShaderModule *shaderModule);

    Device &device_;
    VkPipeline graphicsPipeline;

    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
};
} // namespace Vulkan
