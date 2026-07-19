#pragma once

#include "Utility.h"
class Texture {
public:
    virtual ~Texture() = default;

    virtual Color value(double u, double v, const Vec3& point) const = 0;
};

class SolidColor : public Texture {
public:
    SolidColor(const Color& color) : m_color{color} {}
    SolidColor(double r, double g, double b) : SolidColor(Color(r, g, b)) {}

    Color value(double u, double v, const Vec3& point) const override { return m_color; }
private:
    Color m_color;
};

class CheckerTexture : public Texture {
public:
    CheckerTexture(double scale, shared_ptr<Texture> t0, shared_ptr<Texture> t1) : m_inv_scale(1.0/scale), m_t0(t0), m_t1(t1) {}
    CheckerTexture(double scale, const Color& color0, const Color& color1) 
                : CheckerTexture(scale, make_shared<SolidColor>(color0), make_shared<SolidColor>(color1)) {}

    Color value(double u, double v, const Vec3& point) const override;
private:
    double m_inv_scale;
    shared_ptr<Texture> m_t0, m_t1;
};