#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <limits>

using Vec3 = glm::dvec3;
using Color = Vec3;

using std::shared_ptr;
using std::make_shared;

const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}