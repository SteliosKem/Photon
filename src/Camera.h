#pragma once

#include "Utility.h"
#include "Object.h"

class Camera {
public:
    Camera() : m_aspect_ratio(16.0/9.0), m_image_width(400), m_samples_per_pixel(100) { init(); }
    Camera(double aspect_ratio, double image_width, int samples) 
        : m_aspect_ratio(aspect_ratio), m_image_width(image_width), m_samples_per_pixel(samples) { init(); }

    void render(const Object& scene);
private:
    void init();
    Color ray_color(const Ray& ray, const Object& obj) const;
    Ray get_ray(int i, int j) const;
private:
    double m_aspect_ratio;
    int m_samples_per_pixel;

    int m_image_width;
    int m_image_height;

// Viewport Parameters

    double m_focal_length;
    double m_viewport_height;
    double m_viewport_width;
    Vec3 m_camera_center;

    Vec3 m_viewport_u;
    Vec3 m_viewport_v;
    Vec3 m_viewport_du;
    Vec3 m_viewport_dv;

    Vec3 m_upper_left;
    Vec3 m_first_pixel_location;
};