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
  double u, v;

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

class translate : public hittable {
private:
  shared_ptr<hittable> object;
  vec3 offset;
  aabb bbox;

public:
  translate(shared_ptr<hittable> object, const vec3 &offset) : object(object), offset(offset) {
    bbox = object->bounding_box() + offset;
  }

  aabb bounding_box() const override { return bbox; }

  bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
    ray offset_r(r.origin() - offset, r.direction());

    if (!object->hit(offset_r, ray_t, rec))
      return false;

    rec.p += offset;
    return true;
  }
};

class rotate_y : public hittable {
private:
  shared_ptr<hittable> object;
  double sin_theta;
  double cos_theta;
  aabb bbox;

public:
  rotate_y(shared_ptr<hittable> object, double angle) : object(object) {
    double radians = degrees_to_radians(angle);
    sin_theta = std::sin(radians);
    cos_theta = std::cos(radians);
    bbox = object->bounding_box();

    point3 min = point3(infinity, infinity, infinity);
    point3 max = point3(-infinity, -infinity, -infinity);

    for (int i = 0; i < 2; i++) {
      for (int j = 0; j < 2; j++) {
        for (int k = 0; k < 2; k++) {
          double x = i * bbox.x.max + (1 - i) * bbox.x.min;
          double y = j * bbox.y.max + (1 - j) * bbox.y.min;
          double z = k * bbox.z.max + (1 - k) * bbox.z.min;

          double new_x = cos_theta * x + sin_theta * z;
          double new_z = -sin_theta * x + cos_theta * z;

          vec3 temp = vec3(new_x, y, new_z);

          for (int c = 0; c < 3; c++) {
            min[c] = std::fmin(min[c], temp[c]);
            max[c] = std::fmax(max[c], temp[c]);
          }
        }
      }
    }

    bbox = aabb(min, max);
  }

  bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
    // World space -> object space
    point3 origin = point3(cos_theta * r.origin().x() - sin_theta * r.origin().z(), r.origin().y(),
                           sin_theta * r.origin().x() + cos_theta * r.origin().z());

    vec3 direction = vec3(cos_theta * r.direction().x() - sin_theta * r.direction().z(), r.direction().y(),
                          sin_theta * r.direction().x() + cos_theta * r.direction().z());

    ray rotated_r = ray(origin, direction);

    // Check hit in object space
    if (!object->hit(rotated_r, ray_t, rec))
      return false;

    // Object space -> world space
    rec.p = point3(
      cos_theta * rec.p.x() + sin_theta * rec.p.z(),
      rec.p.y(),
      -sin_theta * rec.p.x() + cos_theta * rec.p.z());

    rec.normal = vec3(
      cos_theta * rec.normal.x() + sin_theta * rec.normal.z(),
      rec.normal.y(),
      -sin_theta * rec.normal.x() + cos_theta * rec.normal.z());

    return true;
  }

  aabb bounding_box() const override { return bbox; }
};
