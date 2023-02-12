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
        glm::float32 alpha;        
    };

    SimpleRenderSystem::SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass)
        : lveDevice{ device } {
        createPipelineLayout(this->pipelineLayoutForLayoutFace);
        createPipelineForFace(renderPass);
        createPipelineLayout(this->pipelineLayoutForLayoutEdge);
        createPipelineForEdge(renderPass);
        createPipelineLayout(this->pipelineLayoutForPEXResistor);
        createPipelineForPEXResistor(renderPass);
        createPipelineLayout(this->pipelineLayoutForPEXCapacitor);
        createPipelineForPEXCapacitor(renderPass);        
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayoutForLayoutFace, nullptr);
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayoutForLayoutEdge, nullptr);
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayoutForPEXResistor, nullptr);
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayoutForPEXCapacitor, nullptr);
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
        assert(pipelineLayoutForLayoutFace != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayoutForLayoutFace;
        
        pipelineConfig.colorBlendAttachment.blendEnable = VK_TRUE;
        pipelineConfig.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        pipelineConfig.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        
        this->lvePipelineForLayoutFace = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void SimpleRenderSystem::createPipelineForEdge(VkRenderPass renderPass) {
        assert(pipelineLayoutForLayoutEdge != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayoutForLayoutEdge;

        pipelineConfig.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

        this->lvePipelineForLayoutEdge = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert_edge.spv",
            "shaders/simple_shader.frag_edge.spv",
            pipelineConfig);
    }

    void SimpleRenderSystem::createPipelineForPEXResistor(VkRenderPass renderPass) {
        assert(pipelineLayoutForPEXResistor != nullptr && "Cannot create pipeline before pipeline layout");
        PipelineConfigInfo pipelineConfigResistor{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfigResistor);
        pipelineConfigResistor.renderPass = renderPass;
        pipelineConfigResistor.pipelineLayout = pipelineLayoutForPEXResistor;

        this->lvePipelineForPEXResistor = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert_pex.spv",
            "shaders/simple_shader.frag_pex.spv",
            pipelineConfigResistor);
    }

    void SimpleRenderSystem::createPipelineForPEXCapacitor(VkRenderPass renderPass) {
        assert(pipelineLayoutForPEXCapacitor != nullptr && "Cannot create pipeline before pipeline layout");
        PipelineConfigInfo pipelineConfigCapacitor{};
        LvePipeline::defaultPipelineConfigInfo(pipelineConfigCapacitor);
        pipelineConfigCapacitor.renderPass = renderPass;
        pipelineConfigCapacitor.pipelineLayout = pipelineLayoutForPEXCapacitor;
        
        pipelineConfigCapacitor.inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

        this->lvePipelineForPEXCapacitor = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert_pex.spv",
            "shaders/simple_shader.frag_pex.spv",
            pipelineConfigCapacitor);
    }

    void SimpleRenderSystem::renderGameObjects(
        VkCommandBuffer commandBuffer,
        std::vector<LveGameObject>& gameObjects,
        const LveCamera& camera) {

        auto projectionView = camera.getProjection() * camera.getView();
        
        //std::cout << "\n\nRenderGameObjects :: object count : " << gameObjects.size() << std::endl;
        //lvePipelineForFace->bind(commandBuffer);
        SimplePushConstantData push{};
        for (auto& obj : gameObjects) {
            
            //push.alpha = 1.0f;
            //push.color = obj.color;
            push.transform = projectionView * obj.transform.mat4();

            if (obj.model->getModelType() == MODEL_TYPE::MODEL_TYPE_LAYOUT && obj.model->getVisible()) {
                lvePipelineForLayoutFace->bind(commandBuffer);
                push.alpha = obj.model->getOpacity();;

                vkCmdPushConstants(
                    commandBuffer, pipelineLayoutForLayoutFace,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                    sizeof(SimplePushConstantData), &push);
                obj.model->bindVertexBuffer(commandBuffer);
                obj.model->bindIndexBufferForFace(commandBuffer);
                obj.model->drawForFace(commandBuffer);

                lvePipelineForLayoutEdge->bind(commandBuffer);
                push.alpha = 1.0f;
                push.color = glm::vec3(1.0f, 1.0f, 1.0f);
                vkCmdPushConstants(
                    commandBuffer, pipelineLayoutForLayoutEdge,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                    sizeof(SimplePushConstantData), &push);
                obj.model->bindVertexBuffer(commandBuffer);
                obj.model->bindIndexBufferForEdge(commandBuffer);
                obj.model->drawForEdge(commandBuffer);
            }

            if (obj.model->getModelType() == MODEL_TYPE::MODEL_TYPE_AXIS && obj.model->getVisible()) {
                lvePipelineForLayoutEdge->bind(commandBuffer);
                push.color = glm::vec3(1.0f, 1.0f, 1.0f);
                vkCmdPushConstants(
                    commandBuffer, pipelineLayoutForLayoutEdge,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                    sizeof(SimplePushConstantData), &push);
                obj.model->bindVertexBuffer(commandBuffer);
                obj.model->bindIndexBufferForEdge(commandBuffer);
                obj.model->drawForEdge(commandBuffer);
            }

            if(obj.model->getModelType() == MODEL_TYPE::MODEL_TYPE_PEX_RESISTOR && obj.model->getVisible()) {
                //*
                lvePipelineForPEXResistor->bind(commandBuffer);
                vkCmdPushConstants(
                    commandBuffer, pipelineLayoutForPEXResistor,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                    sizeof(SimplePushConstantData), &push);
                obj.model->bindVertexBuffer(commandBuffer);
                obj.model->bindIndexBufferForFace(commandBuffer);
                obj.model->drawForFace(commandBuffer);
                //*/
            }

            if (obj.model->getModelType() == MODEL_TYPE::MODEL_TYPE_PEX_CAPACITOR && obj.model->getVisible()) {
                //*
                lvePipelineForPEXCapacitor->bind(commandBuffer);
                vkCmdPushConstants(
                    commandBuffer, pipelineLayoutForPEXCapacitor,
                    VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0,
                    sizeof(SimplePushConstantData), &push);
                obj.model->bindVertexBuffer(commandBuffer);
                obj.model->bindIndexBufferForEdge(commandBuffer);
                obj.model->drawForEdge(commandBuffer);
                //*/
            }
        }
    }

}  // namespace lve
