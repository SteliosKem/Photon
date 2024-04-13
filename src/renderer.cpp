#include "renderer.h"
#include "ray.h"

Renderer::Renderer(int img_width, int img_height) {
    image_height = img_height;
    image_width = img_width;

    viewport_width = viewport_height * (double(img_width)/img_height);
    camera_center = Point3(0, 0, 0);

    

    viewport_u = Vector3(viewport_width, 0, 0);
    viewport_v = Vector3(0, -viewport_height, 0);

    pixel_delta_u = viewport_u / double(img_width);
    pixel_delta_v = viewport_v / double(img_height);

    viewport_upper_left = camera_center - Vector3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;   
    orig_pixel = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
}

std::vector<Color3> Renderer::Render() {
    std::vector<Color3> color_vec;
    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            Point3 pixel_center = origin_pixel() + (i * pixel_delta_u) + (j * pixel_delta_v);
            Vector3 ray_direction = pixel_center - camera_center;
            Ray r(camera_center, ray_direction);
            color_vec.push_back(r.ray_color());
        }
    }
    return color_vec;
}