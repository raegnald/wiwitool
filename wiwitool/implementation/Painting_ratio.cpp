
#include "Paintings_pack/Painting_ratio.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <format>
#include <utility>

std::array<Painting_ratio, 12> all_ratios(void) noexcept {
  return {ONE_ONE,   ONE_TWO,     TWO_ONE,    TWO_TWO,  TWO_THREE,  TWO_FOUR,
          THREE_TWO, THREE_THREE, THREE_FOUR, FOUR_TWO, FOUR_THREE, FOUR_FOUR};
}

std::pair<int, int> ratio_sizes(Painting_ratio r) noexcept {
  return {(static_cast<int>(r) >> 8) & 0xFF, static_cast<int>(r) & 0xFF};
}

double ratio_value(Painting_ratio r) noexcept {
  const auto [w, h] = ratio_sizes(r);
  return static_cast<double>(w) / h;
}

Painting_ratio nearest_ratio(int width, int height) noexcept {
  const auto ratios = all_ratios();
  const double target_ratio = static_cast<double>(width) / height;

  const auto it = std::ranges::min_element(
      ratios, std::ranges::less{}, [&](Painting_ratio r) {
        return std::abs(target_ratio - ratio_value(r));
      });

  return *it;
}

Image_data load_frame(Painting_ratio ratio) {
  if (ratio == Nearest)
    throw std::invalid_argument("load_frame: Nearest ratio is not valid");

  if (ratio == ICON_RATIO)
    return Image_data{frames_directory / "painting.png"};

  const auto [width, height] = ratio_sizes(ratio);
  const auto frame_file = std::format("{}{}.png", width, height);

  return Image_data{frames_directory / frame_file};
}
