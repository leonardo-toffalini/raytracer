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
  quad(const point3 &Q, const vec3 &u, const vec3 &v, shared_ptr<material> mat)
      : Q(Q), u(u), v(v), mat(mat) {
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

class tri : public hittable {
private:
  point3 Q;
  vec3 u, v;
  vec3 w;
  shared_ptr<material> mat;
  aabb bbox;
  vec3 normal;
  double D;

public:
  tri(const point3 &Q, const vec3 &u, const vec3 &v, shared_ptr<material> mat)
      : Q(Q), u(u), v(v), mat(mat) {
    vec3 n = cross(u, v);
    normal = unit_vector(n);
    D = dot(normal, Q);
    w = n / dot(n, n);

    set_bounding_box();
  }

  tri(const point3 &A, const point3 &B, const point3 &C,
      shared_ptr<material> mat, int unused) {
    // Construct a triangle from the three points, the int unused paramter is
    // there just for the signature to differentiate the two constructors
    Q = A;
    u = vec3(B.x() - A.x(), B.y() - A.y(), B.z() - A.z());
    v = vec3(C.x() - A.x(), C.y() - A.y(), C.z() - A.z());
    mat = mat;

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
    return (a + b) <= 1 && a >= 0 && b >= 0;
  }
};
