#pragma once

#include "hittable.h"
#include "rt_common.h"
#include "vec3.h"
#include <ostream>

class camera {
private:
  int image_height;
  point3 center;
  point3 pixel00_loc;
  vec3 pixel_delta_u;
  vec3 pixel_delta_v;

  void initialize() {
    image_height = int(image_width / aspect_ratio);
    image_height = (image_height > 1) ? image_height : 1;

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

  void render(const hittable &world) {
    initialize();

    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
      std::clog << "\rScanLines remaining: " << (image_height - j) << " "
                << std::flush;
      for (int i = 0; i < image_width; i++) {
        vec3 pixel_center =
            pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
        vec3 pixel_direction = pixel_center - center;
        ray r = ray(center, pixel_direction);

        auto pixel_color = ray_color(r, world);
        write_color(std::cout, pixel_color);
      }
    }
    std::clog << "\rDone              :\n";
  }
};
