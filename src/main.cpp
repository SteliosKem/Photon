#include <iostream>
#include <glm/glm.hpp>
#include <memory>
#include "Interval.h"
#include "Ray.h"
#include "Color.h"
#include "Object.h"
#include "Camera.h"

int main() {

    // Scene

    ObjectList scene;
    scene.add(make_shared<Sphere>(Vec3(0, 0, -1), 0.5));
    scene.add(make_shared<Sphere>(Vec3(0, -100.5, -1), 100));

    // Render
    Camera cam{};
    cam.render(scene);
    
    std::clog << "\rDone.                 \n";
    return 0;
}