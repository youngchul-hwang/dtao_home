#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace lve {

	void testPEXData();
	void testLayoutData();

	enum LAYOUTINFO_INDEX {
		//LAYOUTINFO_IDX_STRUCTURE = 0,
		//LAYOUTINFO_IDX_CELLNAME = 1,
		LAYOUTINFO_IDX_LAYER = 0,
		LAYOUTINFO_IDX_DATATYPE = 1,
		LAYOUTINFO_IDX_LEFT = 2,
		LAYOUTINFO_IDX_BOTTOM = 3,
		LAYOUTINFO_IDX_RIGHT = 4,
		LAYOUTINFO_IDX_TOP = 5,
		LAYOUTINFO_IDX_ZSTART = 6,
		LAYOUTINFO_IDX_ZEND = 7,
		LAYOUTINFO_IDX_DEFAULT = -1,

	};

	enum LAYOUT_LAYER_TYPE {
		LAYOUT_LAYER_TYPE_1 = 0,
		LAYOUT_LAYER_TYPE_DEFAULT = 99
	};

	enum PEXINFO_INDEX {
		PEXINFO_INDEX_NAME = 0,
		PEXINFO_INDEX_NODE1_X = 2,
		PEXINFO_INDEX_NODE1_Y = 3,
		PEXINFO_INDEX_NODE1_ZSTART = 4,
		PEXINFO_INDEX_NODE1_ZEND = 5,
		PEXINFO_INDEX_NODE1_LVL = 6,
		PEXINFO_INDEX_NODE2_X = 8,
		PEXINFO_INDEX_NODE2_Y = 9,
		PEXINFO_INDEX_NODE2_ZSTART = 10,
		PEXINFO_INDEX_NODE2_ZEND = 11,
		PEXINFO_INDEX_NODE2_LVL = 12,
		PEXINFO_INDEX_VALUE = 13,
		PEXINFO_INDEX_DESCRIPTION = 14,
		PEXINFO_INDEX_DEFULT = 999
	};
	
	enum PEXResDirection {
		RES_DIRECTION_VERTICAL = 0,
		RES_DIRECTION_HORIZONTAL = 1,
		RES_DIRECTION_NONE = 99
	};

	struct cube_info {
		double minx;
		double miny;
		double maxx;
		double maxy;
		double minz;
		double maxz;
	};

	struct coord3d {
		double x;
		double y;
		double z;
	};

	struct cube_vertex {
		coord3d vertex[8];
	};

	struct node {
		double x;
		double y;
		double z_start;
		double z_end;
		node() : x{}, y{}, z_start {}, z_end{} {};
		node(const double& x_, const double & y_, double & z_s, double & z_e ) 
			: x(x_), y(y_), z_start(z_s), z_end(z_e) {}
	};

	class LayoutItem {
	public:
		LayoutItem();
		~LayoutItem();
		LayoutItem(const cube_info& pattern_, const int layer_number_, const int layer_datatype_, LAYOUT_LAYER_TYPE layer_type_);

	public:
		cube_info pattern;
		int layer_number;
		int layer_datatype;
		LAYOUT_LAYER_TYPE layer_type;
	};


	class LayoutDataManager
	{
	public:
		LayoutDataManager();
		~LayoutDataManager();

	private:
		std::string file_path;
		std::vector<LayoutItem> patterns;
		
		double layout_min_x;
		double layout_max_x;
		double layout_min_y;
		double layout_max_y;
		double layout_min_z;
		double layout_max_z;

		double scale;

	public:
		std::vector<LayoutItem>& getPatterns() { return this->patterns; }
		
		void loadLayoutData(const std::string file_path);
		void clear() { this->patterns.clear(); };
		void printLayoutData() {};

		double getMinX() { return this->layout_min_x; }
		double getMaxX() { return this->layout_max_x; }
		double getMinY() { return this->layout_min_y; }
		double getMaxY() { return this->layout_max_y; }
		double getMinZ() { return this->layout_min_z; }
		double getMaxZ() { return this->layout_max_z; }

	private:
		void calculateScale();

	};

	class PEXItem
	{
	public:
		PEXItem();
		~PEXItem();
		PEXItem(const node& node1_, const node& node2_, const double& value_, const std::string& name_);

	public:
		node node1;
		node node2;
		double value;
		std::string name;

	public:
		void setNode1(node& node_) { this->node1 = node_; }
		void setNode2(node& node_) { this->node2 = node_; }
		void setValue(double value_) { this->value = value_; }

		node& getNode1() { return this->node1; }
		node& getNode2() { return this->node2; }
		double getValue() { return this->value; }

		virtual void print();
	};



	class PEXResistor : public PEXItem
	{
	public:
		PEXResistor();
		~PEXResistor();
		PEXResistor(const std::string& name_, const node& node1_, const node& node2_, const double& value_, const PEXResDirection& direction_);

	private:
		PEXResDirection direction;

	public:
		void setDirection(PEXResDirection direction_) { this->direction = direction_; }
		PEXResDirection getDirection() const { return this->direction; }

		static PEXResDirection checkDirectionFromDescription(const std::string& description);

		void print();
	};

	class PEXCapacitor : public PEXItem
	{
	public:
		PEXCapacitor();
		~PEXCapacitor();
		PEXCapacitor(const std::string& name_, const node& node1_, const node& node2_, const double& value_);

	public:
		void print();
	};


	class PEXDataManager
	{
	public:
		PEXDataManager();
		~PEXDataManager();

	private:
		std::string file_path;

		std::vector<PEXResistor> resistors;
		std::vector<PEXCapacitor> capasitors;

		double max_resistor_vertical;
		double min_resistor_vertical;
		double max_resistor_horizontal;
		double min_resistor_horizontal;
		double max_capacitor;
		double min_capacitor;

	public:
		std::vector<PEXResistor>& getResistors() { return this->resistors; }
		std::vector<PEXCapacitor>& getCapacitors() { return this->capasitors; }

		void loadPEXData(const std::string file_path);
		void clear();
		void printPEXData();


		double getMinResistorVerticalValue() { return this->min_resistor_vertical; }
		double getMaxResistorVerticalValue() { return this->max_resistor_vertical; }
		double getMinResistorHorizontalValue() { return this->min_resistor_horizontal; }
		double getMaxResistorHorizontalValue() { return this->max_resistor_horizontal; }
		double getMinCapacitorValue() { return this->min_capacitor; }
		double getMaxCapacitorValue() { return this->max_capacitor; }

	private:
		void updateMinMaxRes(const double& value, const PEXResDirection direction);
		void updateMinMaxCap(const double& value);
	};
}//namespace lve

