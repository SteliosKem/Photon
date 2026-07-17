#include "Color.h"
#include "Interval.h"

void write_color(std::ostream& out, const Color& pixel_color) {
    double r = pixel_color.x;
    double g = pixel_color.y;
    double b = pixel_color.z;

    // Translate the [0,1] component values to the byte range [0,255].
    static const Interval intensity(0, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}