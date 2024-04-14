#pragma once
#include "math.h"
#include <vector>
#include <object_list.h>

using namespace Math;

class Renderer {
public:
    Renderer(int img_width, int img_height);
    Object_list world;
        
    Vector3 origin_pixel() {return orig_pixel;}
    std::vector<Color3> Render();
    void move(Vector3 pos);
    Color3 ray_color(const Object& world, Ray& ray);
    Point3 camera_center = Point3(0, 0, 0);
private:
    int image_height, image_width;
    float focal_length = 1.0;
    float viewport_height = 2.0;
    float viewport_width;

    int samples_per_pixel = 10;
    double pixel_samples_scale;

    Ray get_ray(int i, int j) const;
    Vector3 sample_square() const;

    Vector3 viewport_u, viewport_v;

    Vector3 pixel_delta_u, pixel_delta_v;

    Vector3 viewport_upper_left;
    Vector3 orig_pixel;
};