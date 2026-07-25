#include "BVH.h"
#include <cstddef>
#include <optional>
#include <algorithm>

BVHNode::BVHNode(std::vector<shared_ptr<Object>>& objects, size_t start, size_t end) {
    m_box = AABB::empty;
    for(size_t obj_idx = start; obj_idx < end; obj_idx++) {
        m_box = AABB(m_box, objects[obj_idx]->bounding_box());
    }
    int axis = m_box.longest_axis();

    auto comparator = (axis == 0) ? box_x_compare
                    : (axis == 1) ? box_y_compare
                    : box_z_compare;

    size_t object_span = end - start;
    if(object_span == 1) {
        m_left = m_right = objects[start];
    }
    else if (object_span == 2) {
        m_left = objects[start];
        m_right = objects[start + 1];
    }
    else {
        std::sort(std::begin(objects) + start, std::begin(objects) + end, comparator);
        size_t mid = start + object_span/2;
        m_left = make_shared<BVHNode>(objects, start, mid);
        m_right = make_shared<BVHNode>(objects, mid, end);
    }
}

HitInfo BVHNode::hit(const Ray& ray, const Interval& interval) const {
    if(!m_box.hit(ray, interval)) return std::nullopt;
    HitInfo hit_left = m_left->hit(ray, interval);
    HitInfo hit_right = m_right->hit(ray, Interval(interval.min(), hit_left ? hit_left->ray_t : interval.max()));
    return hit_left ? (hit_right ? hit_right : hit_left) : hit_right;
}

bool BVHNode::box_compare(const shared_ptr<Object> a, const shared_ptr<Object> b, int axis_index) {
    auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
    auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
    return a_axis_interval.min() < b_axis_interval.min();
}

bool BVHNode::box_x_compare (const shared_ptr<Object> a, const shared_ptr<Object> b) {
    return box_compare(a, b, 0);
}

bool BVHNode::box_y_compare (const shared_ptr<Object> a, const shared_ptr<Object> b) {
    return box_compare(a, b, 1);
}

bool BVHNode::box_z_compare (const shared_ptr<Object> a, const shared_ptr<Object> b) {
    return box_compare(a, b, 2);
}