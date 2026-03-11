// Colours

#pragma once

struct rgba;
struct hsv;

struct rgba {
  unsigned char r, g, b, a;

  rgba(void) : r{0}, g{0}, b{0}, a{255} {}

  rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
      : r{r}, g{g}, b{b}, a{a} {}

  rgba(const rgba &) = default;
  rgba(rgba &&) = default;

  rgba &operator=(const rgba &) = default;
  rgba &operator=(rgba &&) = default;

  rgba(hsv);

  static rgba blend(rgba, rgba, float t = 0.5);
};

struct hsv {
  float h;          // hue        [0, 360]
  float s;          // saturation [0, 1]
  float v;          // value      [0, 1]
  unsigned char a;  // alpha      [0, 255]

  hsv(void) : h{0}, s{0}, v{0}, a{255} {}

  hsv(float h, float s, float v, unsigned char a)
      : h{h}, s{s}, v{v}, a{a} {}

  hsv(const hsv &) = default;
  hsv(hsv &&) = default;

  hsv &operator=(const hsv &) = default;
  hsv &operator=(hsv &&) = default;

  hsv(rgba);
};
