#pragma once

#include <algorithm>
#include <cstddef>
#include <ctime>
#include <format>
#include <random>
#include <string>

inline std::string nospaces(std::string s) {
  const auto upperchar = [](unsigned char c) {
    if (c == ' ') return '_';
    return (char) c;
  };
  std::transform(s.begin(), s.end(), s.begin(), upperchar);
  return s;
}

inline std::string toupper(std::string s) {
  const auto upperchar = [](unsigned char c) { return std::toupper(c); };
  std::transform(s.begin(), s.end(), s.begin(), upperchar);
  return s;
}

inline std::string tolower(std::string s) {
  const auto lowerchar = [](unsigned char c) { return std::tolower(c); };
  std::transform(s.begin(), s.end(), s.begin(), lowerchar);
  return s;
}

inline std::string timestamped(std::string s) {
  const time_t timestamp = time(nullptr);
  return std::format("{}_{}", s, timestamp);
}

inline std::string generate_random_alphabetic_string(size_t chars) {
  std::string s(chars, 'a');

  std::random_device rd;
  std::default_random_engine rng{rd()};
  std::uniform_int_distribution<int> random{0, 'z' - 'a'};

  for (size_t i = 0; i < chars; i++)
    s[i] = 'a' + static_cast<char>(random(rng));

  return s;
}
