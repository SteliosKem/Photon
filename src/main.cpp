#include <iostream>
#include <glm/glm.hpp>
#include "Material.h"
#include "Object.h"
#include "Camera.h"
#include "Texture.h"
#include "Utility.h"
#include "BVH.h"

int main() {

    // Scene

    ObjectList scene;

    auto checker = make_shared<CheckerTexture>(0.32, Color(.2, .3, .1), Color(.9, .9, .9));
    auto ground_material = make_shared<Diffuse>(checker);
    auto diff_light = make_shared<DiffuseLight>(Color(2, 2, 2));
    scene.add(make_shared<Sphere>(Vec3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            Vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - Vec3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = random_vec();
                    sphere_material = make_shared<Diffuse>(albedo);
                    scene.add(make_shared<Sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = random_vec(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    scene.add(make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    scene.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    scene.add(make_shared<Sphere>(Vec3(0, 1, 0), 1.0, material1));

    scene.add(make_shared<Sphere>(Vec3(-4, 1, 0), 1.0, diff_light));

    auto material3 = make_shared<Metal>(Vec3(0.7, 0.6, 0.5), 0.0);
    scene.add(make_shared<Sphere>(Vec3(4, 1, 0), 1.0, material3));

    scene = ObjectList(make_shared<BVHNode>(scene));

    // Render
    CameraAttributes attr{};
    attr.pos = Vec3(5,2,4);
    attr.direction = Vec3(-13, -3, -13);
    attr.vup = Vec3(0, 1, 0);
    attr.vfov = 20;
    attr.image_width = 1200;
    attr.samples_per_pixel = 200;
    attr.max_ray_depth = 50;
    Camera cam(attr);
    cam.render(scene);
    
    std::clog << "\rDone.                 \n";
    return 0;
}