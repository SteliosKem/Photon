#pragma once

#include "Utility.h"
#include "Ray.h"

class Sphere {
public:
    Sphere() = default;
    Sphere(const Vec3& center, double radius) : m_center(center), m_radius(radius) {}

    const Vec3& center() const { return m_center; }
    const double radius() const { return m_radius; }

    double hit(const Ray& ray) const;
private:
    Vec3 m_center;
    double m_radius;
};