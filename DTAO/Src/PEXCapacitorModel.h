#pragma once
#include "lve_model.hpp"
#include "LayoutPEXData.h"

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <limits>

namespace lve {
    typedef unsigned int uint;

    class PEXCapacitorModel : public LveModel
    {
    public:
        struct cap_node {
            double x;
            double y;
            uint layer_number;
            uint layer_datatype;
            std::string name;
            double value;
            uint connected_count;            
        };

        struct pattern_cap {
            cube_info pattern;
            uint layer_number;
            uint layer_datatype;
            double cap_value;
            double normalized_cap_value;
            uint cap_count;
            pattern_cap(cube_info& pattern_, uint number_, uint datatype_, double value_, double norm_value_, uint count_ ) :
                pattern(pattern_), layer_number(number_), 
                layer_datatype(datatype_), cap_value(value_), cap_count(count_), normalized_cap_value(norm_value_) {}
            
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
        std::map<std::string, size_t> cap_node_name_to_index_map;
        LayoutDataManager* layout_data = nullptr;

        std::vector<cap_node> cap_nodes;
        std::map<std::string, std::vector<cap_node*>> layer_to_cap_node_map;

        std::vector<pattern_cap> pattern_caps;
        std::map<std::string, std::vector<pattern_cap*>> layer_to_pattern_cap_map;

        std::set<std::pair<uint, uint>> layers;

        double max_cap = std::numeric_limits<double>::min();
        
    public:
        virtual void makeRenderingData(const std::string& file_path = "");
        virtual void loadData(const std::string& file_path = "");
        virtual void makeVertices();
        virtual void makeIndices();

        void makeCubeVertices();
        
    private:
        void makeCapNodesFromPEXData();
        void makePatternCapsFromLayoutData();

        void attachCapToPattern();
        void makeLayerToCapNodeMap();
        void makeLayerToPatternCapMap();

        void matchCapWithPattern(std::vector<cap_node>& caps, std::vector<pattern_cap>& patterns, 
            uint target_layer_number, uint target_layer_datatype);
        void matchCapWithPattern(
            std::map<std::string, std::vector<cap_node*>>& cap_layer_map_,
            std::map<std::string, std::vector<pattern_cap*>>& pattern_layer_map_,
            uint target_layer_number, uint target_layer_datatype);
        bool isPatternIncludeCap(const pattern_cap& pattern, const cap_node& cap);

        void setLayoutDataManager(LayoutDataManager* layout_data_) { this->layout_data = layout_data_; }
        void addNewCapNode(pex_node& new_node, double value);

        std::string getLayerString(uint layer_number, uint layer_datatype);

        void normalizePatternCap();
        

        

        void printCapNodes();
        void printPatternCaps();
        void printLayerToCapNodeMap();
        void printLayerToPatternCapMap(const char* msg = "");
        void printLayerList();
    };
}

