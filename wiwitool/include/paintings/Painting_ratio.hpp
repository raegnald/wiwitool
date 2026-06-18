#pragma once

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#include "nlohmann/json.hpp"

class Painting_ratio {
public:
  static const Painting_ratio nearest;

  // static const std::vector<Painting_ratio> preferred;
  static const std::vector<Painting_ratio> available;

  /// Use default painting ratio of nearest
  constexpr Painting_ratio(void) noexcept = default;
  constexpr Painting_ratio(size_t l) noexcept : ratio{l, l} {}
  constexpr Painting_ratio(size_t w, size_t h) noexcept : ratio{w, h} {}

  template <typename T>
  Painting_ratio(std::pair<T, T> ratio) noexcept : ratio{ratio} {}

  static Painting_ratio nearest_to(int width, int height) noexcept;

  template <typename T>
  static Painting_ratio nearest_to(std::pair<T, T> size) noexcept {
    return nearest_to(size.first, size.second);
  }

  static Painting_ratio from_string(std::string s);

  bool using_nearest_ratio(void) const noexcept;

  std::pair<size_t, size_t> sizes(void) const noexcept;

  double value(void) const noexcept;

  Painting_ratio opposite(void) const noexcept;

  std::string to_string(void) const noexcept;

private:
  using Ratio = std::pair<size_t, size_t>;
  static constexpr Ratio nearest_ratio{0, 0};

  Ratio ratio{nearest_ratio};
};

void to_json(nlohmann::json &j, const Painting_ratio &r);
void from_json(const nlohmann::json &j, Painting_ratio &r);

// enum Painting_ratio {
//   Nearest,               // nearest painting ratio should be used

//   ONE_ONE     = 0x01'01, // 1:1
//   ONE_TWO     = 0x01'02, // 1:2
//   ONE_THREE   = 0x01'03, // 1:3
//   ONE_FOUR    = 0x01'04, // 1:4

//   TWO_ONE     = 0x02'01, // 2:1
//   TWO_TWO     = 0x02'02, // 2:2
//   TWO_THREE   = 0x02'03, // 2:3
//   TWO_FOUR    = 0x02'04, // 2:4

//   THREE_ONE   = 0x03'01, // 3:1
//   THREE_TWO   = 0x03'02, // 3:2
//   THREE_THREE = 0x03'03, // 3:3
//   THREE_FOUR  = 0x03'04, // 3:4

//   FOUR_ONE    = 0x04'01, // 4:1
//   FOUR_TWO    = 0x04'02, // 4:2
//   FOUR_THREE  = 0x04'03, // 4:3
//   FOUR_FOUR   = 0x04'04, // 4:4

//   ICON_RATIO  = 0x0B'08, // 11:8
// };
