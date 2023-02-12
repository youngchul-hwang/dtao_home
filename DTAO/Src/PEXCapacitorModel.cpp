#include "PEXCapacitorModel.h"

#include <iostream>
#include <map>
#include <vector>
#include <math.h>

#include "LayoutPEXData.h"

using namespace std;

namespace lve {
    PEXCapacitorModel::PEXCapacitorModel(
        LveDevice& device, 
        MODEL_TYPE model_type, 
        const std::string& data_file,
        LayoutDataManager* layout_data_)
        : LveModel(device, model_type), pex_data{}, cap_node_name_to_index_map{}, layout_data{layout_data_} {
        
        this->makePatternCapsFromLayoutData();
        this->makeRenderingData(data_file);
        this->createBuffers();
    }


    PEXCapacitorModel::~PEXCapacitorModel() {
        for (auto& cur_item : this->layer_to_pattern_cap_map) cur_item.second.clear();
        this->layer_to_pattern_cap_map.clear();
        this->pattern_caps.clear();
    }

    void PEXCapacitorModel::makePatternCapsFromLayoutData() {
        std::vector<LayoutItem>& layout_items = this->layout_data->getPatterns();
        if (layout_items.empty()) {            
            std::cerr << "Error! empty patten @PEXCapacitorModel::makePatternWCapsFromLayoutData()\n";
        }

        for (auto& layout_item : layout_items) {
            this->pattern_caps.push_back(
                pattern_cap(layout_item.pattern, layout_item.layer_number, layout_item.layer_datatype, 0.0, 0.0, 0)
            );
            this->layers.insert({ layout_item.layer_number, layout_item.layer_datatype });
        }        
    }
    
    void PEXCapacitorModel::makeRenderingData(const std::string& file_path) {
        loadData(file_path);
        makeCapNodesFromPEXData();

        attachCapToPattern();
        normalizePatternCap();
        printLayerToPatternCapMap("After Normalization Cap");

        makeCubeVertices();
        makeVertices();
        makeIndices();

        for (auto& cur_item : this->layer_to_cap_node_map) cur_item.second.clear();
        this->layer_to_cap_node_map.clear();
        this->cap_nodes.clear();
        this->layers.clear();
    }

    void PEXCapacitorModel::loadData(const std::string& file_path) {
        this->pex_data.loadData(file_path);
        //this->pex_data.printPEXData();
    }


    void PEXCapacitorModel::makeCapNodesFromPEXData() {
        this->cap_node_name_to_index_map.clear();
        std::map<std::string, size_t>::iterator it;
        for (auto & pex_item : this->pex_data.getCapacitors()) {
            //node1
            std::string& node1_name = pex_item.node1.name;
            it = this->cap_node_name_to_index_map.find(node1_name);
            if (it == this->cap_node_name_to_index_map.end()) addNewCapNode(pex_item.node1, pex_item.value);
            else {
                this->cap_nodes[it->second].value += pex_item.value;
                this->cap_nodes[it->second].connected_count++;
            }

            //node2
            std::string& node2_name = pex_item.node2.name;
            it = this->cap_node_name_to_index_map.find(node2_name);
            if (it == this->cap_node_name_to_index_map.end()) addNewCapNode(pex_item.node2, pex_item.value);
            else {
                this->cap_nodes[it->second].value += pex_item.value;
                this->cap_nodes[it->second].connected_count++;
            }
        }//for(auto & pex_item : this->pex_data.getCapacitors())
        this->pex_data.clear();
        this->cap_node_name_to_index_map.clear();
    }     


    void PEXCapacitorModel::addNewCapNode(pex_node& pex_node, double value){
        cap_node new_cap_node{};
        new_cap_node.name = pex_node.name;
        new_cap_node.layer_number = pex_node.layer_number;
        new_cap_node.layer_datatype = pex_node.layer_datatype;
        new_cap_node.x = pex_node.x;
        new_cap_node.y = pex_node.y;
        new_cap_node.value = value;
        new_cap_node.connected_count = 1;

        this->cap_nodes.push_back(new_cap_node);
        this->cap_node_name_to_index_map.insert(std::pair<std::string, size_t>(new_cap_node.name, this->cap_nodes.size()-1));
    }


    void PEXCapacitorModel::attachCapToPattern() {
        makeLayerToCapNodeMap();
        makeLayerToPatternCapMap();

        
        //병렬처리로 변경해야 함
        //matchCapWithPattern(this->cap_layer_map, this->pattern_layer_map, 17, 0);
        for (auto& cur_layer : this->layers) {
            string layer = getLayerString(cur_layer.first, cur_layer.second);
            map<string, vector<cap_node*>>::iterator it_layer_to_cap_node_map = this->layer_to_cap_node_map.find(layer);
            map<string, vector<pattern_cap*>>::iterator it_layer_to_pattern_cap_map = this->layer_to_pattern_cap_map.find(layer);
            if (it_layer_to_cap_node_map == this->layer_to_cap_node_map.end()
                || it_layer_to_pattern_cap_map == this->layer_to_pattern_cap_map.end()) continue;;

            std::vector<cap_node*>& caps = it_layer_to_cap_node_map->second;
            std::vector<pattern_cap*>& patterns = it_layer_to_pattern_cap_map->second;

            matchCapWithPattern(caps, patterns);
            //matchCapWithPattern(this->layer_to_cap_node_map, this->layer_to_pattern_cap_map, cur_layer.first, cur_layer.second);
        }
    }

    void PEXCapacitorModel::matchCapWithPattern(
        std::vector<cap_node*>& caps,
        std::vector<pattern_cap*>& patterns
        ) {

        for (auto& cap : caps) {
            for (auto& pattern : patterns) {            
                if (isPatternIncludeCap(*pattern, *cap)) {
                    pattern->cap_count++;
                    pattern->cap_value += cap->value;
                    break;
                }
            }//for cap : caps
        }//for pattern : patterns

    }

    void PEXCapacitorModel::matchCapWithPattern(
        std::map<std::string, std::vector<cap_node*>>& cap_layer_map_,
        std::map<std::string, std::vector<pattern_cap*>>& pattern_layer_map_,
        uint target_layer_number, uint target_layer_datatype) {

        //string layer = std::to_string(target_layer_number) + "." + std::to_string(target_layer_datatype);
        string layer = getLayerString(target_layer_number, target_layer_datatype);
        std::map<std::string, std::vector<cap_node*>>::iterator it_cap_layer_map = cap_layer_map_.find(layer);
        std::map<std::string, std::vector<pattern_cap*>>::iterator it_pattern_layer_map = pattern_layer_map_.find(layer);
        if (it_cap_layer_map == cap_layer_map_.end() || it_pattern_layer_map == pattern_layer_map_.end()) return;

        std::vector<cap_node*>& caps = it_cap_layer_map->second;
        std::vector<pattern_cap*>& patterns = it_pattern_layer_map->second;

        for (auto& cap : caps) {
            for (auto& pattern : patterns) {
                if (isPatternIncludeCap(*pattern, *cap)) {
                    pattern->cap_count++;
                    pattern->cap_value += cap->value;
                    break;
                }
            }//for cap : caps            
        }//for pattern : patterns
    }

    void PEXCapacitorModel::makeLayerToCapNodeMap() {
        this->layer_to_cap_node_map.clear();
        map<string, vector<cap_node*>>::iterator it;
        for (auto& cur_cap : this->cap_nodes) {
            //std::string layer = std::to_string(cur_cap.layer_number) + "." + std::to_string(cur_cap.layer_datatype);
            string layer = getLayerString(cur_cap.layer_number, cur_cap.layer_datatype);
            it = this->layer_to_cap_node_map.find(layer);
            if (it == this->layer_to_cap_node_map.end()) {
                vector<cap_node*> * new_cap_list = new vector<cap_node*>;
                new_cap_list->push_back(&cur_cap);
                this->layer_to_cap_node_map.insert(pair<string, vector<cap_node*>>(layer, *new_cap_list));
            }
            else {
                it->second.push_back(&cur_cap);
            }
        }//for (auto& cur_cap : this->cap_nodes)

        //printLayerToCapNodeMap();
    }


    void PEXCapacitorModel::makeLayerToPatternCapMap() {
        this->layer_to_pattern_cap_map.clear();
        map<string, vector<pattern_cap*>>::iterator it;
        for (auto& cur_pattern : this->pattern_caps) {
            //string layer = std::to_string(cur_pattern.layer_number) + "." + std::to_string(cur_pattern.layer_datatype);
            string layer = getLayerString(cur_pattern.layer_number, cur_pattern.layer_datatype);
            it = this->layer_to_pattern_cap_map.find(layer);
            if (it == this->layer_to_pattern_cap_map.end()) {
                vector<pattern_cap*> * new_pattern_list = new vector<pattern_cap*>;
                new_pattern_list->push_back(&cur_pattern);
                this->layer_to_pattern_cap_map.insert(pair<string, vector<pattern_cap*>>(layer, *new_pattern_list));
            }
            else {
                it->second.push_back(&cur_pattern);
            }
        }//for (auto& cur_pattern : this->pattern_caps)

        //printLayerToPatternCapMap();
    }


    void PEXCapacitorModel::normalizePatternCap() {
        for (auto& cur_pattern_cap : this->pattern_caps) {
            if (cur_pattern_cap.cap_value > this->max_cap) this->max_cap = cur_pattern_cap.cap_value;
        }

        for (auto& cur_pattern_cap : this->pattern_caps) {
            cur_pattern_cap.normalized_cap_value = 1.0 / (std::log((cur_pattern_cap.cap_value / this->max_cap))*(-1.0) + 1.0);
        }
    }


    inline std::string PEXCapacitorModel::getLayerString(uint layer_number, uint layer_datatype) {
        return (std::string(std::to_string(layer_number) + "." + std::to_string(layer_datatype)));
    }

 
    bool PEXCapacitorModel::isPatternIncludeCap(const pattern_cap& pattern, const cap_node& cap) {
        if (cap.x < pattern.pattern.minx) return false;
        if (cap.x > pattern.pattern.maxx) return false;
        if (cap.y < pattern.pattern.miny) return false;
        if (cap.y > pattern.pattern.maxy) return false;
        return true;
    }
   
    void PEXCapacitorModel::makeCubeVertices() {
        cube_vertex cur_cube_vertices;
        std::vector<pattern_cap>::iterator it;
        for (it = this->pattern_caps.begin(); it != this->pattern_caps.end(); ++it) {
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

    void PEXCapacitorModel::makeVertices() {
        Vertex temp_vertex;
        float cap_color_r{}, cap_color_g{}, cap_color_b{};
        
        std::vector<pattern_cap>::iterator cur_pattern_cap = this->pattern_caps.begin();
        for (const auto& cur_cube : this->cube_vertices) {
            cap_color_r = cur_pattern_cap->normalized_cap_value;
            cap_color_g = 0.0f; // cur_pattern_cap->normalized_cap_value;
            cap_color_b = 0.0f;

            for (int i = 0; i < 8; ++i) {
                temp_vertex.position = { cur_cube.vertex[i].x, cur_cube.vertex[i].y, cur_cube.vertex[i].z };
                temp_vertex.color = { cap_color_r , cap_color_g , cap_color_b };
                vertices.push_back(temp_vertex);
            }
            cur_pattern_cap++;
        }
    }


    void PEXCapacitorModel::makeIndices() {
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


    void PEXCapacitorModel::printCapNodes() {
        printf("***** PEX ITEM LIST\n");
        int i = 1;
        for (auto& pex_item : this->pex_data.getCapacitors()) {
            printf("%dth PEX Item\n", i++);
            pex_item.print();
            printf("\n");
        }

        printf("***** CAP NODE LIST\n");
        i = 1;
        for (auto& cap_node : this->cap_node_name_to_index_map) {
            printf("%dth Cap Node Item\n", i++);
            printf("\nNode Name = %s / Layer# = %d.%d / x,y = %.4f,%.4f / Value = %e / Neibor = %d\n",
                this->cap_nodes[cap_node.second].name.c_str(),
                this->cap_nodes[cap_node.second].layer_number,
                this->cap_nodes[cap_node.second].layer_datatype,
                this->cap_nodes[cap_node.second].x,
                this->cap_nodes[cap_node.second].y,
                this->cap_nodes[cap_node.second].value,
                this->cap_nodes[cap_node.second].connected_count);
            printf("\n");
        }
    }

    void PEXCapacitorModel::printPatternCaps() {
        printf("\n\n###############################################\n");
        for (auto& pattern : this->pattern_caps) {
            printf("\nLayer = %u.%u, Cap count = %u, Total Caps = %e\n", 
                pattern.layer_number, pattern.layer_datatype, pattern.cap_count, pattern.cap_value);
            printf("\tLeft ~ Right/Bottom ~ Top = %.6f ~ %.6f / %.6f ~ %.6f\n",
                pattern.pattern.minx, pattern.pattern.maxx, pattern.pattern.miny, pattern.pattern.maxy);
        }
    }    

    void PEXCapacitorModel::printLayerToCapNodeMap() {
        printf("\n\n\n####################################################\n");
        printf("Cap Layer Map\n");
        for (auto& cur_item : this->layer_to_cap_node_map) {
            printf("\nLayer = %s :: name / cap count / cap value \n", cur_item.first.c_str());
            for (auto cur_cap : cur_item.second) {
                printf("\t%s / %u / %e / %.6f, %.6f\n", 
                    cur_cap->name.c_str(), cur_cap->connected_count, cur_cap->value, cur_cap->x, cur_cap->y);
            }
        }
    }

    void PEXCapacitorModel::printLayerToPatternCapMap(const char* msg) {
        printf("\n\n\n####################################################\n");
        printf("Pattern Layer Map :: %s\n", msg);
        for (auto& cur_item : this->layer_to_pattern_cap_map) {
            printf("\nLayer = %s :: left ~ right / bottom ~ top - cap count, cap value, norm cap value\n", cur_item.first.c_str());
            for (auto cur_pattern : cur_item.second) {
                printf("\t%.6f ~ %.6f / %.6f ~ %.6f - ", 
                    cur_pattern->pattern.minx, cur_pattern->pattern.maxx, cur_pattern->pattern.miny, cur_pattern->pattern.maxy);
                printf("%u, %e, %f\n", cur_pattern->cap_count, cur_pattern->cap_value, cur_pattern->normalized_cap_value);
            }
        }
    }

    void PEXCapacitorModel::printLayerList() {
        printf("\n\n\n####################################################\n");
        printf("Layer List\n");
        for (auto& cur_layer : this->layers) {
            printf("\t%u.%u\n", cur_layer.first, cur_layer.second);
        }
    }
    

}//namespace lve

