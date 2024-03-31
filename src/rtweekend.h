#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <limits>
#include <memory>
#include <cstdlib>


using std::shared_ptr;
using std::make_shared;
using std::sqrt;

//Constants;
const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;
const double HALF_PI = PI * 0.5;
const double DOUBLE_PI = PI * 2.0;

//helper functions
inline double degress_to_radians(double degrees)
{
	return degrees * PI / 180.0;
}

inline double radians_to_degress(double radians)
{
	return 180.0 * radians / PI;
}

inline double random_double()
{
	// return a random real in [0, 1);
	return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max)
{
	return min + (max - min) * random_double();
}


//Common Headers;
#include "ray.h"
#include "vec3.h"
#include "interval.h"
#include "color.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "material.h"
#include "camera.h"

#endif // !RTWEEKEND_H

