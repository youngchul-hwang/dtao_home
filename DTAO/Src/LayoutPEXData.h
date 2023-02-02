#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace pex {

	void testLayoutPEXData();

	typedef enum PEXINFO_INDEX {
		PEXINFO_INDEX_NAME = 0,
		PEXINFO_INDEX_NODE1_X = 2,
		PEXINFO_INDEX_NODE1_Y = 3,
		PEXINFO_INDEX_NODE1_ZSTART = 4,
		PEXINFO_INDEX_NODE1_ZEND = 5,
		PEXINFO_INDEX_NODE2_X = 8,
		PEXINFO_INDEX_NODE2_Y = 9,
		PEXINFO_INDEX_NODE2_ZSTART = 10,
		PEXINFO_INDEX_NODE2_ZEND = 11,
		PEXINFO_INDEX_VALUE = 13,
		PEXINFO_INDEX_DEFULT = 999
	} PEXINFO_INDEX;

	struct point {
		double x;
		double y;
		point() : x{}, y{} {}
		point(const double x_, const double y_) : x(x_), y(y_) {}
	};

	struct rect {
		point left;
		point right;
		point top;
		point bottom;
		rect() : left{}, right{}, top{}, bottom{} {}
		rect(const point& l_, const point& r_, const point& t_, const point& b_) :	
			left(l_), right(r_), top(t_), bottom(b_) {}
	};

	struct node {
		double x;
		double y;
		double z_start;
		double z_end;
		node() : x{}, y{}, z_start {}, z_end{} {};
		node(const double& x_, const double & y_, double & z_s, double & z_e ) 
			: x(x), y(y_), z_start(z_s), z_end(z_e) {}
	};

	class PEXItem
	{
	public:
		PEXItem();
		~PEXItem();
		PEXItem(const node& node1_, const node& node2_, const double& value_);

	private:
		node node1;
		node node2;
		double value;

	public:
		void setNode1(node& node_) { this->node1 = node_; }
		void setNode2(node& node_) { this->node2 = node_; }
		void setValue(double value_) { this->value = value_; }

		node& getNode1() { return this->node1; }
		node& getNode2() { return this->node2; }
		double getValue() { return this->value; }

		virtual void print();
	};

	class PEXResistor : PEXItem
	{
	public:
		PEXResistor();
		~PEXResistor();
		PEXResistor(const node& node1_, const node& node2_, const double& value_);

	public:
		void print();
	};

	class PEXCapacitor : PEXItem
	{
	public:
		PEXCapacitor();
		~PEXCapacitor();
		PEXCapacitor(const node& node1_, const node& node2_, const double& value_);

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

		double max_resistor;
		double min_resistor;
		double max_capacitor;
		double min_capacitor;

	public:
		std::vector<PEXResistor>& getResistors() { return this->resistors; }
		std::vector<PEXCapacitor>& getCapacitors() { return this->capasitors; }

		void loadPEXData(const std::string file_path);
		void printPEXData();

	private:



	};
}//namespace pex

