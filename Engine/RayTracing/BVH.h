#pragma once

#include "Interval.h"
#include "Object.h"

class BVHNode : public Object {
public:
    BVHNode(ObjectList list) : BVHNode(list.objects(), 0, list.objects().size()) {}
    BVHNode(std::vector<shared_ptr<Object>>& objects, size_t start, size_t end);

    HitInfo hit(const Ray& ray, const Interval& interval) const override;

    static bool box_compare(const shared_ptr<Object> a, const shared_ptr<Object> b, int axis_index);
    static bool box_x_compare (const shared_ptr<Object> a, const shared_ptr<Object> b);
    static bool box_y_compare (const shared_ptr<Object> a, const shared_ptr<Object> b);
    static bool box_z_compare (const shared_ptr<Object> a, const shared_ptr<Object> b);

    AABB bounding_box() const override { return m_box; }
private:
    shared_ptr<Object> m_left;
    shared_ptr<Object> m_right;
    AABB m_box;
};