#pragma once

#include "hittable.h"

class sphere : public hittable {
private:
  point3 center;
  double radius;

public:
  sphere(const point3 &center, double radius)
      : center(center), radius(std::fmax(0, radius)) {}

  bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
    vec3 oc = center - r.origin();
    double a = r.direction().length_squared();
    double h = dot(r.direction(), oc);
    double c = oc.length_squared() - (radius * radius);
    double disc = h * h - a * c;

    if (disc < 0) {
      return false;
    }

    double disc_sqrt = std::sqrt(disc);

    // Nearest root in the acceptable range
    double root = (h - disc_sqrt) / a;
    if (!ray_t.sorrounds(root)) {
      root = (h + disc_sqrt) / a;
      if (!ray_t.sorrounds(root)) {
        return false;
      }
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);

    return true;
  }
};
