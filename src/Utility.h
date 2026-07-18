#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <limits>
#include <random>

using Vec3 = glm::dvec3;
using Color = Vec3;

using std::shared_ptr;
using std::make_shared;

const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
    return degrees * PI / 180.0;
}

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}

inline double random_double(double min, double max) {
    return min + (max-min)*random_double();
}

inline Vec3 random_vec() {
    return Vec3(random_double(), random_double(), random_double());
}

inline Vec3 random_vec(double min, double max) {
    return Vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline Vec3 random_unit_vec() {
    Vec3 vec = random_vec(-1, 1);
    return glm::normalize(vec);
}

inline Vec3 random_on_hemisphere(const Vec3& normal) {
    Vec3 vec = random_unit_vec();
    if(glm::dot(normal, vec) <= 0) return -vec;
    return vec;
}

inline double linear_to_gamma(double linear_comp) {
    if(linear_comp > 0) return std::sqrt(linear_comp);
    return 0;
}

inline bool near_zero(Vec3 vec) {
    // Return true if the vector is close to zero in all dimensions.
    auto s = 1e-8;
    return (std::fabs(vec.x) < s) && (std::fabs(vec.y) < s) && (std::fabs(vec.z) < s);
}

inline Vec3 reflect(const Vec3& vec, const Vec3& n) {
    return vec - 2 * glm::dot(vec, n) * n;
}

inline Vec3 refract(const Vec3& uv, const Vec3& n, double etai_over_etat) {
    double cos_theta = std::fmin(dot(-uv, n), 1.0);
    Vec3 r_out_perp =  etai_over_etat * (uv + cos_theta*n);
    Vec3 r_out_parallel = -std::sqrt(std::fabs(1.0 - glm::dot(r_out_perp, r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
}