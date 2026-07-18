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
    auto material_left   = make_shared<Dielectric>(1.50);
    auto material_bubble = make_shared<Dielectric>(1.00 / 1.50);
    auto material_right  = make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

    scene.add(make_shared<Sphere>(Vec3( 0.0, -100.5, -1.0), 100.0, material_ground));
    scene.add(make_shared<Sphere>(Vec3( 0.0,    0.0, -1.2),   0.5, material_center));
    scene.add(make_shared<Sphere>(Vec3(-1.0,    0.0, -1.0),   0.5, material_left));
    scene.add(make_shared<Sphere>(Vec3(-1.0,    0.0, -1.0),   0.4, material_bubble));
    scene.add(make_shared<Sphere>(Vec3( 1.0,    0.0, -1.0),   0.5, material_right));

    // Render
    CameraAttributes attr{};
    attr.pos = Vec3(-2, 2, 1);
    attr.direction = Vec3(2, -2, -2);
    attr.vup = Vec3(0, 1, 0);
    attr.vfov = 20;
    Camera cam(attr);
    cam.render(scene);
    
    std::clog << "\rDone.                 \n";
    return 0;
}