#pragma once

#include "rt_common.h"
#include <algorithm>

class bvh_node : public hittable {
private:
  shared_ptr<hittable> left;
  shared_ptr<hittable> right;
  aabb bbox;

  static bool box_compare(const shared_ptr<hittable> a,
                          const shared_ptr<hittable> b, int axis_index) {
    interval a_axis_interval = a->bounding_box().axis_interval(axis_index);
    interval b_axis_interval = b->bounding_box().axis_interval(axis_index);
    return a_axis_interval.min < b_axis_interval.min;
  }

  static bool box_x_compare(const shared_ptr<hittable> a,
                            const shared_ptr<hittable> b) {
    return box_compare(a, b, 0);
  }

  static bool box_y_compare(const shared_ptr<hittable> a,
                            const shared_ptr<hittable> b) {
    return box_compare(a, b, 1);
  }

  static bool box_z_compare(const shared_ptr<hittable> a,
                            const shared_ptr<hittable> b) {
    return box_compare(a, b, 2);
  }

public:
  bvh_node(hittable_list list)
      : bvh_node(list.objects, 0, list.objects.size()) {}

  bvh_node(std::vector<shared_ptr<hittable>> &objects, size_t start,
           size_t end) {
    bbox = aabb::empty;

    for (size_t object_index = start; object_index < end; object_index++)
      bbox = aabb(bbox, objects[object_index]->bounding_box());

    int axis = bbox.longest_axis();
    auto comparator = box_x_compare;
    if (axis == 1)
      comparator = box_y_compare;
    else if (axis == 2)
      comparator = box_z_compare;

    size_t object_span = end - start;

    if (object_span == 1) {
      left = right = objects[start];
    } else if (object_span == 2) {
      left = objects[start];
      right = objects[start + 1];
    } else {
      std::sort(std::begin(objects) + start, std::begin(objects) + end,
                comparator);

      auto mid = start + object_span / 2;
      left = make_shared<bvh_node>(objects, start, mid);
      right = make_shared<bvh_node>(objects, mid, end);
    }
  }

  bool hit(const ray &r, interval ray_t, hit_record &rec) const override {
    if (!bbox.hit(r, ray_t))
      return false;

    bool hit_left = left->hit(r, ray_t, rec);
    bool hit_right =
        right->hit(r, interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

    return hit_left || hit_right;
  }

  aabb bounding_box() const override { return bbox; }
};
