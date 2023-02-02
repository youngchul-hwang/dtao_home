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

namespace pex {
	/// <summary>
	/// PEXItem
	/// ///////////////////////////////////////////////////////////////
	/// </summary>
	PEXItem::PEXItem() : node1{}, node2{} {
		this->value = 0.0;
	}

	PEXItem::PEXItem(const node& node1_, const node& node2_, const double& value_) :
		node1(node1_), node2(node2_), value(value_) {
	}

	PEXItem::~PEXItem() {
	}

void PEXItem::print() {
		printf("Node1 x1/y1/x2/y2/Z-s/Z-e = %.5f,%.5f,%.5f,%.5f\n", 
			this->node1.x, this->node1.y, this->node1.z_start, this->node1.z_end);
		printf("Node2 x1/y1/x2/y2/Z-s/Z-e = %.5f,%.5f,%.5f,%.5f\n",
			this->node2.x, this->node2.y, this->node2.z_start, this->node2.z_end);
	}

	/// <summary>
	/// PEXResistor
	/// ///////////////////////////////////////////////////////////////
	/// </summary>
	PEXResistor::PEXResistor() : PEXItem() {
	}
	PEXResistor::PEXResistor(const node& node1_, const node& node2_, const double& value_) :
		PEXItem(node1_, node2_, value_) {
	}

	void PEXResistor::print() {
		PEXItem::print();
		printf("Resistor Value = %10.5f\n", this->getValue());
	}

	PEXResistor::~PEXResistor() {
	}

	/// <summary>
	/// PEXCapacitor
	/// ///////////////////////////////////////////////////////////////
	/// </summary>
	PEXCapacitor::PEXCapacitor() : PEXItem() {
	}
	PEXCapacitor::PEXCapacitor(const node& node1_, const node& node2_, const double& value_) :
		PEXItem(node1_, node2_, value_) {
	}
	PEXCapacitor::~PEXCapacitor() {
	}

	void PEXCapacitor::print() {
		PEXItem::print();
		printf("Capacitor Value = %10.5e\n", this->getValue());
	}

	/// <summary>
	/// PEXDataManager
	/// ///////////////////////////////////////////////////////////////
	/// </summary>
	PEXDataManager::PEXDataManager() {
		this->capasitors = {};
		this->resistors = {};
		this->file_path = "";
		this->max_resistor = std::numeric_limits<double>::min();
		this->min_resistor = std::numeric_limits<double>::max();
		this->max_capacitor = std::numeric_limits<double>::min();
		this->min_capacitor = std::numeric_limits<double>::max();
	}

	PEXDataManager::~PEXDataManager() {
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
		
		string node_name{};
		node node1{}, node2{};
		double value = 0.0;

		size_t line_count = infile.GetRowCount();
		for (int i = 1; i < line_count; ++i) {
			vector<string> line = infile.GetRow<string>(i); 

			node_name = line[pex::PEXINFO_INDEX::PEXINFO_INDEX_NAME];
			node1.x = stof(line[pex::PEXINFO_INDEX::PEXINFO_INDEX_NODE1_X]);
			node1.y = stof(line[pex::PEXINFO_INDEX::PEXINFO_INDEX_NODE1_Y]);
			node1.z_start = stof(line[pex::PEXINFO_INDEX::PEXINFO_INDEX_NODE1_ZSTART]);
			node1.z_end = stof(line[pex::PEXINFO_INDEX::PEXINFO_INDEX_NODE1_ZEND]);
			node2.x	= stof(line[pex::PEXINFO_INDEX::PEXINFO_INDEX_NODE2_X]);
			node2.y	= stof(line[pex::PEXINFO_INDEX::PEXINFO_INDEX_NODE2_Y]);
			node2.z_start = stof(line[pex::PEXINFO_INDEX::PEXINFO_INDEX_NODE2_ZSTART]);
			node2.z_end = stof(line[pex::PEXINFO_INDEX::PEXINFO_INDEX_NODE2_ZEND]);
			value = stof(line[pex::PEXINFO_INDEX::PEXINFO_INDEX_VALUE]);

			if (node_name[0] == 'R') {
				this->resistors.push_back(PEXResistor(node1, node2, value));
			}
			else if (node_name[0] == 'C') {
				this->capasitors.push_back(PEXCapacitor(node1, node2, value));
			}
		}
	}
	
	void PEXDataManager::printPEXData() {
		
		std::cout << "\n\nPEX Resistors\n";
		for (auto item : this->resistors) {
			item.print();
		}

		std::cout << "\n\nPEX Capacitors\n";
		for (auto item : this->capasitors) {
			item.print();
		}
	}


	void testLayoutPEXData(){
		PEXDataManager pex_data;
		pex_data.loadPEXData("Data/MVP_PEX_RC_r1_0130.csv");
		pex_data.printPEXData();
	}
}
