#pragma once

class interval {
public:
  double min, max;

  interval() : min(+infinity), max(-infinity) {}
  interval(double min, double max) : min(min), max(max) {}

  interval(const interval &a, const interval &b) {
    min = a.min <= b.min ? a.min : b.min;
    max = a.max >= b.max ? a.max : b.max;
  }

  double size() const { return max - min; }
  double sorrounds(double x) const { return min < x && x < max; }

  double clamp(double x) const {
    if (x < min)
      return min;
    if (x > max)
      return max;
    return x;
  }

  interval expand(double epsilon) const {
    double padding = epsilon / 2;
    return interval(min - padding, max + padding);
  }

  static const interval empty, universe;
};

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

interval operator+(const interval &ival, double displacement) {
  return interval(ival.min + displacement, ival.max + displacement);
}

interval operator+(double displacement, const interval &ival) { return ival + displacement; }

interval operator*(const double scale, const interval &ival) { return interval(scale * ival.min, scale * ival.max); }

interval operator*(const interval &ival, const double scale) { return scale * ival; }
