#ifndef COLOR_H
#define COLOR_H
#include "vec3.h"



void write_color(unsigned char* dest, color pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Replace NaN components with zero. See explanation in Ray Tracing: The Rest of Your Life.
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    // Divide the color by the number of samples and gamma-correct for gamma=2.0.
    auto scale = 1.0 / samples_per_pixel;
    r = sqrt(scale * r);
    g = sqrt(scale * g);
    b = sqrt(scale * b);

    // Write the translated [0,255] value of each color component.
    dest[0] = static_cast<unsigned char>(256 * clamp(r, 0.0, 0.999));
    dest[1] = static_cast<unsigned char>(256 * clamp(g, 0.0, 0.999));
    dest[2] = static_cast<unsigned char>(256 * clamp(b, 0.0, 0.999));
}


#endif
