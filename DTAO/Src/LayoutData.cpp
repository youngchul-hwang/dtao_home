#include "LayoutPEXData.h"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <limits>
#include <cstdio>
#include <chrono>

#include "rapidcsv.h"
#include "utils.h"

using namespace std;

inline double coord_normalize(double in_value, double move, double scale) {
    //return 2 * (in_value - min_value) / (max_value - min_value) - 1.0f;
    //return (2 * (in_value - move) / (scale)-1.0f);
    return 2 * ((in_value - move)*(scale)) -1.0f;
}

namespace lve {

	LayoutItem::LayoutItem() : pattern{}, layer_number{}, layer_datatype{}, layer_type{} {
	}
	LayoutItem::~LayoutItem() {
	}

	LayoutItem::LayoutItem(const cube_info& pattern_, const int layer_number_, const int layer_datatype_, LAYOUT_LAYER_TYPE layer_type_) 
		: pattern(pattern_), layer_number(layer_number_), layer_datatype(layer_datatype_), layer_type(layer_type_){
	}

	
	LayoutDataManager::LayoutDataManager() 
		: file_path{}, patterns{}, scale(1.0),
		layout_min_x{}, layout_max_x{}, layout_min_y{}, layout_max_y{}, layout_min_z{}, layout_max_z{} {
	}

	LayoutDataManager::~LayoutDataManager() {
		this->patterns.clear();
	}

    void LayoutDataManager::calculateScale() {
        double x_diff = this->layout_max_x - this->layout_min_x;
        double y_diff = this->layout_max_y - this->layout_min_y;
        double z_diff = this->layout_max_z - this->layout_min_z;

        double max_diff = x_diff > y_diff ? x_diff : y_diff;
        max_diff = max_diff > z_diff ? max_diff : z_diff;
        
        this->scale = 1.0f / max_diff;
    }

	void LayoutDataManager::loadLayoutData(const std::string file_path) {
        if (!filesystem::exists(file_path)) {
            cerr << "File is not exist : " << file_path << endl;
            return;
        }


        std::chrono::system_clock::time_point start, end;
        std::chrono::seconds run_time;

        start = std::chrono::system_clock::now();

        rapidcsv::Document infile(file_path, rapidcsv::LabelParams(-1, -1));
        
        end = std::chrono::system_clock::now();
        run_time = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        std::cout << "Run Time / LayoutDataManager / loadLayoutData / rapidcsv::infile [sec] : " << run_time << "\n";

        std::vector<float> bbox = infile.GetRow<float>(0);
        this->layout_min_x = bbox[0];
        this->layout_min_y = bbox[1];
        this->layout_max_x = bbox[2];
        this->layout_max_y = bbox[3];
        this->layout_min_z = bbox[4];
        this->layout_max_z = bbox[5];

        calculateScale();        

        size_t line_count = infile.GetRowCount();
        cube_info cur_cube;
        int layer_number, layer_datatype;
        
        start = std::chrono::system_clock::now();

        for (int i = 1; i < line_count; ++i) { //first line is bbox area
            vector<string> line = infile.GetRow<string>(i);
            //if (line[LAYOUTINFO_IDX_STRUCTURE] != "Poly") continue;
            //if (std::stoi(line[LAYOUTINFO_IDX_NUMPOINTS]) != 4) continue;

            layer_number = std::stoi(line[LAYOUTINFO_IDX_LAYER]);
            layer_datatype = std::stoi(line[LAYOUTINFO_IDX_DATATYPE]);

            cur_cube.minz = std::stof(line[LAYOUTINFO_IDX_ZSTART]);
            cur_cube.maxz = std::stof(line[LAYOUTINFO_IDX_ZEND]);
            if (cur_cube.minz == cur_cube.maxz) continue;

            cur_cube.minx = std::stof(line[LAYOUTINFO_IDX_LEFT]);
            cur_cube.maxx = std::stof(line[LAYOUTINFO_IDX_RIGHT]);
            cur_cube.miny = std::stof(line[LAYOUTINFO_IDX_BOTTOM]);
            cur_cube.maxy = std::stof(line[LAYOUTINFO_IDX_TOP]);

            //cur_cube.minx = coord_normalize(cur_cube.minx, this->layout_min_x, scale);
            //cur_cube.maxx = coord_normalize(cur_cube.maxx, this->layout_min_x, scale);
            //cur_cube.miny = coord_normalize(cur_cube.miny, this->layout_min_y, scale);
            //cur_cube.maxy = coord_normalize(cur_cube.maxy, this->layout_min_y, scale);
            //cur_cube.minz = coord_normalize(cur_cube.minz, this->layout_min_z, scale);
            //cur_cube.maxz = coord_normalize(cur_cube.maxz, this->layout_min_z, scale);

            this->patterns.push_back(LayoutItem(cur_cube, layer_number, layer_datatype, LAYOUT_LAYER_TYPE::LAYOUT_LAYER_TYPE_DEFAULT));
            
        }

        end = std::chrono::system_clock::now();
        run_time = std::chrono::duration_cast<std::chrono::seconds>(end - start);
        std::cout << "Run Time / LayoutDataManager / loadLayoutData / pattern push [sec] : " << run_time << "\n";

        infile.Clear();

        /*
        printf("\n\nLayout Info Data\n\n");
        for (auto& cur_pattern : this->patterns) {
            printf("left/bottom/right/top/z-start/z-end = %.4f/%.4f/%.4f/%.4f/%.4f/%.4f/\n",
                cur_pattern.pattern.minx, cur_pattern.pattern.miny, 
                cur_pattern.pattern.maxx, cur_pattern.pattern.maxy, 
                cur_pattern.pattern.minz, cur_pattern.pattern.maxz);
        }
        //*/
	}

    void testLayoutData() {
        LayoutDataManager layout_data;
        layout_data.loadLayoutData("Data/layout_input_data.csv");
        
    }
	
}//namespace lve
