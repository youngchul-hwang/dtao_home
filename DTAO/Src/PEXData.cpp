#include "LayoutPEXData.h"
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <limits>
#include <cstdio>

#include "rapidcsv.h"
#include "utils.h"

using namespace std;

namespace lve {
	/// <summary>
	/// PEXItem
	/// ///////////////////////////////////////////////////////////////
	/// </summary>
	PEXItem::PEXItem() : node1{}, node2{}, name{} {
		this->value = 0.0;
	}

	PEXItem::PEXItem(const node& node1_, const node& node2_, const double& value_, const string& name_) :
		node1(node1_), node2(node2_), value(value_), name(name_) {
	}

	PEXItem::~PEXItem() {
	}
	
	void PEXItem::print() {
		printf("%s\n", this->name.c_str());
		printf("Node1 x1/y1/x2/y2/Z-s/Z-e = %.5f,%.5f,%.5f,%.5f\n", 
			this->node1.x, this->node1.y, this->node1.z_start, this->node1.z_end);
		printf("Node2 x1/y1/x2/y2/Z-s/Z-e = %.5f,%.5f,%.5f,%.5f\n",
			this->node2.x, this->node2.y, this->node2.z_start, this->node2.z_end);
}

	/// <summary>
	/// PEXResistor
	/// ///////////////////////////////////////////////////////////////
	/// </summary>
	PEXResistor::PEXResistor() : PEXItem(), direction(RES_DIRECTION_NONE) {
	}
	PEXResistor::PEXResistor(const string& name_, const node& node1_, const node& node2_, const double& value_, const PEXResDirection& direction_) :
		PEXItem(node1_, node2_, value_, name_), direction(direction_) {
	}

	void PEXResistor::print() {
		PEXItem::print();
		printf("Direction = ");
		if (this->direction == RES_DIRECTION_VERTICAL) printf("Vertical\n");
		else printf("Horizontal\n");
		printf("Resistor Value = %10.5f\n\n", this->getValue());
	}

	PEXResistor::~PEXResistor() {
	}

	PEXResDirection PEXResistor::checkDirectionFromDescription(const std::string& description) {
		PEXResDirection direction;
		if (description.find("$a") == std::string::npos) {
			direction = RES_DIRECTION_HORIZONTAL;
		}
		else {
			direction = RES_DIRECTION_VERTICAL;			
		}
		//printf("#####Description = %s, ", description.c_str());
		//if (direction == RES_DIRECTION_VERTICAL) printf("Vertical\n");
		//else printf("Horizontal\n");

		return direction;
	}

	/// <summary>
	/// PEXCapacitor
	/// ///////////////////////////////////////////////////////////////
	/// </summary>
	PEXCapacitor::PEXCapacitor() : PEXItem() {
	}
	PEXCapacitor::PEXCapacitor(const std::string& name_, const node& node1_, const node& node2_, const double& value_) :
		PEXItem(node1_, node2_, value_, name_) {
	}
	PEXCapacitor::~PEXCapacitor() {
	}

	void PEXCapacitor::print() {
		PEXItem::print();
		printf("Capacitor Value = %10.5e\n\n", this->getValue());
	}

	/// <summary>
	/// PEXDataManager
	/// ///////////////////////////////////////////////////////////////
	/// </summary>
	PEXDataManager::PEXDataManager() {
		printf("\n\n\tPEXDataManager\n");

		this->capasitors = {};
		this->resistors = {};
		this->file_path = "";
		this->max_resistor = std::numeric_limits<double>::min();
		this->min_resistor = std::numeric_limits<double>::max();
		this->max_capacitor = std::numeric_limits<double>::min();
		this->min_capacitor = std::numeric_limits<double>::max();
	}

	PEXDataManager::~PEXDataManager() {
		printf("\n\n\t~PEXDataManager\n");
		this->capasitors.clear();
		this->resistors.clear();
	}

	void PEXDataManager::clear() {
		this->capasitors.clear();
		this->resistors.clear();
	}

	void PEXDataManager::loadPEXData(const std::string file_path) {
		if ( !std::filesystem::exists(file_path) ){
			cerr << "Error! " << file_path << " is not exist!!\n";
			return;
		}

		this->file_path = file_path;
		rapidcsv::Document infile(file_path, rapidcsv::LabelParams(-1, -1));
		
		string item_name{}, node1_lvl{}, node2_lvl{};
		node node1{}, node2{};
		string description{};
		double value = 0.0;

		size_t line_count = infile.GetRowCount();
		for (int i = 1; i < line_count; ++i) {
			vector<string> line = infile.GetRow<string>(i); 

			item_name = line[PEXINFO_INDEX::PEXINFO_INDEX_NAME];
			node1.x = stof(line[PEXINFO_INDEX::PEXINFO_INDEX_NODE1_X]);
			node1.y = stof(line[PEXINFO_INDEX::PEXINFO_INDEX_NODE1_Y]);
			node1.z_start = stof(line[PEXINFO_INDEX::PEXINFO_INDEX_NODE1_ZSTART]);
			node1.z_end = stof(line[PEXINFO_INDEX::PEXINFO_INDEX_NODE1_ZEND]);
			node2.x	= stof(line[PEXINFO_INDEX::PEXINFO_INDEX_NODE2_X]);
			node2.y	= stof(line[PEXINFO_INDEX::PEXINFO_INDEX_NODE2_Y]);
			node2.z_start = stof(line[PEXINFO_INDEX::PEXINFO_INDEX_NODE2_ZSTART]);
			node2.z_end = stof(line[PEXINFO_INDEX::PEXINFO_INDEX_NODE2_ZEND]);
			value = stof(line[PEXINFO_INDEX::PEXINFO_INDEX_VALUE]);
			
			node1_lvl = line[PEXINFO_INDEX_NODE1_LVL];
			node2_lvl = line[PEXINFO_INDEX_NODE2_LVL];

			//임시 코드, 
			//현재 pex 데이터에서 RX의 높이가 0으로 설정되어 있는 오류가 있다.
			//오류 수정전까지 임시로 데이터를 수정
			if (node1_lvl == "RX") { node1.z_start = -0.1; node1.z_end = -0.09;	}
			if (node2_lvl == "RX") { node2.z_start = -0.1; node2.z_end = -0.09;	}
			//임시코드


			if (item_name[0] == 'R') {
				if (node1_lvl == "RX") {
					node1.x = node2.x;
					node1.y = node2.y;
				}

				updateMinMaxRes(value);
				description = line[PEXINFO_INDEX::PEXINFO_INDEX_DESCRIPTION];
				this->resistors.push_back(PEXResistor(item_name, node1, node2, value, PEXResistor::checkDirectionFromDescription(description)));				
			}
			else if (item_name[0] == 'C') {
				updateMinMaxCap(value);
				this->capasitors.push_back(PEXCapacitor(item_name, node1, node2, value));
			}
		}
	}

	inline void PEXDataManager::updateMinMaxRes(const double& new_value) {
		if (this->min_resistor > new_value) this->min_resistor = new_value;
		if (this->max_resistor < new_value) this->max_resistor = new_value;
	}

	inline void PEXDataManager::updateMinMaxCap(const double& new_value) {
		if (this->min_capacitor > new_value) this->min_capacitor = new_value;
		if (this->max_capacitor < new_value) this->max_capacitor = new_value;
	}
	
	void PEXDataManager::printPEXData() {
		
		std::cout << "\n\nPEX Resistors\n";
		for (auto item : this->resistors) {
			item.print();
		}
		printf("##Resistor Count : %lld, Max res = %10.5f, Min res = %10.5f\n", 
			this->resistors.size(), this->getMaxResistorValue(), this->getMinResistorValue());

		std::cout << "\n\nPEX Capacitors\n";
		for (auto item : this->capasitors) {
			item.print();
		}
		printf("##Capacitor Count : %lld, Max cap = %10.5e, Min cap = %10.5e\n",
			this->capasitors.size(), this->getMaxCapacitorValue(), this->getMinCapacitorValue());
	}


	void testPEXData(){
		PEXDataManager pex_data;
		pex_data.loadPEXData("Data/MVP_PEX_RC_r1_0130.csv");
		pex_data.printPEXData();
	}
}
