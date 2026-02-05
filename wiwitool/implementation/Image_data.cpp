#include "Paintings_pack/Image_data.hpp"

#include "util/strutil.hpp"
#include "util/wiwidebug.hpp"

#include <string>
#include <cstdlib>
#include <filesystem>
#include <print>
#include <stdexcept>
#include <vector>

#include "stb_image.h"
#include "stb_image_write.h"

// Image_data::Pixel* == void* returned by stb_image
static_assert(std::is_trivially_copyable_v<Image_data::Pixel>);
static_assert(sizeof(Image_data::Pixel) == 4);

Image_data::Image_data(std::filesystem::path filepath) : path_{filepath} {
  wiwidebug std::println("Loading image data from path {}", filepath.string());

  int w, h;
  const auto imgdata = reinterpret_cast<Pixel *>(
      stbi_load(filepath.c_str(), &w, &h, nullptr, channels_));

  if (not imgdata)
    throw std::runtime_error("Image_data: failed to load " + path_.string());

  width_ = w;
  height_ = h;

  data_ = Stb_image{reinterpret_cast<Pixel *>(imgdata), stbi_image_free};
}


Image_data::Image_data(std::vector<uint8_t> data) {
  wiwidebug std::println("Loading image data from data");

  int w, h;
  const auto imgdata = reinterpret_cast<Pixel *>(stbi_load_from_memory(
      data.data(), data.size(), &w, &h, nullptr, channels_));

  if (not imgdata)
    throw std::runtime_error("Image_data: failed to load from data");

  width_ = w;
  height_ = h;

  data_ = Stb_image{reinterpret_cast<Pixel *>(imgdata), stbi_image_free};
}


Image_data::Image_data(size_t w, size_t h) : width_{w}, height_{h} {
  const size_t size = width_ * height_ * sizeof(Pixel);
  data_ = Stb_image{reinterpret_cast<Pixel *>(malloc(size)), free};
  if (not data_) throw std::runtime_error("Allocation failed");
}

// Copy ctor
Image_data::Image_data(const Image_data &other)
    : width_{other.width_}, height_{other.height_}, path_{other.path_} {

  const auto size = width_ * height_ * sizeof(Pixel);

  data_ = Stb_image{reinterpret_cast<Pixel *>(malloc(size)), free};
  if (not data_) throw std::runtime_error("Image_data: allocation failed during copy");

  memcpy(this->data_.get(), other.data_.get(), size);
}

// Copy assignment
Image_data &Image_data::operator=(const Image_data &other) {
  if (this != &other) {
    if (data_) data_.reset(nullptr);

    path_ = other.path_;
    width_ = other.width_;
    height_ = other.height_;

    const auto size = width_ * height_ * sizeof(Pixel);

    data_ = Stb_image{reinterpret_cast<Pixel *>(malloc(size)), free};
    if (not data_) throw std::runtime_error("Image_data: allocation failed during copy");

    memcpy(this->data_.get(), other.data_.get(), size);
  }

  return *this;
}


Image_data::Pixel &Image_data::at(size_t x, size_t y) {
  if (x >= width_ or y >= height_)
    throw std::invalid_argument(std::format(
        "Image_data::get: accessing ({}, {}) but image is of size ({}, {})", x,
        y, width_, height_));

  return data_.get()[y * width_ + x];
}


const Image_data::Pixel &Image_data::at(size_t x, size_t y) const {
  if (x >= width_ or y >= height_)
    throw std::invalid_argument(std::format(
        "Image_data::get: accessing ({}, {}) but image is of size ({}, {})", x,
        y, width_, height_));

  return data_.get()[y * width_ + x];
}

Image_data Image_data::scale(float factor) const {
  return scale(static_cast<size_t>(width() * factor),
               static_cast<size_t>(height() * factor));
}

Image_data Image_data::scale(size_t target_w, size_t target_h) const {
  const auto step_x = static_cast<float>(width()) / target_w;
  const auto step_y = static_cast<float>(height()) / target_h;

  Image_data out{target_w, target_h};

  const auto sample = [&](int x, int y) {
    int src_y = y * step_y, src_x = x * step_x;

    if (src_y >= height()) src_y = height() - 1;
    if (src_x >= width()) src_x = width() - 1;

    return at(src_x, src_y);
  };

  for (size_t y = 0; y < target_h; ++y)
    for (size_t x = 0; x < target_w; ++x)
      out.at(x, y) = sample(x, y);

  return out;
}

Image_data Image_data::crop(size_t x0, size_t y0, size_t w, size_t h) const {
  Image_data out{w, h};

  for (size_t y = 0; y < h; ++y)
    for (size_t x = 0; x < w; ++x)
      out.at(x, y) = this->at(x0 + x, y0 + y);

  return out;
}

Image_data Image_data::rotate_clockwise(void) const {
  Image_data out{height(), width()};

  for (size_t y = 0; y < this->height(); ++y)
    for (size_t x = 0; x < this->width(); ++x)
      out.at(height() - y - 1, x) = this->at(x, y);

  return out;
}

Image_data Image_data::rotate_anticlockwise(void) const {
  Image_data out{height(), width()};

  for (size_t y = 0; y < this->height(); ++y)
    for (size_t x = 0; x < this->width(); ++x)
      out.at(y, width() - x - 1) = this->at(x, y);

  return out;
}

Image_data Image_data::flip_vertically(void) const {
  Image_data out{width(), height()};

  for (size_t y = 0; y < height(); ++y)
    for (size_t x = 0; x < width(); ++x)
      out.at(x, height() - y - 1) = this->at(x, y);

  return out;
}

Image_data Image_data::flip_horizontally(void) const {
  Image_data out{width(), height()};

  for (size_t y = 0; y < height(); ++y)
    for (size_t x = 0; x < width(); ++x)
      out.at(width() - x - 1, y) = this->at(x, y);

  return out;
}

void Image_data::save_png(std::filesystem::path name) const {
  const auto stride = width_ * sizeof(Pixel);
  stbi_write_png(name.c_str(), width_, height_, channels_, data_.get(), stride);
}

void Image_data::save_jpg(std::filesystem::path name,
                          size_t quality_percentage) const {
  stbi_write_jpg(name.c_str(), width_, height_, 4, data_.get(),
                 quality_percentage);
}

void Image_data::save_as(std::filesystem::path name) const {
  const auto ext = tolower(name.extension());

  if (ext == ".png") {
    save_png(name);
    return;
  }

  if (ext == ".jpg" or ext == ".jpeg") {
    save_jpg(name);
    return;
  }

  throw std::invalid_argument(
      "Image_data::save_as: invalid image extension (use .jpg/.jpeg or .png)");
}

#ifdef EMSCRIPTEN
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <string>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(image_data) {

  // 2. Bind the inner Pixel struct as a value object (converts to/from JS Object {r,g,b,a})
  value_object<Image_data::Pixel>("Pixel")
    .field("r", &Image_data::Pixel::r)
    .field("g", &Image_data::Pixel::g)
    .field("b", &Image_data::Pixel::b)
    .field("a", &Image_data::Pixel::a);

  // 3. Bind Image_data
  class_<Image_data>("Image_data")
    // Constructors
    .constructor<>()
    .constructor<size_t, size_t>()
    // Wrapper constructor for std::filesystem::path -> std::string
    .constructor(optional_override([](std::string path_str) {
      return std::make_unique<Image_data>(std::filesystem::path(path_str));
    }))

    // Attributes
    .property("width", &Image_data::width)
    .property("height", &Image_data::height)
    .property("channels", &Image_data::channels)
    .function("empty", &Image_data::empty)

    // Path handling (wrapper to return string instead of filesystem::path)
    .function("path", optional_override([](const Image_data& self) {
      return self.path().string();
    }))

    // Pixel Access
    // Helper: Returns a typed view into WASM memory.
    // This is ZERO-COPY and allows direct use with HTML Canvas APIs.
    .function("getPixels", optional_override([](const Image_data& self) {
      const size_t size = self.width() * self.height() * 4; // 4 channels
      const uint8_t* ptr = reinterpret_cast<const uint8_t*>(self.data());

      // Return a Uint8ClampedArray (standard for Canvas ImageData)
      return val(typed_memory_view(size, ptr));
    }))
    // Single pixel access
    .function("at", select_overload<const Image_data::Pixel&(size_t, size_t) const>(&Image_data::at))

    // Manipulation (These return new Image_data objects by value)
    .function("scaleFactor", select_overload<Image_data(float) const>(&Image_data::scale))
    .function("scaleDims", select_overload<Image_data(size_t, size_t) const>(&Image_data::scale))
    .function("crop", &Image_data::crop)
    .function("rotateClockwise", &Image_data::rotate_clockwise)
    .function("rotateAnticlockwise", &Image_data::rotate_anticlockwise)
    .function("flipVertically", &Image_data::flip_vertically)
    .function("flipHorizontally", &Image_data::flip_horizontally)

    // Export (Wrappers for std::string)
    .function("saveAs", optional_override([](const Image_data& self, std::string path) {
      self.save_as(std::filesystem::path(path));
    }))
    .function("savePng", optional_override([](const Image_data& self, std::string path) {
      self.save_png(std::filesystem::path(path));
    }))
    .function("saveJpg", optional_override([](const Image_data& self, std::string path, size_t quality) {
      self.save_jpg(std::filesystem::path(path), quality);
    }));

}
#endif
