#pragma once

#include "aabb.h"
#include "rt_common.h"

class material;

class hit_record {
public:
  point3 p;
  vec3 normal;
  shared_ptr<material> mat;
  double t;
  bool front_face;

  void set_face_normal(const ray &r, const vec3 &outward_normal) {
    // Set the hit record normal vector
    // Note: outward_normal must be a normal vector

    front_face = dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class hittable {
public:
  virtual ~hittable() = default;
  virtual bool hit(const ray &r, interval ray_t, hit_record &rec) const = 0;
  virtual aabb bounding_box() const = 0;
};
