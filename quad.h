#pragma once

#include "rt_common.h"

class quad : public hittable {
private:
  point3 Q;
  vec3 u, v;
  vec3 w;
  shared_ptr<material> mat;
  aabb bbox;
  vec3 normal;
  double D;

public:
  quad(const point3 &Q, const vec3 &u, const vec3 &v, shared_ptr<material> mat) : Q(Q), u(u), v(v), mat(mat) {
    vec3 n = cross(u, v);
    normal = unit_vector(n);
    D = dot(normal, Q);
    w = n / dot(n, n);

    set_bounding_box();
  }

  virtual void set_bounding_box() {
    aabb diagonal_bbox_1 = aabb(Q, Q + u + v);
    aabb diagonal_bbox_2 = aabb(Q + u, Q + v);
    bbox = aabb(diagonal_bbox_1, diagonal_bbox_2);
  }

  aabb bounding_box() const override { return bbox; }

  bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
    double denom = dot(normal, r.direction());

    // ray is close to parallel to the plane
    if (std::fabs(denom) < 1e-8)
      return false;

    double t = (D - dot(normal, r.origin())) / denom;
    if (!ray_t.sorrounds(t))
      return false;

    // Check if the plane intersection lies withing the quad on the plane
    point3 intersection = r.at(t);
    vec3 planar_hitpt_vector = intersection - Q;
    double alpha = dot(w, cross(planar_hitpt_vector, v));
    double beta = dot(w, cross(u, planar_hitpt_vector));

    if (!is_interior(alpha, beta, rec))
      return false;

    // Set the hit record if we have successfully hit the quad
    rec.t = t;
    rec.p = intersection;
    rec.mat = mat;
    rec.set_face_normal(r, normal);

    return true;
  }

  virtual bool is_interior(double a, double b, hit_record &rec) const {
    interval unit_interval = interval(0, 1);

    if (!unit_interval.sorrounds(a) || !unit_interval.sorrounds(b))
      return false;

    return true;
  }
};

inline shared_ptr<hittable_list> box(const point3 &a, const point3 &b, shared_ptr<material> mat) {
  auto sides = make_shared<hittable_list>();

  auto min = point3(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z()));
  auto max = point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z()));

  auto dx = vec3(max.x() - min.x(), 0, 0);
  auto dy = vec3(0, max.y() - min.y(), 0);
  auto dz = vec3(0, 0, max.z() - min.z());

  sides->add(make_shared<quad>(point3(min.x(), min.y(), max.z()), dx, dy, mat));  // front
  sides->add(make_shared<quad>(point3(max.x(), min.y(), max.z()), -dz, dy, mat)); // right
  sides->add(make_shared<quad>(point3(max.x(), min.y(), min.z()), -dx, dy, mat)); // back
  sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dz, dy, mat));  // left
  sides->add(make_shared<quad>(point3(min.x(), max.y(), max.z()), dx, -dz, mat)); // top
  sides->add(make_shared<quad>(point3(min.x(), min.y(), min.z()), dx, dz, mat));  // bottom

  return sides;
}
