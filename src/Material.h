#pragma once
#include "Object.h"
#include "Utility.h"
#include "Ray.h"
#include <optional>

struct _Scattering {
    Color attenuation;
    Ray scattered;
};

using Scattering = std::optional<_Scattering>;

class Material {
public:
    virtual ~Material() = default;
    virtual Scattering scatter(const Ray& in, const HitInfo& info) const { return std::nullopt; }
};

class Diffuse : public Material {
public:
    Diffuse(const Color& albedo) : m_albedo(albedo) {}

    Scattering scatter(const Ray& in, const HitInfo& info) const override;
private:
    Color m_albedo;
};

class Metal : public Material {
public:
    Metal(const Color& albedo, double fuzz) : m_albedo(albedo), m_fuzz(fuzz) {}

    Scattering scatter(const Ray& in, const HitInfo& info) const override;
private:
    Color m_albedo;
    double m_fuzz;
};

class Dielectric : public Material {
public:
    Dielectric(double refraction_index) : m_refraction_index(refraction_index) {}

    Scattering scatter(const Ray& in, const HitInfo& info) const override;
private:
    static double reflectance(double cos, double refraction_index);
private:
    double m_refraction_index;
};