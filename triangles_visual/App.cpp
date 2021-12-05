#include "App.hpp"
#include "Camera.hpp"

#include "Octree/octree.hpp"
#include "Octree/triangle.hpp"

#include <memory>
#include <stdio.h>
#include <vector>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Camera scene_camera;

// TODO: NEAREST TASK --- create ssbo array of colours and change it for each 100 frames
//       in main loop of programm
// TODO: AFTER TASK --- create ssbo array for matrices that rotate ONE triangle arount
//       one of it edge

/*
    Let triangle{A,B,C} then for rotate matrix around AB segment need matrix M:
    r' = T(OA) R(AB, phi) T(-OA) r 
    where:
        T(p) --- translation matrix to verctor p
        R(v, phi) --- rotation matrix around v axis on phi angle
        OA --- vector from coordinate system to first point of triangle
*/

struct SceneInfo {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 cam_pos;
    alignas(16) glm::vec3 light_dir;
    alignas(16) glm::vec3 light_color;
} global_scene_info;


static std::vector<Vulkan::Mesh::Vertex> read_vertices_from_file() {
    size_t N = 0;
    size_t total_time = 0;
    std::cin >> N >> total_time;
    struct Pack_of_data {
        glm::Triangle tr;
        glm::vec3 axis[2];
        double omega;
    };
    std::vector<Pack_of_data> data(N);
    for (auto& it : data) {
        std::cin >> it.tr;
        std::cin >> it.axis[0] >> it.axis[1];
        std::cin >> it.omega;
    }

    std::vector<Geomentry::Cylinder> cylinders(N);
    for (size_t i = 0; i < N; i++) {
        cylinders[i] = Geomentry::Cylinder(data[i].tr, { data[i].axis[0], data[i].axis[1] });
    }

    Geomentry::Vec3 min = cylinders[0].get_box().get_min();
    Geomentry::Vec3 max = cylinders[0].get_box().get_max();
    for (size_t i = 1; i < N; i++) {
        auto min_ = cylinders[i].get_box().get_min();
        auto max_ = cylinders[i].get_box().get_max();

        for (int i = 0; i < 3; i++) {
            min[i] = std::min(min[i], min_[i]);
            max[i] = std::max(max[i], max_[i]);
        }
    }

    Algorithm::Octree tree(min, max, cylinders);
    for (size_t i = 0; i < N; i++)
        tree.insert(cylinders[i], i);

    tree.DFS();
    auto set_of_pairs = tree.get_set();

    std::vector<Vulkan::Mesh::Vertex> result(3 * N);
    size_t current_tr = 0;
    glm::vec3 colors[2] = {{0, 0, 1}, {1, 0, 0}};
    glm::vec3 vert[3];
    auto set = tree.get_set();
    for (size_t i = 0; i < N; i++) {
        bool is_intersected = false;

        for (auto vi : {0, 1, 2})
            vert[vi] = data[i].tr[vi];

        glm::vec3 n = glm::cross(vert[0] - vert[1], vert[0] - vert[2]);
        n = glm::normalize(n);

        for (auto vi : {0, 1, 2}) {
            result[current_tr].pos = vert[vi];
            result[current_tr].normal = n;
            result[current_tr].color = colors[is_intersected];
            current_tr++;
        }
    }

    return result;
}

void App::loadMesh() {
    std::vector<Vulkan::Mesh::Vertex> vertices(std::move(read_vertices_from_file()));
    mesh = std::make_unique<Vulkan::Mesh>(device, vertices);
}

App::App() {
    loadMesh();
    globalPool = Vulkan::DescriptorPool::Builder(device)
                     .setMaxSets(Vulkan::SwapChain::MAX_FRAMES_IN_FLIGHT)
                     .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, Vulkan::SwapChain::MAX_FRAMES_IN_FLIGHT)
                     .build();
}

App::~App() { vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr); }

void App::adjustComputePipeline() {
    /* Create set layout for compule pipeline */
    auto computeSetLayout = Vulkan::DescriptorSetLayout::Builder(device)
                            .addBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
                            .addBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT)
                            .build();
}

void App::adjustGraphicsPipeline() {
    /* For each swap-buffer create buffer */
    sceneParamsUniformBuffer.resize(Vulkan::SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (size_t i = 0; i < sceneParamsUniformBuffer.size(); i++) {
        sceneParamsUniformBuffer[i] = std::make_unique<Vulkan::Buffer>(
            device, sizeof(SceneInfo), Vulkan::SwapChain::MAX_FRAMES_IN_FLIGHT, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        sceneParamsUniformBuffer[i]->map();
    }

    /* Create set layout for graphics pipeline */
    auto globalSetLayout = Vulkan::DescriptorSetLayout::Builder(device)
                               .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT)
                               .build();

    /* Moved from constructor */
    createPipelineLayout(globalSetLayout->getDescriptorSetLayout());
    recreateSwapChain();
    createCommandBuffers();
    /* End initialization from constructor */

    /* Create descriptors sets for accessing to UBO object in shaders */
    globalDescriptorSets.resize(Vulkan::SwapChain::MAX_FRAMES_IN_FLIGHT);
    for (int i = 0; i < globalDescriptorSets.size(); i++) {
        auto bufferInfo = sceneParamsUniformBuffer[i]->descriptorInfo();
        Vulkan::DescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
    }
}

void App::run() {

    adjustComputePipeline();
    adjustGraphicsPipeline();

    while (!window.shouldClose()) {
        glfwPollEvents();
        //computeStage();
        drawStage();
        scene_camera.update();
    }
    vkDeviceWaitIdle(device.device());
}

void App::freeCommandBuffers() {
    vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()),
                         commandBuffers.data());
    commandBuffers.clear();
}

void App::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
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
    Vulkan::PipelineConfigInfo pipelineConfig{};
    Vulkan::Pipeline::defaultPipelineConfigInfo(pipelineConfig);
    pipelineConfig.renderPass = swapChain->getRenderPass();
    pipelineConfig.pipelineLayout = pipelineLayout;
    pipeline = std::make_unique<Vulkan::Pipeline>(device, "shader.vert.glsl.spv", "shader.frag.glsl.spv", pipelineConfig);
}

void App::createCommandBuffers() {
    commandBuffers.resize(swapChain->imageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = device.getCommandPool();
    allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

    if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) !=
        VK_SUCCESS) {
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

    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
    clearValues[1].depthStencil = { 1.0f, 0 };
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
    viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{ {0, 0}, swapChain->getSwapChainExtent() };
    vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
    vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

    pipeline->bind(commandBuffers[imageIndex]);
    vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1,
                            &globalDescriptorSets[imageIndex], 0, nullptr);
    mesh->bind(commandBuffers[imageIndex]);
    mesh->draw(commandBuffers[imageIndex]);

    vkCmdEndRenderPass(commandBuffers[imageIndex]);

    if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
}

void App::updateUniformBuffer() {
    SceneInfo &ubo = global_scene_info;
    auto camPos = scene_camera.get_position();

    ubo.model = glm::mat4(1);
    ubo.view = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChain->extentAspectRatio(), 0.1f, 10000.0f);
    ubo.proj[1][1] *= -1;
    ubo.light_color = glm::vec4(1.0f, 1.0f, 1.0f, 1);
    ubo.light_dir = glm::vec4(0, 0, 1, 0);
    ubo.cam_pos = camPos;
}

void App::drawStage() {
    uint32_t imageIndex;
    VkResult result = swapChain->acquireNextImage(&imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return;
    }
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    updateUniformBuffer();
    sceneParamsUniformBuffer[imageIndex]->writeToBuffer(&global_scene_info);
    sceneParamsUniformBuffer[imageIndex]->flush();

    recordCommandBuffer(imageIndex);

    result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasWindowResized()) {
        window.resetWindowResizedFlag();
        recreateSwapChain();
        return;
    }
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
}
