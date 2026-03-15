#pragma once

#include <algorithm>
#include <ctime>
#include <format>
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
