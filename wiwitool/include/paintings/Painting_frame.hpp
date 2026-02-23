// Procedural frame generator for Minecraft paintings.

#pragma once

#include "Image_data.hpp"

#include <cstddef>

class Painting_frame {
public:
  static constexpr inline auto default_brown_frame_colour =
      Image_data::Pixel{99, 72, 39, 255};

  Painting_frame(std::size_t block_width, std::size_t block_height,
                 Image_data::Pixel tint = default_brown_frame_colour);

  const Image_data &get(void);

private:
  static constexpr std::size_t pixels_per_block = 16;

  std::size_t block_width_, block_height_;
  Image_data::Pixel tint_; // {99, 72, 39, 255} // brown

  Image_data frame_;

  void compute_frame(void);
};
