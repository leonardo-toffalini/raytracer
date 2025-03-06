#pragma once

#include "rt_common.h"
#include "rtw_stb_image.h"

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

  solid_color(double red, double green, double blue) : solid_color(color(red, green, blue)) {}

  color value(double u, double v, const point3 &p) const override { return albedo; }
};

class checker_texture : public texture {
private:
  double inv_scale;
  shared_ptr<texture> even;
  shared_ptr<texture> odd;

public:
  checker_texture(double scale, shared_ptr<texture> even, shared_ptr<texture> odd)
      : inv_scale(1.0 / scale), even(even), odd(odd) {}

  checker_texture(double scale, const color &c1, const color &c2)
      : checker_texture(scale, make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}

  color value(double u, double v, const point3 &p) const override {
    int x_int = int(std::floor(inv_scale * p.x()));
    int y_int = int(std::floor(inv_scale * p.y()));
    int z_int = int(std::floor(inv_scale * p.z()));

    bool is_even = (x_int + y_int + z_int) % 2 == 0;

    return is_even ? even->value(u, v, p) : odd->value(u, v, p);
  }
};

class image_texture : public texture {
private:
  rtw_image image;

public:
  image_texture(const char *filename) : image(filename) {}

  color value(double u, double v, const point3 &p) const override {
    // If we have no texture data, then return solid cyan as a debugging aid.
    if (image.height() <= 0)
      return color(0, 1, 1);

    // Clamp input texture coordinates to [0,1] x [1,0]
    u = interval(0, 1).clamp(u);
    v = 1.0 - interval(0, 1).clamp(v); // Flip V to image coordinates

    auto i = int(u * image.width());
    auto j = int(v * image.height());
    auto pixel = image.pixel_data(i, j);

    auto color_scale = 1.0 / 255.0;
    return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
  }
};
