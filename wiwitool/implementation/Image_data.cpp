#include "Paintings_pack/Image_data.hpp"

#include <string>
#include <cstdlib>
#include <filesystem>
#include <print>
#include <stdexcept>

#include "stb_image.h"
#include "stb_image_write.h"

Image_data::Image_data(std::filesystem::path filepath) : path(filepath) {
  std::println("Loading image data from path {}", filepath.string());

  data = stbi_load(path.string().c_str(), &width, &height, NULL, 4);
  channels = 4;

  if (not data)
    throw std::runtime_error("Failed to load image: " + path.string());

  is_stbi_allocated = true;
}

Image_data::Image_data(int w, int h, int ch)
    : width{w}, height{h}, channels{ch} {
  const size_t size = width * height * channels;
  data = malloc(size);
  if (not data) throw std::runtime_error("Allocation failed");
}

Image_data::~Image_data(void) {
  if (data) {
    if (is_stbi_allocated)
      stbi_image_free(data);
    else
      free(data);
  }
}

Image_data::Image_data(const Image_data &other)
    : path{other.path}, width{other.width}, height{other.height},
      channels{other.channels}, is_stbi_allocated{false} {

  const auto image_data_size =
      width * height * channels * sizeof(unsigned char);

  data = malloc(image_data_size);
  if (not data) throw std::runtime_error("Image_data: allocation failed during copy");

  memcpy(this->data, other.data, image_data_size);
}

Image_data::Image_data(Image_data &&other) noexcept
    : path{std::move(other.path)}, width{other.width}, height{other.height},
      channels{other.channels}, data{other.data},
      is_stbi_allocated{other.is_stbi_allocated} {
  other.data = nullptr;
  other.width = other.height = other.channels = 0;
  other.is_stbi_allocated = false;
}

Image_data &Image_data::operator=(Image_data &&other) noexcept {
  if (this != &other) {
    if (data) stbi_image_free(data);

    path = std::move(other.path);
    width = other.width;
    height = other.height;
    channels = other.channels;
    data = other.data;
    is_stbi_allocated = other.is_stbi_allocated;

    other.data = nullptr; // steal ownership
    other.width = other.height = other.channels = 0;
    other.path = "";
  }
  return *this;
}

Pixel &Image_data::at(std::size_t x, std::size_t y) const {
    if (x >= width or y >= height)
      throw std::invalid_argument(std::format(
          "Image_data::get: accessing ({}, {}) but image is of size ({}, {})",
          x, y, width, height));

    return static_cast<Pixel *>(data)[y * width + x];
  }

void Image_data::save_as(std::filesystem::path name) {
  if (name.extension() == ".png") {
    stbi_write_png(name.string().c_str(), width, height, 4, data, width * 4);
    return;
  }

  if (name.extension() == ".jpg") {
    stbi_write_jpg(name.string().c_str(), width, height, 4, data, 100);
    return;
  }

  std::invalid_argument(
      "Image_data::save_as: invalid image extension (use .jpg or .png)");
}
