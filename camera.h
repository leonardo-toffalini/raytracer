#pragma once

#include "hittable.h"
#include "rt_common.h"
#include "vec3.h"
#include <ostream>

class camera {
private:
  int image_height;
  double pixel_samples_scale;
  point3 center;
  point3 pixel00_loc;
  vec3 pixel_delta_u;
  vec3 pixel_delta_v;

  void initialize() {
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height > 1) ? image_height : 1;

    pixel_samples_scale = 1.0 / samples_per_pixel;

    center = point3(0, 0, 0);

    double focal_length = 1.0;
    double viewport_height = 2.0;
    double viewport_width =
        viewport_height * (double(image_width) / image_height);

    vec3 viewport_u = vec3(viewport_width, 0, 0);
    vec3 viewport_v = vec3(0, -viewport_height, 0);

    pixel_delta_u = viewport_u / image_width;
    pixel_delta_v = viewport_v / image_height;

    vec3 viewport_upper_left =
        center - vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
  }

  ray get_ray(int i, int j) const {
    // Generate a random ray around location i j
    vec3 offset = sample_square();
    vec3 pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) +
                        ((j + offset.y()) * pixel_delta_v);
    vec3 ray_origin = center;
    vec3 ray_direction = pixel_sample - ray_origin;

    return ray(ray_origin, ray_direction);
  }

  vec3 sample_square() const {
    // Return a vector of the form (U(-0.5, 0.5), U(-0.5, 0.5), 0)
    return vec3(random_double() - 0.5, random_double() - 0.5, 0);
  }

  color ray_color(const ray &r, const hittable &world) const {
    hit_record rec;

    if (world.hit(r, interval(0, infinity), rec)) {
      return 0.5 * (rec.normal + color(1, 1, 1));
    }

    vec3 unit_direction = unit_vector(r.direction());
    double a = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - a) * color(1.0, 1.0, 1.0) + a * color(0.5, 0.7, 1.0);
  }

public:
  double aspect_ratio = 1.0;
  int image_width = 100;
  int samples_per_pixel = 10;

  void render(const hittable &world) {
    initialize();

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
      std::clog << "\rScanLines remaining: " << (image_height - j) << " "
                << std::flush;
      for (int i = 0; i < image_width; i++) {
        color pixel_color(0, 0, 0);
        for (int sample = 0; sample < samples_per_pixel; sample++) {
          ray r = get_ray(i, j);
          pixel_color += ray_color(r, world);
        }

        write_color(std::cout, pixel_samples_scale * pixel_color);
      }
    }
    std::clog << "\rDone              :\n";
  }
};
