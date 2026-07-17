#include "Camera.h"
#include <iostream>
#include "Color.h"
#include "Utility.h"

Color Camera::ray_color(const Ray& ray, const Object& object) const {
    HitInfo info = object.hit(ray, Interval(0, INF));
    if (info)
        return 0.5 * (info->normal + Color(1, 1, 1));
    Vec3 unit_dir = glm::normalize(ray.direction());
    double a = 0.5*(unit_dir.y + 1.0);
    return (1-a) * Color(1, 1, 1) + a * Color(0.5, 0.7, 1.0);
}

void Camera::init() {
    m_image_height = std::max((int)(m_image_width / m_aspect_ratio), 1);

// Viewport Parameters

    m_focal_length = 1.0;
    m_viewport_height = 2.0;
    m_viewport_width = m_viewport_height * (double)m_image_width / (double)m_image_height;
    m_camera_center = Vec3(0, 0, 0);

    m_viewport_u = Vec3(m_viewport_width, 0, 0);
    m_viewport_v = Vec3(0, -m_viewport_height, 0);
    m_viewport_du = m_viewport_u / (double)m_image_width;
    m_viewport_dv = m_viewport_v / (double)m_image_height;

    m_upper_left = m_camera_center - Vec3(0, 0, m_focal_length) - 0.5 * (m_viewport_u + m_viewport_v);
    m_first_pixel_location = m_upper_left + 0.5 * (m_viewport_du + m_viewport_dv);
}

void Camera::render(const Object& scene) {
    std::cout << "P3\n" << m_image_width << " " << m_image_height << "\n255\n";

    for (int j = 0; j < m_image_height; j++) {
        std::clog << "\rScanlines remaining: " << (m_image_height - j) << ' ' << std::flush;

        for (int i = 0; i < m_image_width; i++) {
            Color pixel_color = Color(0, 0, 0);
            for(int sample = 0; sample < m_samples_per_pixel; sample++) {
                Ray ray = get_ray(i, j);
                pixel_color += ray_color(ray, scene);
            }
            write_color(std::cout, (1.0 / m_samples_per_pixel) * pixel_color);
        }
    }
}

Ray Camera::get_ray(int i, int j) const {
    Vec3 offset(random_double() - 0.5, random_double() - 0.5, 0);
    Vec3 pixel_sample = m_first_pixel_location + (i + offset.x) * m_viewport_du + (j + offset.y) * m_viewport_dv;
    Vec3 ray_origin = m_camera_center;
    Vec3 ray_dir = pixel_sample - m_camera_center;

    return Ray(ray_origin, ray_dir);
}