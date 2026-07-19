#include "Camera.h"
#include <iostream>
#include "Color.h"
#include "Material.h"
#include "Utility.h"

Color Camera::ray_color(const Ray& ray, const Object& object, int depth) const {
    if (depth <= 0) return Color(0, 0, 0);
    HitInfo info = object.hit(ray, Interval(0.001, INF));
    if(!info)
        return m_attributes.background;

    Color emission_color = info->mat->emitted(info->u, info->v, info->point);
    Scattering s = info->mat->scatter(ray, info);
    if(!s) return emission_color;

    return s->attenuation * ray_color(s->scattered, object, depth - 1) + emission_color;
}

void Camera::init() {
    m_image_height = std::max((int)(m_attributes.image_width / m_attributes.aspect_ratio), 1);
    m_camera_center = m_attributes.pos;
// Viewport Parameters

    m_focal_length = glm::length(m_attributes.direction);
    double theta = degrees_to_radians(m_attributes.vfov);
    double h = std::tan(theta/2);
    m_viewport_height = 2 * h * m_focal_length;
    m_viewport_width = m_viewport_height * (double)m_attributes.image_width / (double)m_image_height;
    // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
    w = glm::normalize(-m_attributes.direction);
    u = glm::normalize(cross(m_attributes.vup, w));
    v = cross(w, u);

    // Calculate the vectors across the horizontal and down the vertical viewport edges.
    m_viewport_u = m_viewport_width * u;    // Vector across viewport horizontal edge
    m_viewport_v = m_viewport_height * -v;  // Vector down viewport vertical edge
    m_viewport_du = m_viewport_u / (double)m_attributes.image_width;
    m_viewport_dv = m_viewport_v / (double)m_image_height;

    m_upper_left = m_camera_center - m_focal_length * w - 0.5 * (m_viewport_u + m_viewport_v);
    m_first_pixel_location = m_upper_left + 0.5 * (m_viewport_du + m_viewport_dv);
}

void Camera::render(const Object& scene) {
    std::cout << "P3\n" << m_attributes.image_width << " " << m_image_height << "\n255\n";

    for (int j = 0; j < m_image_height; j++) {
        std::clog << "\rScanlines remaining: " << (m_image_height - j) << ' ' << std::flush;

        for (int i = 0; i < m_attributes.image_width; i++) {
            Color pixel_color = Color(0, 0, 0);
            for(int sample = 0; sample < m_attributes.samples_per_pixel; sample++) {
                Ray ray = get_ray(i, j);
                pixel_color += ray_color(ray, scene, m_attributes.max_ray_depth);
            }
            write_color(std::cout, (1.0 / m_attributes.samples_per_pixel) * pixel_color);
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