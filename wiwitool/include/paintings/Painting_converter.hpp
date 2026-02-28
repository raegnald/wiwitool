#pragma once

#include "Image_data.hpp"
#include "paintings/Painting_frame_generator.hpp"
#include "paintings/Painting_ratio.hpp"

struct Painting_converter {
  const Image_data &image;

  Image_data convert(const Painting_frame_generator &frame_generator,
                     Painting_ratio ratio = Nearest);

  Image_data miniatureise(void);
};
