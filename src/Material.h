#pragma once
#include "Object.h"
#include "Utility.h"
#include "Ray.h"
#include "Texture.h"
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
    virtual Color emitted(double u, double v, const Vec3& point) const { return Color(0, 0, 0); }
};

class Diffuse : public Material {
public:
    Diffuse(shared_ptr<Texture> texture) : m_tex(texture) {}
    Diffuse(const Color& albedo) : Diffuse(make_shared<SolidColor>(albedo)) {}

    Scattering scatter(const Ray& in, const HitInfo& info) const override;
private:
    shared_ptr<Texture> m_tex;
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

class DiffuseLight : public Material {
public:
    DiffuseLight(shared_ptr<Texture> texture) : m_tex(texture) {}
    DiffuseLight(const Color& color) : m_tex(make_shared<SolidColor>(color)) {}

    Color emitted(double u, double v, const Vec3& point) const override;
private:
    shared_ptr<Texture> m_tex;
};