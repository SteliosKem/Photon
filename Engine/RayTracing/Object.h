#pragma once

#include "Utility.h"
#include "Ray.h"
#include <memory>
#include <optional>
#include "Interval.h"
#include "Texture.h"

// Forward declarations
class Material;

struct _HitInfo {
    Vec3 point;
    Vec3 normal;
    shared_ptr<Material> mat;
    double ray_t;
    double u, v;
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

    static std::pair<double, double> get_sphere_uv(const Vec3& point);
private:
    Vec3 m_center;
    double m_radius;
    shared_ptr<Material> m_mat;
    AABB m_bounding_box;
};

class Quad : public Object {
public:
    Quad(const Vec3& point, const Vec3& u, const Vec3& v, shared_ptr<Material> mat) : m_point(point), m_u(u), m_v(v), m_mat(mat) { 
        set_bounding_box();
        Vec3 n = glm::cross(u, v);
        m_normal = glm::normalize(n);
        m_D = glm::dot(m_normal, m_point);
        m_w = n / glm::dot(n, n);
    }

    virtual void set_bounding_box();
    HitInfo hit(const Ray& ray, const Interval& interval) const override;
    AABB bounding_box() const override { return m_bounding_box; }
private:
    virtual bool is_interior(double a, double b, _HitInfo& rec) const;
private:
    Vec3 m_point, m_u, m_v;
    shared_ptr<Material> m_mat;
    AABB m_bounding_box;
    Vec3 m_normal;
    Vec3 m_w;
    double m_D;
};

shared_ptr<ObjectList> box(const Vec3& a, const Vec3& b, shared_ptr<Material> mat);

class Translate : public Object {
public:
    Translate(shared_ptr<Object> obj, const Vec3& transation) : m_object(obj), m_translation(transation) 
        { m_bounding_box = m_object->bounding_box() + m_translation; } 
    HitInfo hit(const Ray& ray, const Interval& interval) const override;
    AABB bounding_box() const override { return m_bounding_box; }
private:
    shared_ptr<Object> m_object;
    Vec3 m_translation;
    AABB m_bounding_box;
};

class RotateY : public Object {
public:
    RotateY(shared_ptr<Object> object, double angle);

    HitInfo hit(const Ray& ray, const Interval& interval) const override;
    AABB bounding_box() const override { return m_bounding_box; }
private:
    shared_ptr<Object> m_object;
    double m_sin;
    double m_cos;
    AABB m_bounding_box;
};