#pragma once

#include <cstddef>
#include <filesystem>

struct Pixel { unsigned char r, g, b, a; };

struct Image_data {
  std::filesystem::path path;
  int width{0}, height{0}, channels{0};
  void *data{nullptr};
  bool is_stbi_allocated{false};

  Image_data(void) = delete;

  Image_data(int w, int h, int ch = 4);

  explicit Image_data(std::filesystem::path filepath);

  ~Image_data(void);

  // No copy -- YES copy
  Image_data(const Image_data &);
  // Image_data(const Image_data &) = delete;
  // Image_data& operator=(const Image_data &) = delete;

  // Move-only type -- SHUT UP
  Image_data(Image_data&& other) noexcept;
  Image_data& operator=(Image_data&& other) noexcept;

  Pixel &at(std::size_t x, std::size_t y) const;

  void save_as(std::filesystem::path name);
};
