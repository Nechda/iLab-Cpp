#include "Lowering/window.hpp"
#include "Lowering/device.hpp"
#include "Lowering/pipeline.hpp"
#include "Lowering/SwapChain.hpp"
#include "Lowering/Mesh.hpp"
#include "Lowering/Descriptors.hpp"
#include "Camera.hpp"

#include "Octree/octree.hpp"
#include "Octree/triangle.hpp"

#include <vector>
#include <memory>
#include <stdio.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


WindowInfo* WindowInfo::instance_ptr = nullptr;

Camera scene_camera;

struct SceneInfo {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 cam_pos;
    alignas(16) glm::vec3 light_dir;
    alignas(16) glm::vec3 light_color;
}global_scene_info;

std::vector<Vulkan::Mesh::Vertex> read_vertices_from_file() {
    size_t N = 0;
    std::cin >> N;
    std::vector<Geomentry::Triangle> trs(N);
    for (auto &tr : trs) {
        for (auto v : {0, 1, 2})
            for (auto d : {0, 1, 2})
                std::cin >> tr[v][d];
    }

    // find an area where located all triangles
    Geomentry::Vec3 min = trs[0][0];
    Geomentry::Vec3 max = trs[0][0];
    for (auto &tr : trs) {
        for (auto d : {0, 1, 2})
            for (auto v : {0, 1, 2}) {
                min[d] = std::min(min[d], tr[v][d]);
                max[d] = std::max(max[d], tr[v][d]);
            }
    }

    Algorithm::Octree tree(min, max, trs);
    for (size_t i = 0; i < N; i++)
        tree.insert(trs[i], i);

    // dfs
    tree.DFS();

    std::vector<Vulkan::Mesh::Vertex> result(3 * N);
    size_t current_tr = 0;
    glm::vec3 colors[2] = {
       {0,0,1},
       {1,0,0}
    };
    glm::vec3 vert[3];
    auto set = tree.get_set();
    for (size_t i = 0; i < N; i++) {
        bool is_intersected = set[i];
        for(auto vi: {0, 1, 2})
        for(auto di: {0, 1, 2})
            vert[vi][di] = trs[i][vi][di];

        glm::vec3 n = glm::cross(vert[0] - vert[1], vert[0] - vert[2]);
        n = glm::normalize(n);

        for(auto vi : {0, 1, 2}) {
            result[current_tr].pos = vert[vi];
            result[current_tr].normal = n;
            result[current_tr].color = colors[is_intersected];
            current_tr++;
        }
    }

    return result;
}


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
        void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
        void createPipeline();
        void createCommandBuffers();
        void drawFrame();
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

        std::vector<std::unique_ptr<Vulkan::Buffer>> sceneUBOBuffer;
        std::vector<VkDescriptorSet> globalDescriptorSets;

        VkPipelineLayout pipelineLayout;
        std::vector<VkCommandBuffer> commandBuffers;
};

App::App() {
    loadMesh();
    globalPool =
    Vulkan::DescriptorPool::Builder(device)
          .setMaxSets(Vulkan::SwapChain::MAX_FRAMES_IN_FLIGHT)
          .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Vulkan::SwapChain::MAX_FRAMES_IN_FLIGHT)
          .build();
}

App::~App() {
    vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
}

void App::run() {

    sceneUBOBuffer.resize(Vulkan::SwapChain::MAX_FRAMES_IN_FLIGHT);
    for(size_t i = 0; i < sceneUBOBuffer.size(); i++) {
        sceneUBOBuffer[i] = std::make_unique<Vulkan::Buffer>(
            device,
            sizeof(SceneInfo),
            Vulkan::SwapChain::MAX_FRAMES_IN_FLIGHT,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );
        sceneUBOBuffer[i]->map();
    }

    auto globalSetLayout =
    Vulkan::DescriptorSetLayout::Builder(device)
        .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
        .build();

    /* Moved from constructor */
    createPipelineLayout(globalSetLayout->getDescriptorSetLayout());
    recreateSwapChain();
    createCommandBuffers();
    /* End initialization from constructor */


    /* Create descriptors sets for accessing to UBO object in shaders*/
    globalDescriptorSets.resize(Vulkan::SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = sceneUBOBuffer[i]->descriptorInfoForIndex(i);
        Vulkan::DescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }

    while(!window.shouldClose()) {
        glfwPollEvents();
        drawFrame();
        scene_camera.update();
    }
    vkDeviceWaitIdle(device.device());
}

void App::loadMesh() {
    // just one triangle
    #if 0
    std::vector<Vulkan::Mesh::Vertex> vertices {
        {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}},
        {{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 1.0f}}
    };
    #endif

    std::vector<Vulkan::Mesh::Vertex> vertices(std::move(read_vertices_from_file()));

    mesh = std::make_unique<Vulkan::Mesh>(device, vertices);
}

void App::freeCommandBuffers() {
    vkFreeCommandBuffers(
        device.device(),
        device.getCommandPool(),
        static_cast<uint32_t>(commandBuffers.size()),
        commandBuffers.data());
    commandBuffers.clear();
}

void App::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
    // TODO: SHOULD_ENABLE
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts{globalSetLayout};
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

    if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
}

void App::recreateSwapChain() {
    auto extent = window.getExtent();
    while (extent.width == 0 || extent.height == 0) {
        extent = window.getExtent();
        glfwWaitEvents();
    }
    vkDeviceWaitIdle(device.device());

    if (swapChain == nullptr) {
        swapChain = std::make_unique<Vulkan::SwapChain>(device, extent);
    } else {
        swapChain = std::make_unique<Vulkan::SwapChain>(device, extent, std::move(swapChain));
        if (swapChain->imageCount() != commandBuffers.size()) {
            freeCommandBuffers();
            createCommandBuffers();
        }
    }

    createPipeline();
}

void App::createPipeline() {
    auto pipelineConfig = Vulkan::Pipeline::defaultPipelineConfigInfo(swapChain->width(), swapChain->height());
    pipelineConfig.renderPass = swapChain->getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Vulkan::Pipeline>(device, "shaders/vert.spv", "shaders/frag.spv", pipelineConfig);
}

void App::createCommandBuffers() {
    commandBuffers.resize(swapChain->imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = device.getCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();

    if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

void App::recordCommandBuffer(int imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = swapChain->getRenderPass();
    renderPassInfo.framebuffer = swapChain->getFrameBuffer(imageIndex);
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        pipeline->bind(commandBuffers[imageIndex]);
        vkCmdBindDescriptorSets(
            commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
            0, 1, &globalDescriptorSets[imageIndex], 0, nullptr
        );
        mesh->bind(commandBuffers[imageIndex]);
        mesh->draw(commandBuffers[imageIndex]);

    vkCmdEndRenderPass(commandBuffers[imageIndex]);

    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void App::updateUniformBuffer() {
    SceneInfo& ubo = global_scene_info;
    auto camPos = scene_camera.get_position();

    ubo.model = glm::mat4(1);
    ubo.view = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChain->extentAspectRatio(), 0.1f, 10000.0f);
    ubo.proj[1][1] *= -1;
    ubo.light_color = glm::vec4(1.0f, 1.0f, 1.0f,1);
    ubo.light_dir = glm::vec4(0,0,1,0);
    ubo.cam_pos = camPos;
}


void App::drawFrame() {
    uint32_t imageIndex;
    VkResult result = swapChain->acquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    // TODO: SHOULD_ENABLED
    updateUniformBuffer();
    sceneUBOBuffer[imageIndex]->writeToBuffer(&global_scene_info);
    sceneUBOBuffer[imageIndex]->flush();

    recordCommandBuffer(imageIndex);

    result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR  || window.wasWindowResized()) {
        window.resetWindowResizedFlag();
        recreateSwapChain();
        return;
    }
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
}

int main() {
    App app;
    try{
    app.run();
    } catch(std::exception& e) {
        printf("what() = %s", e.what());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
