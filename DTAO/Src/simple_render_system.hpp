#pragma once

#include "lve_camera.hpp"
#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_pipeline.hpp"

// std
#include <memory>
#include <vector>

namespace lve {
    class SimpleRenderSystem {
    public:
        SimpleRenderSystem(LveDevice& device, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete;
        SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

        void renderGameObjects(
            VkCommandBuffer commandBuffer,
            std::vector<LveGameObject>& gameObjects,
            const LveCamera& camera);

        

    private:
        void createPipelineLayout(VkPipelineLayout & pipeline_layout);
        void createPipelineForFace(VkRenderPass renderPass);
        void createPipelineForEdge(VkRenderPass renderPass);
        void createPipelineForPEXResistor(VkRenderPass renderPass);
        void createPipelineForPEXCapacitor(VkRenderPass renderPass);

        LveDevice& lveDevice;

        std::unique_ptr<LvePipeline> lvePipelineForLayoutFace;
        VkPipelineLayout pipelineLayoutForLayoutFace;

        std::unique_ptr<LvePipeline> lvePipelineForLayoutEdge;
        VkPipelineLayout pipelineLayoutForLayoutEdge;

        std::unique_ptr<LvePipeline> lvePipelineForPEXResistor;
        VkPipelineLayout pipelineLayoutForPEXResistor;

        std::unique_ptr<LvePipeline> lvePipelineForPEXCapacitor;
        VkPipelineLayout pipelineLayoutForPEXCapacitor;
    };
}  // namespace lve
