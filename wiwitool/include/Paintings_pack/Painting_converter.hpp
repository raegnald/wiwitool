#pragma once

#include "Image_data.hpp"
#include "Paintings_pack/Painting_ratio.hpp"

struct Painting_converter {
  const Image_data &image;

  std::pair<unsigned, unsigned> convert(std::filesystem::path output,
                                        Painting_ratio ratio = Nearest);

  void miniatureise(std::filesystem::path output);
};
