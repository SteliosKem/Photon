#pragma once
#include "math.h"

using namespace Math;

class Ray {
    public:
    Ray() {}

    Ray(const Point3& orig, const Vector3& _dir) : _origin(orig), _direction(_dir) {};

    const Point3& origin() {
        return _origin;
    }

    const Vector3& direction() {
        return _direction;
    }

    Point3 at(double t) const {
        return _origin + t*_direction;
    }

    Color3 ray_color() {
        Vector3 unit_direction = unit_vector(direction());
        auto a = 0.5*(unit_direction.y() + 1.0);
        return (1.0-a)*Color3(1.0, 1.0, 1.0) + a*Color3(0.5, 0.7, 1.0);
    }

    private:
    Point3 _origin;
    Point3 _direction;
};