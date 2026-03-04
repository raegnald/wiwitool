#include "paintings/Painting_frame_generator.hpp"

#include "Image_data.hpp"
#include "colour.hpp"
#include "paintings/Painting_ratio.hpp"

#include "stb_perlin.h"

#include <cstdlib>
#include <filesystem>
#include <format>
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

Image_data Procedural_frame_generator::get(Painting_ratio ratio) const {
  // Block size
  const auto [bw, bh] = ratio_sizes(ratio);
  // Pixel size
  const size_t pw = static_cast<size_t>(16 * bw),
               ph = static_cast<size_t>(16 * bh);

  Image_data im{pw, ph};

  const hsv base{tint};

  for (size_t y = 0; y < ph; y++) {
    for (size_t x = 0; x < pw; x++) {

      if (y != 0 and y != ph - 1 and x != 0)
        x = pw - 1;

      constexpr float A = 0.25;
      constexpr auto power = 4;

      const auto px = (float) (power * x) / pw;
      const auto py = (float) (power * y) / ph;
      const auto pz = 0;

      const auto noise_wrap = std::pow(2, power);

      auto tinted = base;
      const auto delta = A * stb_perlin_noise3_seed(px, py, pz, noise_wrap,
                                                    noise_wrap, 0, seed);

      tinted.v += delta;
      if (tinted.v < 0) tinted.v = 0;
      if (tinted.v > 1) tinted.v = 1;

      im.at(x, y) = tinted;
    }
  }

  return im;
}

// JSON serialisation

void to_json(nlohmann::json &j, const Minecraft_default_frame_generator &g) {
  j = nlohmann::json{{"type", "default"}};
}

void from_json(const nlohmann::json &j, Minecraft_default_frame_generator &g) {
  return;
}

void to_json(nlohmann::json &j, const Procedural_frame_generator &g) {
  auto tint = g.get_tint();

  j = nlohmann::json{{"type", "procedural"},
                     {"seed", g.get_seed()},
                     {"tint", {tint.r, tint.g, tint.b, tint.a}}};
}

void from_json(const nlohmann::json &j, Procedural_frame_generator &g) {
  if (j.contains("seed"))
    g.set_seed(j.at("seed").get<std::uint64_t>());

  if (j.contains("tint")) {
    auto tint_arr = j.at("tint").get<std::array<unsigned char, 4>>();
    g.set_tint({tint_arr[0], tint_arr[1], tint_arr[2], tint_arr[3]});
  }
}


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
