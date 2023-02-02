#pragma once

#include "lve_device.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// std
#include <vector>

#include <string>
#include <iostream>

#include "LayoutPEXData.h"

typedef enum MODEL_TYPE {
    MODEL_TYPE_LAYOUT = 0,
    MODEL_TYPE_PEX,
    MODEL_TYPE_AXIS,
    MODEL_TYPE_PEX_RESISTOR,
    MODEL_TYPE_PEX_CAPACITOR,
    MODEL_TYPE_DEFAULT = 99
} MODEL_TYPE;



namespace lve {
    class LveModel {
    public:
        struct Vertex {
            glm::vec3 position{};
            glm::vec3 color{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };
        
        LveModel(LveDevice& device, MODEL_TYPE model_type);
        ~LveModel();

        LveModel() = delete;
        LveModel(const LveModel&) = delete;
        LveModel& operator=(const LveModel&) = delete;

        //void bind(VkCommandBuffer commandBuffer);
        void bindVertexBuffer(VkCommandBuffer commandBuffer);
        void bindIndexBufferForFace(VkCommandBuffer commandBuffer);
        void bindIndexBufferForEdge(VkCommandBuffer commandBuffer);
        void drawForFace(VkCommandBuffer commandBuffer);
        void drawForEdge(VkCommandBuffer commandBuffer);


    public:
        void createBuffers();
        void destroyBuffers();
        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices, VkBuffer & buffer, VkDeviceMemory & memory);

        LveDevice& lveDevice;
        VkBuffer vertexBuffer{};
        VkDeviceMemory vertexBufferMemory{};
        uint32_t vertexCount{};

        VkBuffer indexBufferForFace{};
        VkDeviceMemory indexBufferMemoryForFace{};
        
        VkBuffer indexBufferForEdge{};
        VkDeviceMemory indexBufferMemoryForEdge{};
        
        MODEL_TYPE model_type{};

    public:
        //std::string layout_info_file;
        std::vector<cube_info> cube_infos{};
        std::vector<cube_vertex> cube_vertices{};

        std::vector<Vertex> vertices{};

        std::vector<uint32_t> indices_face{};
        std::vector<uint32_t> indices_edge{};

    public:
        MODEL_TYPE getModelType() { return this->model_type; }

    public:
        //void setLayoutInfoFile(const std::string file_path) { this->layout_info_file = file_path; }
        virtual void makeRenderingData(const std::string& file_path = "") {};
        virtual void loadData(const std::string& file_path = "") {};
        virtual void makeVertices() {};
        virtual void makeIndices() {};

        void makeAxisData(const float axis_length = (2.0f) );
    };
}  // namespace lve
