
#include "paintings/Painting_ratio.hpp"
#include "util/wiwidebug.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <format>
#include <print>
#include <utility>

#include "nlohmann/json.hpp"

const Painting_ratio Painting_ratio::nearest{Painting_ratio::nearest_ratio};

const std::vector<Painting_ratio> Painting_ratio::available {
  {1, 1}, {1, 2}, {1, 3}, {1, 4},
  {2, 1}, {2, 2}, {2, 3}, {2, 4},
  {3, 1}, {3, 2}, {3, 3}, {3, 4},
  {4, 1}, {4, 2}, {4, 3}, {4, 4},
};

Painting_ratio Painting_ratio::nearest_to(int width, int height) noexcept {
  const double target_ratio = static_cast<double>(width) / height;

  const auto it = std::ranges::min_element(
      available, std::ranges::less{},
      [&](Painting_ratio r) { return std::abs(target_ratio - r.value()); });

  return *it;
}

bool Painting_ratio::using_nearest_ratio(void) const noexcept {
  return ratio == nearest_ratio;
}

std::pair<size_t, size_t> Painting_ratio::sizes(void) const noexcept {
  return ratio;
}

double Painting_ratio::value(void) const noexcept {
  const auto [w, h] = sizes();
  return static_cast<double>(w) / h;
}

std::string Painting_ratio::to_string(void) const noexcept {
  if (using_nearest_ratio())
    return "ratio_nearest";

  auto [w, h] = sizes();
  return std::format("ratio_{}x{}", w, h);
}

Painting_ratio parse_ratio_old_format(std::string s) {
  if (s == "nearest")
    return Painting_ratio::nearest;

  // Old format goes from sizes 1x1 up to 4x4, so we will only consider
  // digits ONE, TWO, THREE, and FOUR, separated by an underscore.
  //
  // For example, the old format is encodes the ratio 3x4 as "THREE_FOUR",
  // and 2x1 as "TWO_ONE".

  constexpr auto str_value = [](const std::string &str) -> size_t {
    if (str == "ONE")   return 1;
    if (str == "TWO")   return 2;
    if (str == "THREE") return 3;
    if (str == "FOUR")  return 4;

    return 0;
  };

  const auto sep = s.find('_');

  if (sep == std::string::npos)
    // no underscore found: not old format
    return Painting_ratio::nearest;

  const auto first = s.substr(0, sep);
  const auto second = s.substr(sep + 1, s.size() - sep - 1);

  const auto w = str_value(first);
  const auto h = str_value(second);

  if (w == 0 or h == 0)
    return Painting_ratio::nearest;

  return {w, h};
}

Painting_ratio Painting_ratio::from_string(std::string s) {
  if (s == "ratio_nearest")
    return nearest_ratio;

  // FOTUT: change this loop that goes through all 256 possibilities
  // to a smarter loop that goes first through the preferred ratios
  for (auto r : available)
    if (r.to_string() == s)
      return r;

  wiwidebug std::println(
      "Warning: Invalid ratio string '{}', trying old format", s);

  return parse_ratio_old_format(s);
}

Painting_ratio Painting_ratio::opposite(void) const noexcept {
  const auto [w, h] = sizes();
  return {h, w};
}

void to_json(nlohmann::json &j, const Painting_ratio &r) {
  j = r.to_string();
}

void from_json(const nlohmann::json &j, Painting_ratio &r) {
  r = Painting_ratio::from_string(j.get<std::string>());
}
