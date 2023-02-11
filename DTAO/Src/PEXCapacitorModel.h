#pragma once
#include "lve_model.hpp"
#include "LayoutPEXData.h"

#include <iostream>
#include <vector>
#include <map>

namespace lve {
    class PEXCapacitorModel : public LveModel
    {
    public:
        struct cap_node {
            double x;
            double y;
            unsigned int layer_number;
            unsigned int layer_datatype;
            std::string name;
            double value;
            unsigned int connected_count;            
        };

        struct pattern_w_cap {
            cube_info pattern;
            unsigned int layer_number;
            unsigned int layer_datatype;
            double cap_value;
            unsigned int cap_count;
        };

    public:
        PEXCapacitorModel(
            LveDevice& device, MODEL_TYPE model_type, 
            const std::string& data_file = "", LayoutDataManager* layout_data_ = nullptr);
        ~PEXCapacitorModel();

        PEXCapacitorModel() = delete;
        PEXCapacitorModel(const PEXCapacitorModel&) = delete;
        PEXCapacitorModel& operator=(const PEXCapacitorModel&) = delete;

    private:
        PEXCapacitorDataManager pex_data;
        std::map<std::string, size_t> cap_nodeidx_map;
        LayoutDataManager* layout_data = nullptr;

        std::vector<cap_node> cap_nodes;
        std::vector<pattern_w_cap> pattern_w_caps;
        
    public:
        virtual void makeRenderingData(const std::string& file_path = "");
        virtual void loadData(const std::string& file_path = "");
        virtual void makeVertices();
        virtual void makeIndices();

    private:
        void makeCapNodesFromPEXData();
        void makePatternWCapsFromLayoutData();

        void setLayoutDataManager(LayoutDataManager* layout_data_) { this->layout_data = layout_data_; }
        void addNewCapNode(pex_node& new_node, double value);
        //void makeCubeVertices();
        //void makeCubes();
        //void makeCube(const PEXResistor& in_res, cube_info& out_cube, PEXResDirection direction);
        //void setResCubeThickness(double value) { this->res_cube_thickness = value; }

        void printCapNodes();
    };
}

