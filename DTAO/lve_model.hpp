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

typedef enum MODEL_TYPE {
    MODEL_TYPE_LAYOUT = 0,
    MODEL_TYPE_PEX,
    MODEL_TYPE_AXIS,
    MODEL_TYPE_DEFAULT = 99
} MODEL_TYPE;

typedef enum LAYOUTINFO_INDEX {
    LAYOUTINFO_IDX_STRUCTURE = 0,
    LAYOUTINFO_IDX_CELLNAME = 1,
    LAYOUTINFO_IDX_LAYER = 2,
    LAYOUTINFO_IDX_DATATYPE = 3,
    LAYOUTINFO_IDX_LABELSTRING = 4,
    LAYOUTINFO_IDX_ZSTART = 5,
    LAYOUTINFO_IDX_ZEND = 6,
    LAYOUTINFO_IDX_NUMPOINTS = 7,
    LAYOUTINFO_IDX_X1 = 8,
    LAYOUTINFO_IDX_DEFAULT = -1,

} LAYOUTINFO_INDEX;

namespace lve {
    class LveModel {
    public:
        struct Vertex {
            glm::vec3 position{};
            glm::vec3 color{};

            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        struct rect_from_layout {
            float minx;
            float miny;
            float maxx;
            float maxy;
            float minz;
            float maxz;
        };

        struct coord3d {
            float x;
            float y;
            float z;
        };

        struct cube_vertex {
            coord3d vertex[8];
        };


        LveModel(LveDevice& device, MODEL_TYPE model_type, const std::string& layout_info_file = "");
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


    private:
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

    private:
        //std::string layout_info_file;
        std::vector<rect_from_layout> rects{};
        std::vector<cube_vertex> cubes{};

        std::vector<Vertex> vertices{};

        std::vector<uint32_t> indices_face{};
        std::vector<uint32_t> indices_edge{};

    public:
        //void setLayoutInfoFile(const std::string file_path) { this->layout_info_file = file_path; }
        void loadRenderingData(const std::string file_path);
        void loadPEXRenderingData(const std::string file_path);
        void makeAxisData();

        void makeRectFromLayoutInfo( const std::string& file_path );
        void makeCubeFromLayoutRect();

        void makeVerticesFromCube();
        void makeIndicesForFaceTriangle();
        void makeIndicesForEdgeLine();
                  
        MODEL_TYPE getModelType() { return this->model_type; }
        
    };
}  // namespace lve
