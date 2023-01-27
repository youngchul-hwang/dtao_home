#include "simple_render_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <stdexcept>

namespace lve {

    struct SimplePushConstantData {
        glm::mat4 transform{ 1.f };
        alignas(16) glm::vec3 color{};
    };

    SimpleRenderSystem::SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass)
        : lveDevice{ device } {
        createPipelineLayout(this->pipelineLayoutForFace);
        createPipelineForFace(renderPass);
        createPipelineLayout(this->pipelineLayoutForEdge);
        createPipelineForEdge(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayoutForFace, nullptr);
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayoutForEdge, nullptr);
    }

    void SimpleRenderSystem::createPipelineLayout(VkPipelineLayout & pipeline_layout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipeline_layout) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::createPipelineForFace(VkRenderPass renderPass) {
        assert(pipelineLayoutForFace != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayoutForFace;
        this->lvePipelineForFace = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void SimpleRenderSystem::createPipelineForEdge(VkRenderPass renderPass) {
        assert(pipelineLayoutForEdge != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayoutForEdge;
        pipelineConfig.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        this->lvePipelineForEdge = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert_edge.spv",
            "shaders/simple_shader.frag_edge.spv",
            pipelineConfig);
    }

    void SimpleRenderSystem::renderGameObjects(
        VkCommandBuffer commandBuffer,
        std::vector<LveGameObject>& gameObjects,
        const LveCamera& camera) {

        auto projectionView = camera.getProjection() * camera.getView();
        
        //std::cout << "\n\nRenderGameObjects :: object count : " << gameObjects.size() << std::endl;
        //lvePipelineForFace->bind(commandBuffer);
        for (auto& obj : gameObjects) {
            lvePipelineForFace->bind(commandBuffer);
            SimplePushConstantData push{};
            push.color = obj.color;
            push.transform = projectionView * obj.transform.mat4();

            vkCmdPushConstants(
                commandBuffer,
                pipelineLayoutForFace,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
            obj.model->bindVertexBuffer(commandBuffer);
            obj.model->bindIndexBufferForFace(commandBuffer);
            obj.model->drawForFace(commandBuffer);
        }
        
        //lvePipelineForEdge->bind(commandBuffer);
        for (auto& obj : gameObjects) {
            lvePipelineForEdge->bind(commandBuffer);
            SimplePushConstantData push{};
            //push.color = obj.color;
            push.color = glm::vec3(0.67f,0.67f,0.67f);
            push.transform = projectionView * obj.transform.mat4();

            vkCmdPushConstants(
                commandBuffer,
                pipelineLayoutForEdge,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);
            obj.model->bindVertexBuffer(commandBuffer);
            obj.model->bindIndexBufferForEdge(commandBuffer);
            obj.model->drawForEdge(commandBuffer);
        }
    }

}  // namespace lve
