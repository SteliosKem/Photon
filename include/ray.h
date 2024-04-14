#pragma once
#include "math.h"
using namespace Math;

class Ray {
    public:
    Ray() {}

    Ray(const Point3& orig, const Vector3& _dir) : _origin(orig), _direction(_dir) {};

    Point3& origin() {
        return _origin;
    }

    Vector3& direction() {
        return _direction;
    }

    Point3 at(double t) const {
        return _origin + t*_direction;
    }

    private:
    Point3 _origin;
    Point3 _direction;
};