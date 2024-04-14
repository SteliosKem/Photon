#pragma once
#include "ray.h"
#include "math.h"
#include "interval.h"

using namespace Math;

class Hit_record {
  public:
    Point3 point;
    Vector3 normal;
    double t;
    bool front_face;

    void set_face_normal(Ray& r, const Vector3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Object {
  public:
    virtual ~Object() = default;

    virtual bool hit(Ray& r, Interval ray_t, Hit_record& rec) const = 0;
};