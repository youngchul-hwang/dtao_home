#pragma once

namespace pex {

	struct point {
		double x;
		double y;
		point(const double x_, const double y_) : x(x_), y(y_) {}
	};

	struct rect {
		point l;
		point r;
		point t;
		point b;
		rect(const point& l_, const point& r_, const point& t_, const point& b_) :	l(l_), r(r_), t(t_), b(b_) {}
	};

	struct node {
		point p1;
		point p2;
		node(const point& p1_, const point& p2_) : p1(p1_), p2(p2_) {}
	};

	class PEXItem
	{
	public:
		PEXItem();
		~PEXItem();

	private:
		node node1;
		node node2;

		double value;

	};

	class PEXResistor : PEXItem
	{

	};

	class PEXCapacitor : PEXItem
	{

	};

	class LayoutPEXData
	{
	};

	class PEXDataManager
	{

	};
}//namespace pex

