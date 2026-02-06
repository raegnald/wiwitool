#pragma once

#include <filesystem>
#include <array>
#include <utility>

#include "Image_data.hpp"

const inline std::filesystem::path frames_directory{"frames/"};

enum Painting_ratio {
  Nearest,               // nearest painting ratio should be used

  ONE_ONE     = 0x01'01, // 1:1
  ONE_TWO     = 0x01'02, // 1:2
  TWO_ONE     = 0x02'01, // 2:1
  TWO_TWO     = 0x02'02, // 2:2
  TWO_THREE   = 0x02'03, // 2:3
  TWO_FOUR    = 0x02'04, // 2:4
  THREE_TWO   = 0x03'02, // 3:2
  THREE_THREE = 0x03'03, // 3:3
  THREE_FOUR  = 0x03'04, // 3:4
  FOUR_TWO    = 0x04'02, // 4:2
  FOUR_THREE  = 0x04'03, // 4:3
  FOUR_FOUR   = 0x04'04, // 4:4

  ICON_RATIO  = 0x0B'08, // 11:8
};

std::array<Painting_ratio, 12> all_ratios(void) noexcept;

std::pair<int, int> ratio_sizes(Painting_ratio r) noexcept;
double ratio_value(Painting_ratio r) noexcept;

Painting_ratio nearest_ratio(int width, int height) noexcept;

Image_data load_frame(Painting_ratio ratio);
