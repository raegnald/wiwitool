// Value-type 4-channel (RGBA) image container supporting file
// loading, manipulation, and saving.

#pragma once

#include <cstddef>
#include <filesystem>
#include <memory>

class Image_data {
public:
  struct Pixel { unsigned char r, g, b, a; };

  Image_data(void) = delete;
  Image_data(size_t w, size_t h);
  Image_data(std::filesystem::path filepath);

  ~Image_data(void) = default;

  // Copy
  Image_data(const Image_data &);
  Image_data &operator=(const Image_data &);

  // Move
  Image_data(Image_data &&other) noexcept = default;
  Image_data &operator=(Image_data &&other) noexcept = default;

  // Image attributes
  inline std::filesystem::path path(void) const noexcept { return path_; };
  inline size_t width(void) const noexcept { return width_; }
  inline size_t height(void) const noexcept { return height_; }
  inline size_t channels(void) const noexcept { return channels_; }

  // Access pixel data
  Pixel &at(size_t x, size_t y);
  const Pixel &at(size_t x, size_t y) const;

  inline Pixel *data(void) noexcept { return data_.get(); }
  inline const Pixel *data(void) const noexcept { return data_.get(); }

  // Image manipulation

  Image_data scale(float factor) const;
  Image_data scale(size_t target_w, size_t target_h) const;

  Image_data crop(size_t x, size_t y, size_t width, size_t height) const;

  Image_data rotate_clockwise(void) const;
  Image_data rotate_anticlockwise(void) const;

  Image_data flip_vertically(void) const;
  Image_data flip_horizontally(void) const;

  // Export

  // Dispatch between multiple image formats
  void save_as(std::filesystem::path name) const;

  void save_png(std::filesystem::path name) const;
  void save_jpg(std::filesystem::path name,
                size_t quality_percentage = 100) const;

private:
  static constexpr size_t channels_{4};
  size_t width_{0}, height_{0};

  std::filesystem::path path_;

  using Stb_image = std::unique_ptr<Pixel, void (*)(void *)>;
  Stb_image data_{nullptr, [](void *) {}};
};
