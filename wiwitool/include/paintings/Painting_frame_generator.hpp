// A frame generator

#pragma once

#include "Image_data.hpp"
#include "paintings/Painting_ratio.hpp"
#include <cstdint>
#include <variant>

struct Minecraft_default_frame_generator {
  Image_data get(Painting_ratio) const;
};

class Procedural_frame_generator {
public:
  Procedural_frame_generator(void);

  void set_seed(std::uint64_t s) { seed = s; }
  std::uint64_t get_seed(void) const { return seed; }

  void set_tint(Image_data::Pixel t) { tint = t; }
  Image_data::Pixel get_tint(void) const { return tint; }

  Image_data get(Painting_ratio) const;

  // from_json, to_json...

private:
  std::uint64_t seed{0};
  Image_data::Pixel tint{99, 72, 39, 255};
};

using Painting_frame_generator =
    std::variant<Minecraft_default_frame_generator, Procedural_frame_generator>;

Image_data get_frame(Painting_frame_generator, Painting_ratio);

// Painting_frame_generator from_json(), to_json(Painting_frame_generator)...
