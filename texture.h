#pragma once

#include "rt_common.h"

class texture {
public:
  virtual ~texture() = default;

  virtual color value(double u, double v, const point3 &p) const = 0;
};

class solid_color : public texture {
private:
  color albedo;

public:
  solid_color(color albedo) : albedo(albedo) {}

  solid_color(double red, double green, double blue)
      : solid_color(color(red, green, blue)) {}

  color value(double u, double v, const point3 &p) const override {
    return albedo;
  }
};
