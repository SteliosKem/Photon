#include "Object.h"
#include <optional>

void _HitInfo::set_face_normal(const Ray& ray, const Vec3& outward_normal) {
    front_face = glm::dot(ray.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
}

HitInfo ObjectList::hit(const Ray& ray, const Interval& interval) const {
    _HitInfo info{};
    bool hit_anything = false;
    double closest = interval.max();
    for(const shared_ptr<Object>& obj : m_objects) {
        HitInfo h = obj->hit(ray, interval);
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
    m_bounding_box = AABB(m_bounding_box, obj->bounding_box());
}

Sphere::Sphere(const Vec3& center, double radius, shared_ptr<Material> mat) : m_center(center), m_radius(std::fmax(0, radius)), m_mat(mat) {
    double rvec = m_radius;
    m_bounding_box = AABB(center - Vec3(rvec, rvec, rvec),
                            center + Vec3(rvec, rvec, rvec));
}

HitInfo Sphere::hit(const Ray& ray, const Interval& interval) const {
    Vec3 oc = m_center - ray.origin();
    double a = glm::dot(ray.direction(), ray.direction());
    double b = -2.0 * glm::dot(oc, ray.direction());
    double c = glm::dot(oc, oc) - m_radius * m_radius;
    double discriminant = b * b - 4 * a * c;
    if(discriminant < 0) return {};
    // Set smallest t value for given ray with respect to t_min & t_max
    double t = (-b - sqrt(discriminant)) / (2.0*a);
    if(!interval.surrounds(t)) {
        t = -b + sqrt(discriminant) / (2.0*a);
        if(!interval.surrounds(t)) return std::nullopt;
    }
    auto[u, v] = get_sphere_uv(ray.at(t));
    _HitInfo info{
        Vec3(ray.at(t)),
        glm::normalize(ray.at(t) - m_center),
        m_mat,
        t,
        u, v
    };
    Vec3 outward_normal = (info.point - m_center) / m_radius;
    info.set_face_normal(ray, outward_normal);
    return info;
}

std::pair<double, double> Sphere::get_sphere_uv(const Vec3& point) {
    double theta = std::acos(-point.y);
    double phi = std::atan2(-point.z, point.x) + PI;

    return std::pair<double, double>(phi / (2*PI), theta / PI);
}

void Quad::set_bounding_box() {
    AABB diag1 = AABB(m_point, m_point + m_u + m_v);
    AABB diag2 = AABB(m_point + m_u, m_point + m_v);
    m_bounding_box = AABB(diag1, diag2);
}

HitInfo Quad::hit(const Ray& ray, const Interval& interval) const {
    auto denom = dot(m_normal, ray.direction());

    // No hit if the ray is parallel to the plane.
    if (std::fabs(denom) < 1e-8)
        return std::nullopt;

    // Return false if the hit point parameter t is outside the ray interval.
    auto t = (m_D - dot(m_normal, ray.origin())) / denom;
    if (!interval.contains(t))
        return std::nullopt;

    Vec3 intersection = ray.at(t);
    Vec3 planar_hitpt_vector = intersection - m_point;
    double alpha = glm::dot(m_w, cross(planar_hitpt_vector, m_v));
    double beta = glm::dot(m_w, cross(m_u, planar_hitpt_vector));

    _HitInfo rec;
    if (!is_interior(alpha, beta, rec))
        return std::nullopt;

    rec.ray_t = t;
    rec.point = intersection;
    rec.mat = m_mat;
    rec.set_face_normal(ray, m_normal);

    return rec;
}

bool Quad::is_interior(double a, double b, _HitInfo& rec) const {
    Interval unit_interval = Interval(0, 1);
    // Given the hit point in plane coordinates, return false if it is outside the
    // primitive, otherwise set the hit record UV coordinates and return true.

    if (!unit_interval.contains(a) || !unit_interval.contains(b))
        return false;

    rec.u = a;
    rec.v = b;
    return true;
}