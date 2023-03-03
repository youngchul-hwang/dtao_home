
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <QFile>

#include "LayoutModel.h"
#include "LayoutPEXData.h"


LayoutModel::LayoutModel(LveDevice& device, MODEL_TYPE model_type, const std::string& data_file)
    : LveModel(device, model_type), layout_data{} {
    this->makeRenderingData(data_file);
    this->createBuffers();
}

LayoutModel::~LayoutModel() {
    this->layout_data.clear();
}

void LayoutModel::makeRenderingData(const std::string& file_path) {
    if (!QFile::exists(file_path.c_str())) {
        std::cerr << "Error! " << file_path << " is not exist!!\n";
        return;
    }

    loadData(file_path);
    makeCubeVertices();
    makeVertices();
    makeIndices();
}

void LayoutModel::loadData(const std::string& file_path) {
    this->layout_data.loadLayoutData(file_path);
}

void LayoutModel::makeCubeVertices() {
    std::vector<LayoutItem>& layout_items =  this->layout_data.getPatterns();
    std::vector<LayoutItem>::iterator it = layout_items.begin();
    cube_vertex cur_cube_vertices;

    for (it = layout_items.begin(); it != layout_items.end(); ++it) {
        cur_cube_vertices.vertex[0] = { it->pattern.minx, it->pattern.maxy, it->pattern.maxz };
        cur_cube_vertices.vertex[1] = { it->pattern.minx, it->pattern.miny, it->pattern.maxz };
        cur_cube_vertices.vertex[2] = { it->pattern.maxx, it->pattern.miny, it->pattern.maxz };
        cur_cube_vertices.vertex[3] = { it->pattern.maxx, it->pattern.maxy, it->pattern.maxz };

        cur_cube_vertices.vertex[4] = { it->pattern.minx, it->pattern.maxy, it->pattern.minz };
        cur_cube_vertices.vertex[5] = { it->pattern.minx, it->pattern.miny, it->pattern.minz };
        cur_cube_vertices.vertex[6] = { it->pattern.maxx, it->pattern.miny, it->pattern.minz };
        cur_cube_vertices.vertex[7] = { it->pattern.maxx, it->pattern.maxy, it->pattern.minz };

        this->cube_vertices.push_back(cur_cube_vertices);
    }
}

void LayoutModel::makeVertices() {
    Vertex temp_vertex;

    float up_color = 0.15f;
    float down_color = 0.6f;

    for (const auto& cur_cube : this->cube_vertices) {
        for (int i = 0; i < 8; ++i) {
            temp_vertex.position = { cur_cube.vertex[i].x, cur_cube.vertex[i].y, cur_cube.vertex[i].z };

            if (i < 4) temp_vertex.color = { up_color , up_color , up_color };
            else       temp_vertex.color = { down_color , down_color , down_color };

            vertices.push_back(temp_vertex);
        }
    }
}

void LayoutModel::makeIndices() {
    makeIndicesForFace();
    makeIndicesForEdge();
}

void LayoutModel::makeIndicesForFace() {
    size_t cube_count = this->cube_vertices.size();

    size_t start_idx = 0;
    std::vector<uint32_t>& indices = this->indices_face;
    for (size_t i = 0; i < cube_count; ++i) {
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
}

void LayoutModel::makeIndicesForEdge() {
    size_t cube_count = this->cube_vertices.size();

    std::vector<uint32_t>& indices = this->indices_edge;
    size_t start_idx = 0;
    for (size_t i = 0; i < cube_count; ++i, start_idx += 8) {
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
}



