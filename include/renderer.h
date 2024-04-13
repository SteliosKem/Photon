#pragma once
#include "math.h"
#include <vector>

using namespace Math;

class Renderer {
public:
    Renderer(int img_width, int img_height);

    Vector3 origin_pixel() {return orig_pixel;}
    std::vector<Color3> Render();

private:
    int image_height, image_width;
    float focal_length = 1.0;
    float viewport_height = 2.0;
    float viewport_width;
    Point3 camera_center = Point3(0, 0, 0);

    Vector3 viewport_u, viewport_v;

    Vector3 pixel_delta_u, pixel_delta_v;

    Vector3 viewport_upper_left;
    Vector3 orig_pixel;
};