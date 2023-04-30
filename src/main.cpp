#include <iostream>

#include "color.h"
#include "vec3.h"

int main(void) {
    // Image
    const int image_width = 256;
    const int image_height = 256;

    // Render
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << '\n';
        for (int i = 0; i < image_width; ++i) {
            write_color(std::cout, color(double(i) / (image_width - 1),
                                         double(j) / (image_height - 1), 0.25));
        }
    }
    std::cerr << "\nDone.\n";
    return 0;
}