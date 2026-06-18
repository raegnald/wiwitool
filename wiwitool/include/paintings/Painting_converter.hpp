#pragma once

#include "Image_data.hpp"
#include "paintings/Painting_frame_generator.hpp"
#include "paintings/Painting_ratio.hpp"

struct Painting_converter {
  const Image_data &image;

  Image_data convert(const Painting_frame_generator &frame_generator,
                     size_t pixels_per_block,
                     Painting_ratio ratio = Painting_ratio::nearest,
                     float contrast = 1.0f, int brightness = 0);

  Image_data miniatureise(void);
};
