#pragma once

#include <string>
#include <utility>
#include <vector>

#include "nlohmann/json.hpp"

class Painting_ratio {
public:
  static const Painting_ratio nearest;

  // static const std::vector<Painting_ratio> preferred;
  static const std::vector<Painting_ratio> available;

  // Constructors

  /// Use default painting ratio of nearest
  constexpr Painting_ratio(void) noexcept = default;
  constexpr Painting_ratio(size_t l) noexcept : ratio{l, l} { ensure_valid_ratio(); }
  constexpr Painting_ratio(size_t w, size_t h) noexcept : ratio{w, h} { ensure_valid_ratio(); }

  template <typename T>
  Painting_ratio(std::pair<T, T> ratio) noexcept : ratio{ratio} { ensure_valid_ratio(); }

  static Painting_ratio nearest_to(int width, int height) noexcept;

  template <typename T>
  static Painting_ratio nearest_to(std::pair<T, T> size) noexcept {
    return nearest_to(static_cast<size_t>(size.first),
                      static_cast<size_t>(size.second));
  }

  static Painting_ratio from_string(std::string s);

  // Predicates

  bool using_nearest_ratio(void) const noexcept;

  // Get properties

  std::pair<size_t, size_t> sizes(void) const noexcept;
  double value(void) const noexcept;

  // Transformations

  Painting_ratio opposite(void) const noexcept;

  // Export

  std::string to_string(void) const noexcept;

private:
  using Ratio = std::pair<size_t, size_t>;
  static constexpr Ratio nearest_ratio{0, 0};
  static constexpr size_t MAX_SIDE_SIZE = 16; // in units of Minecraft blocks

  Ratio ratio{nearest_ratio};

  // If a ratio is invalid, it will be forced to the be the nearest ratio.
  constexpr void ensure_valid_ratio(void) noexcept {
    if (ratio.first > MAX_SIDE_SIZE or ratio.second > MAX_SIDE_SIZE)
      ratio = nearest_ratio;
  }
};

void to_json(nlohmann::json &j, const Painting_ratio &r);
void from_json(const nlohmann::json &j, Painting_ratio &r);
