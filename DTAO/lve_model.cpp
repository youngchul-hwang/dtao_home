#include "lve_model.hpp"
#include "rapidcsv.h"

// std
#include <cassert>
#include <cstring>


using namespace std;

inline float coord_normalize(float in_value, float move, float scale) {
    //return 2 * (in_value - min_value) / (max_value - min_value) - 1.0f;
    return (2 * (in_value - move) / (scale)-1.0f);
}

namespace lve {
    LveModel::LveModel(LveDevice& device, MODEL_TYPE type) : lveDevice{ device }, model_type(type) {
    }

    

    LveModel::LveModel(LveDevice& device, const std::string& layout_info_file) : lveDevice{ device } {
        loadRenderingData(layout_info_file);
        createBuffers();       
    }

    LveModel::~LveModel() {
        destroyBuffers();

        this->rects.clear();
        this->cubes.clear();
        this->vertices.clear();
        this->indices_face.clear();
        this->indices_edge.clear();
    }

    void LveModel::createBuffers() {
        createVertexBuffers(this->vertices);

        if (this->model_type == MODEL_TYPE_EDGE_ONLY) {
            createIndexBuffers(this->indices_edge, this->indexBufferForEdge, this->indexBufferMemoryForEdge);
        }
        else if (this->model_type == MODEL_TYPE_FACE_ONLY) {
            createIndexBuffers(this->indices_face, this->indexBufferForFace, this->indexBufferMemoryForFace);
        }
        else if (this->model_type == MODEL_TYPE_FACE_EDGE) {
            createIndexBuffers(this->indices_face, this->indexBufferForFace, this->indexBufferMemoryForFace);
            createIndexBuffers(this->indices_edge, this->indexBufferForEdge, this->indexBufferMemoryForEdge);
        }
    }

    void LveModel::destroyBuffers() {
        vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);

        if (this->model_type == MODEL_TYPE_EDGE_ONLY) {
            vkDestroyBuffer(lveDevice.device(), indexBufferForEdge, nullptr);
            vkFreeMemory(lveDevice.device(), indexBufferMemoryForEdge, nullptr);
        }
        else if (this->model_type == MODEL_TYPE_FACE_ONLY) {
            vkDestroyBuffer(lveDevice.device(), indexBufferForFace, nullptr);
            vkFreeMemory(lveDevice.device(), indexBufferMemoryForFace, nullptr);
        }
        else if (this->model_type == MODEL_TYPE_FACE_EDGE) {
            vkDestroyBuffer(lveDevice.device(), indexBufferForFace, nullptr);
            vkFreeMemory(lveDevice.device(), indexBufferMemoryForFace, nullptr);

            vkDestroyBuffer(lveDevice.device(), indexBufferForEdge, nullptr);
            vkFreeMemory(lveDevice.device(), indexBufferMemoryForEdge, nullptr);
        }

    }
    void LveModel::createVertexBuffers(const std::vector<Vertex>& vertices) {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        lveDevice.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(lveDevice.device(), stagingBufferMemory);

        lveDevice.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer,
            vertexBufferMemory);

        lveDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        vkDestroyBuffer(lveDevice.device(), stagingBuffer, nullptr);
        vkFreeMemory(lveDevice.device(), stagingBufferMemory, nullptr);
    }

    void LveModel::createIndexBuffers(
        const std::vector<uint32_t>& indices,
        VkBuffer& buffer,
        VkDeviceMemory& memory) {
        assert( !indices.empty() && "Index size can't be zero");
        VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

        std::cout << "CreateIndexBuffers :: indices size : " << indices.size() << std::endl;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        lveDevice.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(lveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t)bufferSize);
        vkUnmapMemory(lveDevice.device(), stagingBufferMemory);

        lveDevice.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            buffer,
            memory);

        lveDevice.copyBuffer(stagingBuffer, buffer, bufferSize);

        vkDestroyBuffer(lveDevice.device(), stagingBuffer, nullptr);
        vkFreeMemory(lveDevice.device(), stagingBufferMemory, nullptr);
    }

    void LveModel::drawForFace(VkCommandBuffer commandBuffer) {
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(this->indices_face.size()), 1, 0, 0, 0);
    }

    void LveModel::drawForEdge(VkCommandBuffer commandBuffer) {
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(this->indices_edge.size()), 1, 0, 0, 0);
    }

    void LveModel::bindVertexBuffer(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void LveModel::bindIndexBufferForFace(VkCommandBuffer commandBuffer) {
        vkCmdBindIndexBuffer(commandBuffer, this->indexBufferForFace, 0, VK_INDEX_TYPE_UINT32);
    }

    void LveModel::bindIndexBufferForEdge(VkCommandBuffer commandBuffer) {
        vkCmdBindIndexBuffer(commandBuffer, this->indexBufferForEdge, 0, VK_INDEX_TYPE_UINT32);
    }

    /*
    void LveModel::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = { vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, this->indexBufferForFace, 0, VK_INDEX_TYPE_UINT32);
    }
    */

    std::vector<VkVertexInputBindingDescription> LveModel::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> LveModel::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);
        return attributeDescriptions;
    }

    void LveModel::loadRenderingData(const std::string file_path) {
        this->makeRectFromLayoutInfo(file_path);
        this->makeCubeFromLayoutRect();
        this->makeVerticesFromCube();
        this->makeIndicesForFaceTriangle();
        this->makeIndicesForEdgeLine();
    }

    void LveModel::makeRectFromLayoutInfo(const std::string& file_path) {
        rapidcsv::Document infile(file_path, rapidcsv::LabelParams(-1, -1));

        std::vector<float> bbox = infile.GetRow<float>(0);
        float bbox_min_x = bbox[0];
        float bbox_min_y = bbox[1];
        float bbox_max_x = bbox[2];
        float bbox_max_y = bbox[3];
        float bbox_min_z = bbox[4];
        float bbox_max_z = bbox[5];

        float x_diff = bbox_max_x - bbox_min_x;
        float y_diff = bbox_max_y - bbox_min_y;
        float z_diff = bbox_max_z - bbox_min_z;

        float max_diff = x_diff > y_diff ? x_diff : y_diff;
        max_diff = max_diff > z_diff ? max_diff : z_diff;
        float scale = 1.0f / max_diff;

        size_t line_count = infile.GetRowCount();
        rect_from_layout cur_rect;
        for (int i = 0; i < line_count; ++i) {
            vector<string> line = infile.GetRow<string>(i);
            if (line[LAYOUTINFO_IDX_STRUCTURE] != "Poly") continue;
            if (std::stoi(line[LAYOUTINFO_IDX_NUMPOINTS]) != 4) continue;

            cur_rect.minz = std::stof(line[LAYOUTINFO_IDX_ZSTART]);
            cur_rect.maxz = std::stof(line[LAYOUTINFO_IDX_ZEND]);
            if (cur_rect.minz == cur_rect.maxz) continue;

            cur_rect.minx = min(std::stof(line[LAYOUTINFO_IDX_X1]), std::stof(line[LAYOUTINFO_IDX_X1 + 4]));
            cur_rect.maxx = max(std::stof(line[LAYOUTINFO_IDX_X1]), std::stof(line[LAYOUTINFO_IDX_X1 + 4]));
            cur_rect.miny = min(std::stof(line[LAYOUTINFO_IDX_X1 + 1]), std::stof(line[LAYOUTINFO_IDX_X1 + 5]));
            cur_rect.maxy = max(std::stof(line[LAYOUTINFO_IDX_X1 + 1]), std::stof(line[LAYOUTINFO_IDX_X1 + 5]));

            //cur_rect.minx = coord_normalize(cur_rect.minx, bbox_min_x, scale);
            //cur_rect.maxx = coord_normalize(cur_rect.maxx, bbox_min_x, scale);
            //cur_rect.miny = coord_normalize(cur_rect.miny, bbox_min_y, scale);
            //cur_rect.maxy = coord_normalize(cur_rect.maxy, bbox_min_y, scale);
            //cur_rect.minz = coord_normalize(cur_rect.minz, bbox_min_z, scale);
            //cur_rect.maxz = coord_normalize(cur_rect.maxz, bbox_min_z, scale);

            this->rects.push_back(cur_rect);
        }

        infile.Clear();
    }

    void LveModel::makeCubeFromLayoutRect() {
        vector<rect_from_layout>::const_iterator it;
        cube_vertex cur_cube;

        for (it = rects.begin(); it != rects.end(); ++it) {
            cur_cube.vertex[0] = { it->minx, it->maxy, it->maxz };
            cur_cube.vertex[1] = { it->minx, it->miny, it->maxz };
            cur_cube.vertex[2] = { it->maxx, it->miny, it->maxz };
            cur_cube.vertex[3] = { it->maxx, it->maxy, it->maxz };

            cur_cube.vertex[4] = { it->minx, it->maxy, it->minz };
            cur_cube.vertex[5] = { it->minx, it->miny, it->minz };
            cur_cube.vertex[6] = { it->maxx, it->miny, it->minz };
            cur_cube.vertex[7] = { it->maxx, it->maxy, it->minz };

            cubes.push_back(cur_cube);
        }
    }

    void LveModel::makeVerticesFromCube() {
        Vertex temp_vertex;

        float up_color = 0.15f;
        float down_color = 0.6f;
        float temp_color[3] = { 0.0f, 0.0f, 0.0f };

        float edge_color = 0.15f;

        for (const auto& cur_cube : this->cubes) {
            for (int i = 0; i < 8; ++i) {

                temp_vertex.position = { cur_cube.vertex[i].x, cur_cube.vertex[i].y, cur_cube.vertex[i].z };
                //temp_vertex.color_edge = { edge_color, edge_color, edge_color };
                if (i < 4) temp_vertex.color = { up_color , up_color , up_color };
                else       temp_vertex.color = { down_color , down_color , down_color };

                vertices.push_back(temp_vertex);
            }
        }

//*
        temp_vertex = { { -2.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } };
        vertices.push_back(temp_vertex);
        temp_vertex = { { 2.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } };
        vertices.push_back(temp_vertex);

        temp_vertex = { { 0.0f, -2.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } };
        vertices.push_back(temp_vertex);
        temp_vertex = { { 0.0f, 2.0f, 0.0f }, { 0.0f, 1.0f, 0.0f } };
        vertices.push_back(temp_vertex);

        temp_vertex = { { 0.0f, 0.0f, -2.0f }, { 1.0f, 1.0f, 1.0f } };
        vertices.push_back(temp_vertex);
        temp_vertex = { { 0.0f, 0.0f, 2.0f }, { 0.0f, 0.0f, 1.0f } };
        vertices.push_back(temp_vertex);
//*/
    }

    void LveModel::makeIndicesForFaceTriangle() {

        size_t cube_count = this->cubes.size();

        std::cout << "\nMake Indices For Face Triangle\n";
        std::cout << "\tCube count : " << this->cubes.size() << std::endl;

        uint32_t start_idx = 0;
        std::vector<uint32_t>& indices = this->indices_face;
        for (int i = 0; i < cube_count; ++i) {
            start_idx = 8 * i;
            indices.push_back(start_idx + 0); indices.push_back(start_idx + 1); indices.push_back(start_idx + 2);
            indices.push_back(start_idx + 2); indices.push_back(start_idx + 3); indices.push_back(start_idx + 0);//top

            indices.push_back(start_idx + 3); indices.push_back(start_idx + 2); indices.push_back(start_idx + 6);
            indices.push_back(start_idx + 6); indices.push_back(start_idx + 7); indices.push_back(start_idx + 3);//right

            indices.push_back(start_idx + 0); indices.push_back(start_idx + 3); indices.push_back(start_idx + 7);
            indices.push_back(start_idx + 7); indices.push_back(start_idx + 4); indices.push_back(start_idx + 0);//front

            indices.push_back(start_idx + 4); indices.push_back(start_idx + 7); indices.push_back(start_idx + 6);
            indices.push_back(start_idx + 6); indices.push_back(start_idx + 5); indices.push_back(start_idx + 4);//bottom

            indices.push_back(start_idx + 0); indices.push_back(start_idx + 4); indices.push_back(start_idx + 5);
            indices.push_back(start_idx + 5); indices.push_back(start_idx + 1); indices.push_back(start_idx + 0);//left

            indices.push_back(start_idx + 1); indices.push_back(start_idx + 5); indices.push_back(start_idx + 6);
            indices.push_back(start_idx + 6); indices.push_back(start_idx + 2); indices.push_back(start_idx + 1);//back
        }

        std::cout << "\tIndices count : " << indices.size() << std::endl;
    }

    void LveModel::makeIndicesForEdgeLine() {
        size_t cube_count = this->cubes.size();

        std::cout << "\nMake Indices For Edge Line\n";
        std::cout << "\tCube count : " << this->cubes.size() << std::endl;

        
        std::vector<uint32_t>& indices = this->indices_edge;
        uint32_t start_idx = 0;
        for (int i = 0; i < cube_count; ++i, start_idx += 8) {
            //start_idx = 8 * i;
            //top
            indices.push_back(start_idx + 0); indices.push_back(start_idx + 1);
            indices.push_back(start_idx + 1); indices.push_back(start_idx + 2);
            indices.push_back(start_idx + 2); indices.push_back(start_idx + 3);
            indices.push_back(start_idx + 3); indices.push_back(start_idx + 0);

            //bottom
            indices.push_back(start_idx + 4); indices.push_back(start_idx + 7);
            indices.push_back(start_idx + 7); indices.push_back(start_idx + 6);
            indices.push_back(start_idx + 6); indices.push_back(start_idx + 5);
            indices.push_back(start_idx + 5); indices.push_back(start_idx + 4);

            //right
            indices.push_back(start_idx + 3); indices.push_back(start_idx + 2);
            indices.push_back(start_idx + 2); indices.push_back(start_idx + 6);
            indices.push_back(start_idx + 6); indices.push_back(start_idx + 7);
            indices.push_back(start_idx + 7); indices.push_back(start_idx + 3);

            //left
            indices.push_back(start_idx + 0); indices.push_back(start_idx + 4);
            indices.push_back(start_idx + 4); indices.push_back(start_idx + 5);
            indices.push_back(start_idx + 5); indices.push_back(start_idx + 1);
            indices.push_back(start_idx + 1); indices.push_back(start_idx + 0);

            //front
            indices.push_back(start_idx + 0); indices.push_back(start_idx + 3);
            indices.push_back(start_idx + 3); indices.push_back(start_idx + 7);
            indices.push_back(start_idx + 7); indices.push_back(start_idx + 4);
            indices.push_back(start_idx + 4); indices.push_back(start_idx + 0);

            //back
            indices.push_back(start_idx + 1); indices.push_back(start_idx + 5);
            indices.push_back(start_idx + 5); indices.push_back(start_idx + 6);
            indices.push_back(start_idx + 6); indices.push_back(start_idx + 2);
            indices.push_back(start_idx + 2); indices.push_back(start_idx + 1);
        }

        std::cout << "\tEdge Indices count : " << indices.size() << std::endl;

        int axis_index = start_idx;

        indices.push_back(axis_index + 0); indices.push_back(axis_index + 1);
        indices.push_back(axis_index + 2); indices.push_back(axis_index + 3);
        indices.push_back(axis_index + 4); indices.push_back(axis_index + 5);
        
        std::cout << "\tEdge Indices count including Axis Line : " << indices.size() << std::endl;
    }

}  // namespace lve
