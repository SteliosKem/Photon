#include "Object.h"

bool Sphere::hit(const Ray& ray) const {
    Vec3 oc = m_center - ray.origin();
    double a = glm::dot(ray.direction(), ray.direction());
    double b = -2.0 * glm::dot(oc, ray.direction());
    double c = glm::dot(oc, oc) - m_radius * m_radius;
    double discriminant = b * b - 4 * a * c;
    return (discriminant >= 0);
}