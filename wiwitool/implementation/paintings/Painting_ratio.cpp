
#include "paintings/Painting_ratio.hpp"
#include "util/wiwidebug.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <format>
#include <print>
#include <stdexcept>
#include <utility>

#include "nlohmann/json.hpp"

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

std::string string_of_ratio(Painting_ratio r) {
  if (r == Painting_ratio::Nearest) return "nearest";

  const auto digit = [](int i) {
    switch (i) {
    case 1: return "ONE";
    case 2: return "TWO";
    case 3: return "THREE";
    case 4: return "FOUR";
    default: throw std::invalid_argument("string_of_ratio: digit");
    }
  };

  // ratio_sizes extracts the high/low bytes from the enum value
  auto [w, h] = ratio_sizes(r);
  return std::format("{}_{}", digit(w), digit(h));
}


Painting_ratio ratio_of_string(std::string s) {
  if (s == "nearest" || s == "Nearest") return Painting_ratio::Nearest;

  for (auto r : all_ratios())
    if (string_of_ratio(r) == s) return r;

  wiwidebug std::println(
      "Warning: Invalid ratio string '{}', defaulting to Nearest", s);

  return Painting_ratio::Nearest;
}

Painting_ratio opposite_ratio(Painting_ratio r) noexcept {
  const auto [w, h] = ratio_sizes(r);
  return static_cast<Painting_ratio>((h << 8) | w);
}

void to_json(nlohmann::json &j, const Painting_ratio &r) {
    j = string_of_ratio(r);
}

void from_json(const nlohmann::json &j, Painting_ratio &r) {
  r = ratio_of_string(j.get<std::string>());
}
