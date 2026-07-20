#pragma once

#include "Object.h"
#include "Material.h"

class ConstantMedium : public Object {
public:
    ConstantMedium(shared_ptr<Object> boundary, double density, shared_ptr<Texture> texture)
        : m_boundary(boundary), m_neg_inv_density(-1/density), m_phase_function(make_shared<Isotropic>(texture)) {}

    ConstantMedium(shared_ptr<Object> boundary, double density, const Color& albedo)
        : m_boundary(boundary), m_neg_inv_density(-1/density), m_phase_function(make_shared<Isotropic>(albedo)) {}

    HitInfo hit(const Ray& r, const Interval& ray_t) const override;
    AABB bounding_box() const override { return m_boundary->bounding_box(); }
private:
    shared_ptr<Object> m_boundary;
    double m_neg_inv_density;
    shared_ptr<Material> m_phase_function;
};