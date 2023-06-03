#ifndef RENDER_H
#define RENDER_H

#include <iostream>
#include <thread>
#include <vector>

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "ray.h"

using matrix = std::vector<color>;

struct config {
    int image_width;
    int image_height;
    int samples_per_pixel;
    int max_depth;
};

color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    // If we've exceeded the ray bounce limit, no more light is gathered.
    if (depth <= 0) return color(0, 0, 0);

    if (world.hit(r, 0.001, infinity, rec)) {
        ray scattered;
        color attenuation;
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return color(0, 0, 0);
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

void sequential_render(const config& setup, const hittable_list& world,
                       const camera& cam, matrix& image) {
    image.clear();

    for (int j = setup.image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << '\n';
        for (int i = 0; i < setup.image_width; ++i) {
            color pixel_color(0, 0, 0);
            for (int s = 0; s < setup.samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (setup.image_width - 1);
                auto v = (j + random_double()) / (setup.image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, setup.max_depth);
            }
            image.push_back(pixel_color);
        }
    }

    std::cerr << "\nDone.\n";
}

void parallel_render(const config& setup, const hittable_list& world,
                     const camera& cam, std::vector<color>& image,
                     int num_threads) {
    // Resize the image matrix to match the image dimensions
    image.resize(setup.image_height * setup.image_width);

    // Define a function that will be executed by each thread
    auto render_task = [&](int start, int end) {
        for (int idx = start; idx <= end; ++idx) {
            int i = idx % setup.image_width;
            int j = setup.image_height - (floor(idx / setup.image_width) + 1);
            color pixel_color(0, 0, 0);
            for (int s = 0; s < setup.samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (setup.image_width - 1);
                auto v = (j + random_double()) / (setup.image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world, setup.max_depth);
            }
            image[idx] = pixel_color;
        }
    };

    // Create a vector to store the threads
    std::vector<std::thread> threads;

    // Calculate the number of pixels to be processed by each thread
    const int pixels_per_thread =
        (setup.image_height * setup.image_width) / num_threads;

    // Create and start the threads
    for (int t = 0; t < num_threads; t++) {
        int start = t * pixels_per_thread;
        int end = (t == (num_threads - 1))
                      ? (setup.image_height * setup.image_width) - 1
                      : (t + 1) * pixels_per_thread;
        threads.emplace_back(render_task, start, end);
    }

    // Wait for all threads to finish
    for (auto& thread : threads) {
        thread.join();
    }

    std::cerr << "\nDone.\n";
}

void save_image(std::ostream& out, const config& setup, matrix& image) {
    out << "P3\n"
        << setup.image_width << " " << setup.image_height << "\n255\n";

    for (auto& pixel_color : image) {
        write_color(out, pixel_color, setup.samples_per_pixel);
    }
}

#endif