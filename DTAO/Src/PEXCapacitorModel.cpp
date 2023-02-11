#include "PEXCapacitorModel.h"

#include <iostream>
#include <map>
#include <vector>

#include "LayoutPEXData.h"

namespace lve {
    PEXCapacitorModel::PEXCapacitorModel(
        LveDevice& device, 
        MODEL_TYPE model_type, 
        const std::string& data_file,
        LayoutDataManager* layout_data_)
        : LveModel(device, model_type), pex_data{}, cap_nodeidx_map{}, layout_data{layout_data_} {
        
        this->makePatternWCapsFromLayoutData();
        this->makeRenderingData(data_file);
    }
    PEXCapacitorModel::~PEXCapacitorModel() {
        this->pex_data.clear();
    }

    void PEXCapacitorModel::makePatternWCapsFromLayoutData() {
        if (this->layout_data->getPatterns().size()) {
            
            std::cerr << "Error! empty patten"
                      << "@PEXCapacitorModel::makePatternWCapsFromLayoutData()\n";
        }
    }

    void PEXCapacitorModel::makeRenderingData(const std::string& file_path) {
        loadData(file_path);
        makeCapNodesFromPEXData();
        
        
        //test
        printCapNodes();

    }

    void PEXCapacitorModel::loadData(const std::string& file_path) {
        this->pex_data.loadData(file_path);
        //this->pex_data.printPEXData();
    }

    void PEXCapacitorModel::makeCapNodesFromPEXData() {
        std::map<std::string, size_t>::iterator it;
        for (auto & pex_item : this->pex_data.getCapacitors()) {
            //pex_item.print();

            //node1
            std::string& node1_name = pex_item.node1.name;
            it = this->cap_nodeidx_map.find(node1_name);
            if (it == this->cap_nodeidx_map.end()) addNewCapNode(pex_item.node1, pex_item.value);
            else {
                this->cap_nodes[it->second].value += pex_item.value;
                this->cap_nodes[it->second].connected_count++;
            }

            //node2
            std::string& node2_name = pex_item.node2.name;
            it = this->cap_nodeidx_map.find(node2_name);
            if (it == this->cap_nodeidx_map.end()) addNewCapNode(pex_item.node2, pex_item.value);
            else {
                this->cap_nodes[it->second].value += pex_item.value;
                this->cap_nodes[it->second].connected_count++;
            }
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
        for (auto& cap_node : this->cap_nodeidx_map) {
            printf("%dth Cap Node Item\n", i++);
            printf("Node Name = %s / Layer# = %d.%d / x,y = %.4f,%.4f / Value = %e / Neibor = %d\n",
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

        this->cap_nodeidx_map.insert(std::pair<std::string, size_t>(new_cap_node.name, this->cap_nodes.size()-1));
    }

   
 
    void PEXCapacitorModel::makeVertices() {

    }
    void PEXCapacitorModel::makeIndices() {

    }
}

