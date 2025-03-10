#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>

// C++ Std Usings

using std::make_shared;
using std::shared_ptr;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180.0;
}

inline double random_double() {
  // Return a random real number in [0, 1)
  return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
  // Return a random real number in [min, max)
  return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
  return int(random_double(min, max + 1));
}

// Common Headers
#include "color.h"
#include "interval.h"
#include "ray.h"
#include "vec3.h"
#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include "primitive.h"
