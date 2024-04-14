#pragma once
#include "ray.h"
#include "object.h"

namespace Shapes
{
    class Sphere : public Object {
    public:
        Sphere(const Point3& center, double radius) : center(center), radius(fmax(0,radius)) {}

        bool hit(Ray& r, Interval ray_t, Hit_record& rec) const override {
            Vector3 oc = center - r.origin();
            auto a = r.direction().length_squared();
            auto h = dot(r.direction(), oc);
            auto c = oc.length_squared() - radius*radius;

            auto discriminant = h*h - a*c;
            if (discriminant < 0)
                return false;

            auto sqrtd = sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            auto root = (h - sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                root = (h + sqrtd) / a;
                if (!ray_t.surrounds(root))
                    return false;
            }

            rec.t = root;
            rec.point = r.at(rec.t);
            Vector3 outward_normal = (rec.point - center) / radius;
            rec.set_face_normal(r, outward_normal);

            return true;
        }

    private:
        Point3 center;
        double radius;
    };
}
