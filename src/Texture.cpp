#include "Texture.h"
#include <cmath>

Color CheckerTexture::value(double u, double v, const Vec3& point) const {
    int x = int(std::floor(m_inv_scale * point.x));
    int y = int(std::floor(m_inv_scale * point.y));
    int z = int(std::floor(m_inv_scale * point.z));

    bool is_even = (x + y + z) % 2;
    return is_even ? m_t0->value(u, v, point) : m_t1->value(u, v, point);
}