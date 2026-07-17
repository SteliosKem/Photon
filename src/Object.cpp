#include "Object.h"
#include <optional>

void _HitInfo::set_face_normal(const Ray& ray, const Vec3& outward_normal) {
    front_face = glm::dot(ray.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
}

HitInfo ObjectList::hit(const Ray& ray, double t_min, double t_max) const {
    _HitInfo info{};
    bool hit_anything = false;
    double closest = t_max;
    for(const shared_ptr<Object>& obj : m_objects) {
        HitInfo h = obj->hit(ray, t_min, t_max);
        if(h && h->ray_t < closest) {
            hit_anything = true;
            closest = h->ray_t;
            info = h.value();
        }
    }
    if(hit_anything) return info;
    return std::nullopt;
}

void ObjectList::add(shared_ptr<Object> obj) {
    m_objects.push_back(obj);
}

HitInfo Sphere::hit(const Ray& ray, double t_min, double t_max) const {
    Vec3 oc = m_center - ray.origin();
    double a = glm::dot(ray.direction(), ray.direction());
    double b = -2.0 * glm::dot(oc, ray.direction());
    double c = glm::dot(oc, oc) - m_radius * m_radius;
    double discriminant = b * b - 4 * a * c;
    if(discriminant < 0) return {};
    // Set smallest t value for given ray with respect to t_min & t_max
    double t = (-b - sqrt(discriminant)) / (2.0*a);
    if(t <= t_min || t >= t_max) {
        t = -b + sqrt(discriminant) / (2.0*a);
        if(t <= t_min || t >= t_max) return std::nullopt;
    }
    _HitInfo info{
        Vec3(ray.at(t)),
        glm::normalize(ray.at(t) - m_center),
        t
    };
    Vec3 outward_normal = (info.point - m_center) / m_radius;
    info.set_face_normal(ray, outward_normal);
    return info;
}
