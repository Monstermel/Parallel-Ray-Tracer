#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

#include "camera.h"
#include "color.h"
#include "hittable_list.h"
#include "material.h"
#include "render.h"
#include "rtweekend.h"
#include "sphere.h"

using namespace std::chrono;
using std::stoi, std::stod;

struct Material_setup {
    double ref;
    double R1;
    double G1;
    double B1;
    double R2;
    double G2;
    double B2;
    int rand;
};

hittable_list random_scene(const Material_setup& config) {
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

    if (config.rand) {
        for (int a = -11; a < 11; a++) {
            for (int b = -11; b < 11; b++) {
                auto choose_mat = random_double();
                point3 center(a + 0.9 * random_double(), 0.2,
                              b + 0.9 * random_double());

                if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                    shared_ptr<material> sphere_material;

                    if (choose_mat < 0.8) {
                        auto albedo = color::random() * color::random();
                        sphere_material = make_shared<lambertian>(albedo);
                        world.add(
                            make_shared<sphere>(center, 0.2, sphere_material));
                    } else if (choose_mat < 0.95) {
                        auto albedo = color::random(0.5, 1);
                        auto fuzz = random_double(0, 0.5);
                        sphere_material = make_shared<metal>(albedo, fuzz);
                        world.add(
                            make_shared<sphere>(center, 0.2, sphere_material));
                    } else {
                        sphere_material = make_shared<dielectric>(1.5);
                        world.add(
                            make_shared<sphere>(center, 0.2, sphere_material));
                    }
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(config.ref);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 =
        make_shared<lambertian>(color(config.R1, config.G1, config.B1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 =
        make_shared<metal>(color(config.R2, config.G2, config.B2), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));

    return world;
}

int main(int argc, char* argv[]) {
    const int workers = stoi(argv[1]);
    const auto aspect_ratio = stod(argv[2]) / stod(argv[3]);
    const int image_width = stoi(argv[4]);
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = stoi(argv[5]);
    const int max_depth = stoi(argv[6]);

    auto world = random_scene({stod(argv[16]), stod(argv[10]), stod(argv[11]),
                               stod(argv[12]), stod(argv[13]), stod(argv[14]),
                               stod(argv[15]), stoi(argv[17])});

    point3 lookfrom(std::stoi(argv[7]), std::stoi(argv[8]), std::stoi(argv[9]));
    point3 lookat(0, 0, 0);
    vec3 vup(0, 1, 0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.1;

    camera cam(lookfrom, lookat, vup, 20, aspect_ratio, aperture,
               dist_to_focus);

    matrix image;
    config setup = {image_width, image_height, samples_per_pixel, max_depth};
    if (workers == 1) {
        auto start = high_resolution_clock::now();
        sequential_render(setup, world, cam, image);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(end - start);
        std::cout << duration.count();
    } else {
        auto start = high_resolution_clock::now();
        parallel_render(setup, world, cam, image, workers);
        auto end = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(end - start);
        std::cout << duration.count();
    }

    std::ofstream outputFile("image.ppm");
    if (outputFile.is_open()) {
        save_image(outputFile, setup, image);
        outputFile.close();
    }

    return 0;
}