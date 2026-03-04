#include "colour.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>

rgba::rgba(hsv hsv) {
  float c = hsv.v * hsv.s;
  float x = c * (1.0f - std::abs(std::fmod(hsv.h / 60.0f, 2.0f) - 1.0f));
  float m = hsv.v - c;

  float fr = 0.0f, fg = 0.0f, fb = 0.0f;

  // every hue sector is 60 deg
  if      (hsv.h >= 0.0f && hsv.h < 60.0f)   { fr = c; fg = x; fb = 0; }
  else if (hsv.h >= 60.0f && hsv.h < 120.0f) { fr = x; fg = c; fb = 0; }
  else if (hsv.h >= 120.0f && hsv.h < 180.0f){ fr = 0; fg = c; fb = x; }
  else if (hsv.h >= 180.0f && hsv.h < 240.0f){ fr = 0; fg = x; fb = c; }
  else if (hsv.h >= 240.0f && hsv.h < 300.0f){ fr = x; fg = 0; fb = c; }
  else                                       { fr = c; fg = 0; fb = x; }

  // add baseline modifier m and scale to 255
  r = static_cast<unsigned char>(std::round((fr + m) * 255.0f));
  g = static_cast<unsigned char>(std::round((fg + m) * 255.0f));
  b = static_cast<unsigned char>(std::round((fb + m) * 255.0f));
  a = hsv.a;
}

hsv::hsv(rgba rgba) {
  float fr = rgba.r / 255.0f;
  float fg = rgba.g / 255.0f;
  float fb = rgba.b / 255.0f;

  float cmax = std::max({fr, fg, fb});
  float cmin = std::min({fr, fg, fb});
  float delta = cmax - cmin;

  v = cmax; // value is simply the max component

  // Calculate saturation and hue
  if (delta > 0.0f) {
    s = delta / cmax;

    if (cmax == fr) {
      h = 60.0f * std::fmod(((fg - fb) / delta) + 6.0f, 6.0f);
    } else if (cmax == fg) {
      h = 60.0f * (((fb - fr) / delta) + 2.0f);
    } else if (cmax == fb) {
      h = 60.0f * (((fr - fg) / delta) + 4.0f);
    }
  }

  a = rgba.a;
}
