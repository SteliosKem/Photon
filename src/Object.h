#pragma once

#include "Utility.h"
#include "Ray.h"
#include <memory>
#include <optional>
#include "Interval.h"

// Forward declarations
class Material;

struct _HitInfo {
    Vec3 point;
    Vec3 normal;
    shared_ptr<Material> mat;
    double ray_t;
    bool front_face;

    void set_face_normal(const Ray& ray, const Vec3& outward_normal);
};

using HitInfo = std::optional<_HitInfo>;

class Object {
public:
    virtual ~Object() = default;

    virtual HitInfo hit(const Ray& ray, const Interval& interval) const = 0;
    virtual AABB bounding_box() const = 0;
};

class ObjectList : public Object {
public:
    ObjectList() = default;
    ObjectList(shared_ptr<Object> obj) { add(obj); }
    ObjectList(const std::vector<shared_ptr<Object>>& objs) : m_objects(objs) {}

    void add(shared_ptr<Object> obj);
    HitInfo hit(const Ray& ray, const Interval& interval) const override;

    AABB bounding_box() const override { return m_bounding_box; }
    const std::vector<shared_ptr<Object>>& objects() const { return m_objects; }
    std::vector<shared_ptr<Object>>& objects() { return m_objects; }
private:
    std::vector<std::shared_ptr<Object>> m_objects;
    AABB m_bounding_box;
};

class Sphere : public Object {
public:
    Sphere() = default;
    Sphere(const Vec3& center, double radius, shared_ptr<Material> mat);

    const Vec3& center() const { return m_center; }
    const double radius() const { return m_radius; }

    HitInfo hit(const Ray& ray, const Interval& interval) const override;
    AABB bounding_box() const override { return m_bounding_box; }
private:
    Vec3 m_center;
    double m_radius;
    shared_ptr<Material> m_mat;
    AABB m_bounding_box;
};