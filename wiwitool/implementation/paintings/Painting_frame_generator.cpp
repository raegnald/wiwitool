#include "paintings/Painting_frame_generator.hpp"

#include "Image_data.hpp"
#include "paintings/Painting_ratio.hpp"
#include "util/wiwidebug.hpp"

#include <filesystem>
#include <format>
#include <utility>
#include <variant>

Image_data get_frame(Painting_frame_generator frame_generator,
                     Painting_ratio ratio) {
  return std::visit([ratio](const auto &gen) { return gen.get(ratio); },
                    frame_generator);
}

Image_data Minecraft_default_frame_generator::get(Painting_ratio ratio) const {
  if (ratio == Nearest)
    throw std::invalid_argument("Nearest ratio is not valid");

  if (ratio == ICON_RATIO)
    return Image_data{frames_directory / "painting.png"};

  const auto [width, height] = ratio_sizes(ratio);
  const auto frame_file = std::format("{}{}.png", width, height);

  return Image_data{frames_directory / frame_file};
}

Procedural_frame_generator::Procedural_frame_generator(void) {}

Image_data Procedural_frame_generator::get(Painting_ratio ratio) const {
  // Block size
  const auto [bw, bh] = ratio_sizes(ratio);
  // Pixel size
  const size_t pw = static_cast<size_t>(16 * bw),
               ph = static_cast<size_t>(16 * bh);

  Image_data im{pw, ph};

  srand(seed);

  for (size_t y = 0; y < ph; y++) {
    for (size_t x = 0; x < pw; x++) {
      if (y != 0 and y != ph - 1 and x != 0)
        x = pw - 1;

      auto colour = tint;

      {
        colour.r += rand() % 50;
        colour.r = (colour.r > 255) ? 255 : colour.r;

        colour.g += rand() % 50;
        colour.g = (colour.g > 255) ? 255 : colour.g;

        colour.b += rand() % 50;
        colour.b = (colour.b > 255) ? 255 : colour.b;
      }

      im.at(x, y) = colour;
    }
  }

  return im;
};


#ifdef EMSCRIPTEN
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(painting_frames) {
  class_<Procedural_frame_generator>("ProceduralFrameGenerator")
      .property("seed", &Procedural_frame_generator::get_seed,
                &Procedural_frame_generator::set_seed)
      .property("tint", &Procedural_frame_generator::get_tint,
                &Procedural_frame_generator::set_tint)

      .property(
          "tintHex",
          +[](const Procedural_frame_generator &self) -> std::string {
            auto t = self.get_tint();
            return std::format("#{:02x}{:02x}{:02x}", t.r, t.g, t.b);
          },
          +[](Procedural_frame_generator &self, std::string hex) {
            if (hex.size() == 7 && hex[0] == '#') {
              try {
                unsigned char r = std::stoul(hex.substr(1, 2), nullptr, 16);
                unsigned char g = std::stoul(hex.substr(3, 2), nullptr, 16);
                unsigned char b = std::stoul(hex.substr(5, 2), nullptr, 16);
                self.set_tint({r, g, b, 255});
              } catch (...) {}
            }
          });
}
#endif
