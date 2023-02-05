#include "lve_model.hpp"
#include "rapidcsv.h"

// std
#include <cassert>
#include <cstring>

#include<filesystem>


using namespace std;

inline float coord_normalize(float in_value, float move, float scale) {
    //return 2 * (in_value - min_value) / (max_value - min_value) - 1.0f;
    return (2 * (in_value - move) / (scale)-1.0f);
}

namespace lve {
    
    LveModel::LveModel(LveDevice& device, MODEL_TYPE type) 
        : lveDevice{ device }, model_type(type) {
        if (type == MODEL_TYPE::MODEL_TYPE_AXIS) {
            makeAxisData();
            createBuffers();
        }
   
        //createBuffers();       
    }

    LveModel::~LveModel() {
        destroyBuffers();

        this->cube_infos.clear();
        this->cube_vertices.clear();
        this->vertices.clear();
        this->indices_face.clear();
        this->indices_edge.clear();
    }

    void LveModel::createBuffers() {
        createVertexBuffers(this->vertices);

        if (this->model_type == MODEL_TYPE::MODEL_TYPE_LAYOUT) {
            createIndexBuffers(this->indices_face, this->indexBufferForFace, this->indexBufferMemoryForFace);
            createIndexBuffers(this->indices_edge, this->indexBufferForEdge, this->indexBufferMemoryForEdge);
        }
        else if (this->model_type == MODEL_TYPE::MODEL_TYPE_AXIS) {
            createIndexBuffers(this->indices_edge, this->indexBufferForEdge, this->indexBufferMemoryForEdge);
        }
        else if (this->model_type == MODEL_TYPE::MODEL_TYPE_PEX_RESISTOR) {
            createIndexBuffers(this->indices_face, this->indexBufferForFace, this->indexBufferMemoryForFace);
        }
        else if (this->model_type == MODEL_TYPE::MODEL_TYPE_PEX_CAPACITOR) {
            createIndexBuffers(this->indices_edge, this->indexBufferForEdge, this->indexBufferMemoryForEdge);
        }
    }

    void LveModel::destroyBuffers() {
        vkDestroyBuffer(lveDevice.device(), vertexBuffer, nullptr);
        vkFreeMemory(lveDevice.device(), vertexBufferMemory, nullptr);
       
        if (this->model_type == MODEL_TYPE::MODEL_TYPE_LAYOUT) {
            vkDestroyBuffer(lveDevice.device(), indexBufferForFace, nullptr);
            vkFreeMemory(lveDevice.device(), indexBufferMemoryForFace, nullptr);
            vkDestroyBuffer(lveDevice.device(), indexBufferForEdge, nullptr);
            vkFreeMemory(lveDevice.device(), indexBufferMemoryForEdge, nullptr);
        }
        else if (this->model_type == MODEL_TYPE::MODEL_TYPE_AXIS) {
            vkDestroyBuffer(lveDevice.device(), indexBufferForEdge, nullptr);
            vkFreeMemory(lveDevice.device(), indexBufferMemoryForEdge, nullptr);
        }
        else if (this->model_type == MODEL_TYPE::MODEL_TYPE_PEX_RESISTOR) {
            vkDestroyBuffer(lveDevice.device(), indexBufferForFace, nullptr);
            vkFreeMemory(lveDevice.device(), indexBufferMemoryForFace, nullptr);
        }
        else if (this->model_type == MODEL_TYPE::MODEL_TYPE_PEX_CAPACITOR) {
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

    void LveModel::makeAxisData(const float axis_length) {
        Vertex temp_vertex;
        this->vertices.clear();
        this->vertices = {
            { { -axis_length, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f } },
            { {  axis_length, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f } },
            { { 0.0f, -axis_length, 0.0f }, { 1.0f, 1.0f, 1.0f } },
            { { 0.0f,  axis_length, 0.0f }, { 0.0f, 1.0f, 0.0f } },
            { { 0.0f, 0.0f, -axis_length }, { 1.0f, 1.0f, 1.0f } },
            { { 0.0f, 0.0f,  axis_length }, { 0.0f, 0.0f, 1.0f } }
        };

        vector<uint32_t>& indices = this->indices_edge;
        indices.clear();
        indices.push_back(0); 
        indices.push_back(1);
        indices.push_back(2); 
        indices.push_back(3);
        indices.push_back(4); 
        indices.push_back(5);
    }

    void LveModel::updateOpacity(float amount) {
        this->opacity += amount;
        if (this->opacity > 1.0f) this->opacity = 1.0f;
        if (this->opacity < 0.0f) this->opacity = 0.0f;
    }
}  // namespace lve
