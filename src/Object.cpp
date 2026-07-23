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

shared_ptr<ObjectList> box(const Vec3& a, const Vec3& b, shared_ptr<Material> mat)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    auto sides = make_shared<ObjectList>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    auto min = Vec3(std::fmin(a.x,b.x), std::fmin(a.y,b.y), std::fmin(a.z,b.z));
    auto max = Vec3(std::fmax(a.x,b.x), std::fmax(a.y,b.y), std::fmax(a.z,b.z));

    auto dx = Vec3(max.x - min.x, 0, 0);
    auto dy = Vec3(0, max.y - min.y, 0);
    auto dz = Vec3(0, 0, max.z - min.z);

    sides->add(make_shared<Quad>(Vec3(min.x, min.y, max.z),  dx,  dy, mat)); // front
    sides->add(make_shared<Quad>(Vec3(max.x, min.y, max.z), -dz,  dy, mat)); // right
    sides->add(make_shared<Quad>(Vec3(max.x, min.y, min.z), -dx,  dy, mat)); // back
    sides->add(make_shared<Quad>(Vec3(min.x, min.y, min.z),  dz,  dy, mat)); // left
    sides->add(make_shared<Quad>(Vec3(min.x, max.y, max.z),  dx, -dz, mat)); // top
    sides->add(make_shared<Quad>(Vec3(min.x, min.y, min.z),  dx,  dz, mat)); // bottom

    return sides;
}

HitInfo Translate::hit(const Ray& ray, const Interval& interval) const {
    Ray translated(ray.origin() - m_translation, ray.direction());

    HitInfo info = m_object->hit(translated, interval);
    if(!info) return std::nullopt;

    info->point += m_translation;
    return info;
}

HitInfo RotateY::hit(const Ray& ray, const Interval& interval) const {
     // Transform the ray from world space to object space.

    auto origin = Vec3(
        (m_cos * ray.origin().x) - (m_sin * ray.origin().z),
        ray.origin().y,
        (m_sin * ray.origin().x) + (m_cos * ray.origin().z)
    );

    auto direction = Vec3(
        (m_cos * ray.direction().x) - (m_sin * ray.direction().z),
        ray.direction().y,
        (m_sin * ray.direction().x) + (m_cos * ray.direction().z)
    );

    Ray rotated_ray(origin, direction);

    // Determine whether an intersection exists in object space (and if so, where).
    HitInfo rec = m_object->hit(rotated_ray, interval);
    if (!rec)
        return std::nullopt;

    // Transform the intersection from object space back to world space.

    rec->point = Vec3(
        (m_cos * rec->point.x) + (m_sin * rec->point.z),
        rec->point.y,
        (-m_sin * rec->point.x) + (m_cos * rec->point.z)
    );

    rec->normal = Vec3(
        (m_cos * rec->normal.x) + (m_sin * rec->normal.z),
        rec->normal.y,
        (-m_sin * rec->normal.x) + (m_cos * rec->normal.z)
    );

    return rec;
}

RotateY::RotateY(shared_ptr<Object> object, double angle) : m_object(object) {
    auto radians = degrees_to_radians(angle);
    m_sin = std::sin(radians);
    m_cos = std::cos(radians);
    m_bounding_box = object->bounding_box();

    Vec3 min( INF,  INF,  INF);
    Vec3 max(-INF, -INF, -INF);

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                auto x = i*m_bounding_box.x().max() + (1-i)*m_bounding_box.x().min();
                auto y = j*m_bounding_box.y().max() + (1-j)*m_bounding_box.y().min();
                auto z = k*m_bounding_box.z().max() + (1-k)*m_bounding_box.z().min();

                auto newx =  m_cos*x + m_sin*z;
                auto newz = -m_sin*x + m_cos*z;

                Vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
                    min[c] = std::fmin(min[c], tester[c]);
                    max[c] = std::fmax(max[c], tester[c]);
                }
            }
        }
    }

    m_bounding_box = AABB(min, max);
}