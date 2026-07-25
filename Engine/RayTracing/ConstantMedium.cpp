#include "ConstantMedium.h"
#include "Object.h"
#include <optional>

HitInfo ConstantMedium::hit(const Ray& r, const Interval& ray_t) const {
    HitInfo rec1 = m_boundary->hit(r, Interval::R);

    if (!rec1)
        return std::nullopt;

    HitInfo rec2 = m_boundary->hit(r, Interval(rec1->ray_t+0.0001, INF));

    if (!rec2)
        return std::nullopt;

    if (rec1->ray_t < ray_t.min()) rec1->ray_t = ray_t.min();
    if (rec2->ray_t > ray_t.max()) rec2->ray_t = ray_t.max();

    if (rec1->ray_t >= rec2->ray_t)
        return std::nullopt;

    if (rec1->ray_t < 0)
        rec1->ray_t = 0;

    auto ray_length = r.direction().length();
    auto distance_inside_boundary = (rec2->ray_t - rec1->ray_t) * ray_length;
    auto hit_distance = m_neg_inv_density * std::log(random_double());

    if (hit_distance > distance_inside_boundary)
        return std::nullopt;

    _HitInfo rec;

    rec.ray_t = rec1->ray_t + hit_distance / ray_length;
    rec.point = r.at(rec.ray_t);

    rec.normal = Vec3(1,0,0);  // arbitrary
    rec.front_face = true;     // also arbitrary
    rec.mat = m_phase_function;

    return rec;
}