#pragma once

#include "Utility.h"
#include "Object.h"

struct CameraAttributes {
    double aspect_ratio = 16.0/9.0;
    int image_width = 400;
    int samples_per_pixel = 200;
    int max_ray_depth = 50;
    double vfov = 90.0;
    Vec3 pos = Vec3(0, 0, 0);
    Vec3 vup = Vec3(0, 1, 0);
    Vec3 direction = Vec3(0, 0, -1);
};

class Camera {
public:
    Camera() { init(); }
    Camera(const CameraAttributes& attributes) : m_attributes(attributes) { init(); }

    void render(const Object& scene);
private:
    void init();
    Color ray_color(const Ray& ray, const Object& obj, int depth) const;
    Ray get_ray(int i, int j) const;
private:
    CameraAttributes m_attributes{};
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

    Vec3 u, v, w;
};