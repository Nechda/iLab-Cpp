#pragma once

#include "Lowering/Descriptors.hpp"
#include "Lowering/Device.hpp"
#include "Lowering/Mesh.hpp"
#include "Lowering/Pipeline.hpp"
#include "Lowering/SwapChain.hpp"
#include "Lowering/Window.hpp"

#include <vector>
#include <memory>

class App {
  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;
    App();
    ~App();

    App(const App &) = delete;
    App &operator=(const App &) = delete;

    void run();

  private:
    void loadMesh();
    void adjustGraphicsPipeline();
    void adjustComputePipeline();
    void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
    void createPipeline();
    void createCommandBuffers();
    void drawStage();
    void recreateSwapChain();
    void recordCommandBuffer(int imageIndex);
    void freeCommandBuffers();
    void updateUniformBuffer();

    WindowInfo window_m{WIDTH, HEIGHT};
    Vulkan::Device device_m{window_m};
    std::unique_ptr<Vulkan::DescriptorPool> globalPool_m{};
    std::unique_ptr<Vulkan::SwapChain> swapChain_m;
    std::unique_ptr<Vulkan::Pipeline> pipeline_m;

    std::unique_ptr<Vulkan::Mesh> mesh_m;

    std::vector<std::unique_ptr<Vulkan::Buffer>> sceneParamsUniformBuffer_m;
    std::vector<VkDescriptorSet> globalDescriptorSets_m;

    VkPipelineLayout pipelineLayout_m;
    std::vector<VkCommandBuffer> commandBuffers_m;
};
