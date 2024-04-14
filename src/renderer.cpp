#include "renderer.h"
#include "ray.h"
#include <memory>
#include "shapes.h"
#include "general.h"

using std::make_shared;

Color3 Renderer::ray_color(const Object& world, Ray& ray) {
    Hit_record record;
    if (world.hit(ray, Interval(0, infinity), record)) {
        return 0.5*(record.normal + Color3(1, 1, 1));
    }

    Vector3 unit_direction = unit_vector(ray.direction());
    auto a = 0.5*(unit_direction.y() + 1.0);
    return (1.0-a)*Color3(1.0, 1.0, 1.0) + a*Color3(0.5, 0.7, 1.0);
}

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
    
    pixel_samples_scale = 1.0 / samples_per_pixel;
    
}

std::vector<Color3> Renderer::Render() {
    std::vector<Color3> color_vec;
    for (int j = 0; j < image_height; j++) {
        for (int i = 0; i < image_width; i++) {
            /auto pixel_center = orig_pixel + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            Ray r(camera_center, ray_direction);

            color_vec.push_back(ray_color(world, r));
            
            Color3 pixel_color(0,0,0);
            for (int sample = 0; sample < samples_per_pixel; sample++) {
                Ray r = get_ray(i, j);
                pixel_color += ray_color(world, r);
            }
            color_vec.push_back(pixel_color);
        }
    }
    return color_vec;
}

void Renderer::move(Vector3 pos) {
    camera_center = pos;
}

Ray Renderer::get_ray(int i, int j) const {
    Vector3 offset = sample_square();
    Vector3 pixel_sample = orig_pixel + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

    Point3 ray_origin = camera_center;
    Vector3 ray_direction = pixel_sample - ray_origin;

    return Ray(ray_origin, ray_direction);
}

Vector3 Renderer::sample_square() const {
    return Vector3(random_double() - 0.5, random_double() - 0.5, 0);
}