#include "LayoutModel.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <chrono>

#include "LayoutPEXData.h"

namespace lve {

	LayoutModel::LayoutModel(LveDevice& device, MODEL_TYPE model_type, const std::string& data_file) 
		: LveModel(device, model_type), layout_data{} {
		this->makeRenderingData(data_file);
		this->createBuffers();
	}

	LayoutModel::~LayoutModel() {
		this->layout_data.clear();
	}

	void LayoutModel::makeRenderingData(const std::string& file_path) {
		if (!std::filesystem::exists(file_path)) {
			std::cerr << "Error! " << file_path << " is not exist!!\n";
			return;
		}

		std::chrono::system_clock::time_point start, end;
		std::chrono::seconds run_time;

		start = std::chrono::system_clock::now();
		loadData(file_path);
		end = std::chrono::system_clock::now();
		run_time = std::chrono::duration_cast<std::chrono::seconds>(end - start);
		std::cout << "Run Time / LayoutModel / loadData [sec] : " << run_time << "\n";

		start = std::chrono::system_clock::now();
		makeCubeVertices();
		end = std::chrono::system_clock::now();
		run_time = std::chrono::duration_cast<std::chrono::seconds>(end - start);
		std::cout << "Run Time / LayoutModel / makeCubeVertices [sec] : " << run_time << "\n";

		start = std::chrono::system_clock::now();
		makeVertices();
		end = std::chrono::system_clock::now();
		run_time = std::chrono::duration_cast<std::chrono::seconds>(end - start);
		std::cout << "Run Time / LayoutModel / lmakeVertices [sec] : " << run_time << "\n";

		start = std::chrono::system_clock::now();
		makeIndices();
		end = std::chrono::system_clock::now();
		run_time = std::chrono::duration_cast<std::chrono::seconds>(end - start);
		std::cout << "Run Time / LayoutModel / makeIndices [sec] : " << run_time << "\n";
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
	}

	void LayoutModel::makeIndicesForEdge() {
		size_t cube_count = this->cube_vertices.size();

		std::vector<uint32_t>& indices = this->indices_edge;
		uint32_t start_idx = 0;
		for (int i = 0; i < cube_count; ++i, start_idx += 8) {
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

	
}//namespace lve
