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

    WindowInfo window{WIDTH, HEIGHT};
    Vulkan::Device device{window};
    std::unique_ptr<Vulkan::DescriptorPool> globalPool{};
    std::unique_ptr<Vulkan::SwapChain> swapChain;
    std::unique_ptr<Vulkan::Pipeline> pipeline;

    std::unique_ptr<Vulkan::Mesh> mesh;

    std::vector<std::unique_ptr<Vulkan::Buffer>> sceneParamsUniformBuffer;
    std::vector<VkDescriptorSet> globalDescriptorSets;

    VkPipelineLayout pipelineLayout;
    std::vector<VkCommandBuffer> commandBuffers;
};
