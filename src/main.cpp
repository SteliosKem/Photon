#include <iostream>
#include <glm/glm.hpp>
#include "Material.h"
#include "Object.h"
#include "Camera.h"
#include "Texture.h"
#include "Utility.h"
#include "BVH.h"
#include "ConstantMedium.h"

int main() {
    ObjectList world;

    auto red   = make_shared<Diffuse>(Color(.65, .05, .05));
    auto white = make_shared<Diffuse>(Color(.73, .73, .73));
    auto green = make_shared<Diffuse>(Color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

    world.add(make_shared<Quad>(Vec3(555,0,0), Vec3(0,555,0), Vec3(0,0,555), green));
    world.add(make_shared<Quad>(Vec3(0,0,0), Vec3(0,555,0), Vec3(0,0,555), red));
    world.add(make_shared<Quad>(Vec3(343, 554, 332), Vec3(-130,0,0), Vec3(0,0,-105), light));
    world.add(make_shared<Quad>(Vec3(0,0,0), Vec3(555,0,0), Vec3(0,0,555), white));
    world.add(make_shared<Quad>(Vec3(555,555,555), Vec3(-555,0,0), Vec3(0,0,-555), white));
    world.add(make_shared<Quad>(Vec3(0,0,555), Vec3(555,0,0), Vec3(0,555,0), white));

    world.add(box(Vec3(130, 0, 65), Vec3(295, 165, 230), white));
    world.add(box(Vec3(265, 0, 295), Vec3(430, 330, 460), white));

    CameraAttributes cam_attr;
    cam_attr.aspect_ratio      = 1.0;
    cam_attr.image_width       = 600;
    cam_attr.samples_per_pixel = 200;
    cam_attr.max_ray_depth     = 50;
    cam_attr.background        = Color(0,0,0);

    cam_attr.vfov      = 40;
    cam_attr.pos       = Vec3(278, 278, -800);
    cam_attr.direction = Vec3(278, 278, 0) - cam_attr.pos;
    cam_attr.vup       = Vec3(0,1,0);
    Camera cam(cam_attr);
    cam.render(world);
    
    std::clog << "\rDone.                 \n";
    return 0;
}