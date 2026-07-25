#pragma once
#include "Utility.h"
#include "Ray.h"

class Interval {
public:
    Interval() : m_min(INF), m_max(-INF) {}
    Interval(double min, double max) : m_min(min), m_max(max) {}
    Interval(const Interval& a, const Interval& b) {
        // Create the interval tightly enclosing the two input intervals.
        m_min = a.m_min <= b.m_min ? a.m_min : b.m_min;
        m_max = a.m_max >= b.m_max ? a.m_max : b.m_max;
    }

    double size() const { return m_max - m_min; }
    bool contains(double x) const { return m_min <= x && x <= m_max; }
    bool surrounds(double x) const { return m_min < x && x < m_max; }

    const static Interval empty;
    const static Interval R;

    double max() const { return m_max; }
    double min() const { return m_min; }

    double& max() { return m_max; }
    double& min() { return m_min; }

    double clamp(double x) const { if(x < m_min) return m_min; if (x > m_max) return m_max; return x;}
    Interval expand(double delta) const { return Interval(m_min - delta/2.0, m_max + delta/2.0); }
private:
    double m_min;
    double m_max;
};

inline Interval operator+(const Interval& interval, double displacement) {
    return Interval(interval.min() + displacement, interval.max() + displacement);
}

inline Interval operator+(double displacement, const Interval& interval) {
    return Interval(interval.min() + displacement, interval.max() + displacement);
}

inline const Interval Interval::empty = Interval();
inline const Interval Interval::R = Interval(-INF, INF);

class AABB {
public:
    AABB() { pad_to_minimums(); }
    AABB(const Interval& x, const Interval& y, const Interval& z) : m_x(x), m_y(y), m_z(z) { pad_to_minimums(); }
    AABB(const Vec3& a, const Vec3& b) {
        // Treat the two points a and b as extrema for the bounding box, so we don't require a
        // particular minimum/maximum coordinate order.

        m_x = (a.x <= b.x) ? Interval(a.x, b.x) : Interval(b.x, a.x);
        m_y = (a.y <= b.y) ? Interval(a.y, b.y) : Interval(b.y, a.y);
        m_z = (a.z <= b.z) ? Interval(a.z, b.z) : Interval(b.z, a.z);

        pad_to_minimums();
    }
    AABB(const AABB& box0, const AABB& box1) {
        m_x = Interval(box0.m_x, box1.m_x);
        m_y = Interval(box0.m_y, box1.m_y);
        m_z = Interval(box0.m_z, box1.m_z);

        pad_to_minimums();
    }

    const Interval& axis_interval(int n) const {
        switch (n) {
            case 1: return m_y;
            case 2: return m_z;
            case 0: 
            default:
            return m_x;
        };
    }

    bool hit(const Ray& ray, Interval ray_t) const {
        const Vec3& ray_orig = ray.origin();
        const Vec3& ray_dir  = ray.direction();

        for (int axis = 0; axis < 3; axis++) {
            const Interval& ax = axis_interval(axis);
            const double adinv = 1.0 / ray_dir[axis];

            double t0 = (ax.min() - ray_orig[axis]) * adinv;
            double t1 = (ax.max() - ray_orig[axis]) * adinv;

            if (t0 < t1) {
                if (t0 > ray_t.min()) ray_t.min() = t0;
                if (t1 < ray_t.max()) ray_t.max() = t1;
            } else {
                if (t1 > ray_t.min()) ray_t.min() = t1;
                if (t0 < ray_t.max()) ray_t.max() = t0;
            }

            if (ray_t.max() <= ray_t.min())
                return false;
        }
        return true;
    }

    int longest_axis() const {
        if(m_x.size() > m_y.size())
            return m_x.size() > m_z.size() ? 0 : 2;
        return m_y.size() > m_z.size() ? 1 : 2;
    }

    const Interval& x() const { return m_x; }
    const Interval& y() const { return m_y; }
    const Interval& z() const { return m_z; }

    const static AABB empty;
    const static AABB universe;
private:
    void pad_to_minimums() {
        const static double delta = 0.0001;
        if(m_x.size() < delta) m_x.expand(delta);
        if(m_y.size() < delta) m_y.expand(delta);
        if(m_z.size() < delta) m_z.expand(delta);
    }
private:
    Interval m_x;
    Interval m_y;
    Interval m_z;
};

inline AABB operator+(const AABB& box, const Vec3& offset) {
    return AABB(box.x() + offset.x, box.y() + offset.y, box.z() + offset.z);
}

inline AABB operator+(const Vec3& offset, const AABB& box) {
    return box + offset;
}

inline const AABB AABB::empty = AABB(); 