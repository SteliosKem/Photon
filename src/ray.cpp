#include "ray.h"
#include "general.h"

/*double Ray::hit_sphere(Shapes::Sphere sphere) {
    Vector3 oc = sphere.center - origin();
    auto a = direction().length_squared();
    auto h = dot(direction(), oc);
    auto c = oc.length_squared() - sphere.radius*sphere.radius;
    auto discriminant = h*h - a*c;

    if (discriminant < 0) {
        return -1.0;
    } else {
        return (h - sqrt(discriminant)) / a;
    }
}*/

