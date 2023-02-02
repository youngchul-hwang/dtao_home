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
        void createPipelineForPEX(VkRenderPass renderPass);

        LveDevice& lveDevice;

        std::unique_ptr<LvePipeline> lvePipelineForFace;
        VkPipelineLayout pipelineLayoutForFace;

        std::unique_ptr<LvePipeline> lvePipelineForEdge;
        VkPipelineLayout pipelineLayoutForEdge;

        std::unique_ptr<LvePipeline> lvePipelineForPEX;
        VkPipelineLayout pipelineLayoutForPEX;
    };
}  // namespace lve
