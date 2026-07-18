#include <iostream>
#include <glm/glm.hpp>
#include "Material.h"
#include "Object.h"
#include "Camera.h"

int main() {

    // Scene

    ObjectList scene;
    auto material_ground = make_shared<Diffuse>(Color(0.8, 0.8, 0.0));
    auto material_center = make_shared<Diffuse>(Color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<Metal>(Color(0.8, 0.8, 0.8), 0.2);
    auto material_right  = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.8);

    scene.add(make_shared<Sphere>(Vec3( 0.0, -100.5, -1.0), 100.0, material_ground));
    scene.add(make_shared<Sphere>(Vec3( 0.0,    0.0, -1.2),   0.5, material_center));
    scene.add(make_shared<Sphere>(Vec3(-1.0,    0.0, -1.0),   0.5, material_left));
    scene.add(make_shared<Sphere>(Vec3( 1.0,    0.0, -1.0),   0.5, material_right));

    // Render
    Camera cam{};
    cam.render(scene);
    
    std::clog << "\rDone.                 \n";
    return 0;
}