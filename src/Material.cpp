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
    _Scattering scattering{};
    scattering.scattered = Ray(info->point, reflected);
    scattering.attenuation = m_albedo;
    return scattering;
}