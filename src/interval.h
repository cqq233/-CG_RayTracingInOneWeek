#ifndef INTERVAL_H
#define INTERVAL_H

#include "rtweekend.h"

class interval
{
public:
	double min, max;

	interval() : min(INF), max(-INF) {} // default : the interval is empty
	interval(double _min, double _max) : min(_min), max(_max) {}

	bool contains(double x)
	{
		return min <= x && x <= max;
	}

	bool surrounds(double x)
	{
		return min < x && x < max;
	}

	double clamp(double x) const
	{
		if (x < min) return min;
		if (x > max) return max;
		return x;
	}

	static const interval empty, universe;
};

const static interval empty(+INF, -INF);
const static interval universe(-INF, +INF);

#endif
