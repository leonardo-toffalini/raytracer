#pragma once

#include "rt_common.h"

class sphere : public hittable {
private:
  point3 center;
  double radius;
  shared_ptr<material> mat;
  aabb bbox;

  static void get_sphere_uv(const point3 &p, double &u, double &v) {
    double theta = std::acos(-p.y());
    double phi = std::atan2(-p.z(), p.x()) + pi;

    u = phi / (2 * pi);
    v = theta / pi;
  }

public:
  sphere(const point3 &center, double radius, shared_ptr<material> mat)
      : center(center), radius(std::fmax(0, radius)), mat(mat) {
    vec3 rvec = vec3(radius, radius, radius);
    bbox = aabb(center - rvec, center + rvec);
  }

  aabb bounding_box() const override { return bbox; }

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
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat = mat;

    return true;
  }
};
