#include "Material.h"
#include "Utility.h"

Scattering Diffuse::scatter(const Ray& ray, const HitInfo& info) const {
    Vec3 scatter_direction = info->normal + random_unit_vec();
    if(near_zero(scatter_direction)) scatter_direction = info->normal;
    _Scattering scattering{};
    scattering.scattered = Ray(info->point, scatter_direction);
    scattering.attenuation = m_albedo;
    return scattering;
}

Scattering Metal::scatter(const Ray& ray, const HitInfo& info) const {
    Vec3 reflected = reflect(ray.direction(), info->normal);
    reflected = glm::normalize(reflected) + m_fuzz * random_unit_vec();
    _Scattering scattering{};
    scattering.scattered = Ray(info->point, reflected);
    scattering.attenuation = m_albedo;
    if(glm::dot(scattering.scattered.direction(), info->normal) <= 0) return std::nullopt;
    return scattering;
}

Scattering Dielectric::scatter(const Ray& ray, const HitInfo& info) const {
    _Scattering scatter{};
    scatter.attenuation = Color(1.0, 1.0, 1.0);
    double ri = info->front_face ? (1.0/m_refraction_index) : m_refraction_index;

    Vec3 unit_direction = glm::normalize(ray.direction());
    double cos_theta = std::fmin(dot(-unit_direction, info->normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

    bool cannot_refract = ri * sin_theta > 1.0;
    Vec3 direction;

    if (cannot_refract || reflectance(cos_theta, ri) > random_double())
        direction = reflect(unit_direction, info->normal);
    else
        direction = refract(unit_direction, info->normal, ri);

    scatter.scattered = Ray(info->point, direction);
    return scatter;
}

double Dielectric::reflectance(double cosine, double refraction_index) {
    double r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0*r0;
    return r0 + (1-r0)*std::pow((1 - cosine),5);
}