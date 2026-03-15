// A frame generator

#pragma once

#include "Image_data.hpp"
#include "paintings/Painting_ratio.hpp"
#include <cstdint>
#include <variant>

#include "nlohmann/json.hpp"

class Procedural_frame_generator {
public:
  Procedural_frame_generator(void) = default;

  void set_seed(std::uint64_t s) { seed = s; }
  std::uint64_t get_seed(void) const { return seed; }

  void set_tint(Image_data::Pixel t) { tint = t; }
  Image_data::Pixel get_tint(void) const { return tint; }

  Image_data get(Painting_ratio) const;

  friend void to_json(nlohmann::json &j, const Procedural_frame_generator &g);
  friend void from_json(const nlohmann::json &j, Procedural_frame_generator &g);

private:
  std::uint64_t seed{0};
  rgba tint{144, 70, 0, 255};
};

using Painting_frame_generator = std::variant<Procedural_frame_generator>;

Image_data get_frame(Painting_frame_generator, Painting_ratio);
