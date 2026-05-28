#include "Image_data.hpp"

#include "util/strutil.hpp"
#include "util/wiwidebug.hpp"

#include <string>
#include <cstdlib>
#include <filesystem>
#include <print>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include "nlohmann/json.hpp"

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

  if (not imgdata) {
    // throw std::runtime_error("Image_data: failed to load from data");
    wiwidebug std::println(
        "Failed to decode PNG data! Constructing empty image instead");
    width_ = 0;
    height_ = 0;
    data_ = Stb_image{nullptr, [](void *) {}};
    return;
  }

  width_ = w;
  height_ = h;

  data_ = Stb_image{reinterpret_cast<Pixel *>(imgdata), stbi_image_free};
}

Image_data::Image_data(size_t w, size_t h) : width_{w}, height_{h} {
  const size_t size = width_ * height_ * sizeof(Pixel);
  data_ = Stb_image{reinterpret_cast<Pixel *>(malloc(size)), free};
  if (not data_) throw std::runtime_error("Allocation failed");

  for (size_t y = 0; y < height_; y++)
    for (size_t x = 0; x < width_; x++)
      at(x, y) = Pixel{0, 0, 0, 0};
}

// Copy ctor
Image_data::Image_data(const Image_data &other)
    : width_{other.width_}, height_{other.height_}, path_{other.path_} {

  const auto size = width_ * height_ * sizeof(Pixel);

  if (size > 0) {
    data_ = Stb_image{reinterpret_cast<Pixel *>(malloc(size)), free};
    if (not data_) throw std::runtime_error("Image_data: allocation failed during copy");

    memcpy(this->data_.get(), other.data_.get(), size);
  } else {
    data_ = Stb_image{nullptr, [](void *) {}};
  }
}

// Copy assignment
Image_data &Image_data::operator=(const Image_data &other) {
  if (this != &other) {
    if (data_) data_.reset(nullptr);

    path_ = other.path_;
    width_ = other.width_;
    height_ = other.height_;

    const auto size = width_ * height_ * sizeof(Pixel);

    if (size > 0) {
      data_ = Stb_image{reinterpret_cast<Pixel *>(malloc(size)), free};
      if (not data_) throw std::runtime_error("Image_data: allocation failed during copy");

      memcpy(this->data_.get(), other.data_.get(), size);
    }
    else {
      data_ = Stb_image{nullptr, [](void *) {}};
    }
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

template <typename Fun> void Image_data::foreach(Fun f) {
  constexpr bool x_y_pixel = std::is_invocable_v<Fun, size_t, size_t, Pixel &>;
  constexpr bool x_y = std::is_invocable_v<Fun, size_t, size_t>;
  constexpr bool pixel_only = std::is_invocable_v<Fun, Pixel &>;

  for (size_t y = 0; y < height(); ++y) {
    for (size_t x = 0; x < width(); ++x) {
      auto &pixel = at(x, y);

      if constexpr (x_y_pixel)
        f(x, y, pixel);
      else if constexpr (x_y)
        f(x, y);
      else if constexpr (pixel_only)
        f(pixel);
      else
        static_assert(x_y_pixel or x_y or pixel_only,
                      "for_each_pixel(f) requires f(pixel &), f(size_t, "
                      "size_t, pixel &), f(size_t, size_t)");
    }
  }
}

template <typename Fun> void Image_data::foreach(Fun f) const {
  constexpr bool x_y_pixel = std::is_invocable_v<Fun, size_t, size_t, Pixel>;
  constexpr bool x_y = std::is_invocable_v<Fun, size_t, size_t>;
  constexpr bool pixel_only = std::is_invocable_v<Fun, Pixel>;

  for (size_t y = 0; y < height(); ++y) {
    for (size_t x = 0; x < width(); ++x) {
      const auto pixel = at(x, y);

      if constexpr (x_y_pixel)
        f(x, y, pixel);
      else if constexpr (x_y)
        f(x, y);
      else if constexpr (pixel_only)
        f(pixel);
      else
        static_assert(x_y_pixel or x_y or pixel_only,
                      "for_each_pixel(f) requires f(pixel), f(size_t, "
                      "size_t, pixel), f(size_t, size_t)");
    }
  }
}

Image_data Image_data::scale(float factor) const {
  return scale(static_cast<size_t>(width() * factor),
               static_cast<size_t>(height() * factor));
}

Image_data Image_data::scale(size_t target_w, size_t target_h) const {
  const auto step_x = static_cast<float>(width()) / target_w;
  const auto step_y = static_cast<float>(height()) / target_h;

  Image_data out{target_w, target_h};

  const auto sample = [&](size_t x, size_t y, rgba &pixel) {
    int src_y = y * step_y, src_x = x * step_x;

    if (src_y >= height()) src_y = height() - 1;
    if (src_x >= width()) src_x = width() - 1;

    pixel = at(src_x, src_y);
  };

  out.foreach (sample);

  return out;
}

Image_data Image_data::crop(size_t x0, size_t y0, size_t w, size_t h) const {
  Image_data out{w, h};

  out.foreach ([&](size_t x, size_t y, Pixel &pixel) {
    pixel = this->at(x0 + x, y0 + y);
  });

  return out;
}

Image_data Image_data::crop(size_t rw, size_t rh) const {
  const auto target_ratio = static_cast<double>(rw) / rh;
  const auto current_ratio = static_cast<double>(width()) / height();

  // Practically equal, return original
  if (std::abs(current_ratio - target_ratio) < 0.001)
    return Image_data{*this};

  int new_w, new_h, x0, y0;

  // Image too wide, crop sides
  if (current_ratio > target_ratio) {
    new_h = height();
    new_w = static_cast<int>(height() * target_ratio);
    x0 = (width() - new_w) / 2;
    y0 = 0;
  }
  // Image too tall, crop top/bottom
  else {
    new_w = width();
    new_h = static_cast<int>(width() / target_ratio);
    x0 = 0;
    y0 = (height() - new_h) / 2;
  }

  return crop(x0, y0, new_w, new_h);
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

Image_data Image_data::adjust_contrast_brightness(float contrast_factor,
                                                  int brightness_shift) const {
  if (contrast_factor == 1.0f and brightness_shift == 0)
    return Image_data{*this};

  Image_data out{width(), height()};

  const auto adjust_channel = [&](uint8_t value) -> uint8_t {
    const int x = (value - 128) * contrast_factor + 128 + brightness_shift;
    return static_cast<uint8_t>(std::clamp(x, 0, 255));
  };

  foreach ([&](size_t x, size_t y, const Pixel src) {
    out.at(x, y) = Pixel{
      adjust_channel(src.r),
      adjust_channel(src.g),
      adjust_channel(src.b),
      src.a
    };
  });

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

static void stbi_write_to_vector(void *context, void *data, int size) {
  auto *vec = static_cast<std::vector<uint8_t> *>(context);
  auto *ptr = static_cast<const uint8_t *>(data);
  vec->insert(vec->end(), ptr, ptr + size);
}

std::vector<uint8_t> Image_data::encode_png(void) const {
  std::vector<uint8_t> buffer;
  const auto stride = width_ * sizeof(Pixel);

  stbi_write_png_to_func(stbi_write_to_vector, &buffer, width_, height_,
                         channels_, data_.get(), stride);

  return buffer;
}

void to_json(nlohmann::json &j, const Image_data &image) {
  if (image.empty())
    j = nlohmann::json{{"image_data", nullptr}};
  else
    j = nlohmann::json{
        {"image_data", nlohmann::json::binary(image.encode_png())}};
}

void from_json(const nlohmann::json &j, Image_data &image) {
  auto &img = j.at("image_data");

  if (img.is_binary()) {
    image = Image_data{img.get_binary()};
  }
  else if (img.is_array()) {
    std::vector<uint8_t> encoded_data;
    img.get_to(encoded_data);
    image = encoded_data.empty() ? Image_data{} : Image_data{encoded_data};
  }
  else {
    image = Image_data{};
  }
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
  class_<Image_data>("ImageData")
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

    .function("crop", select_overload<Image_data(size_t, size_t, size_t, size_t) const>(&Image_data::crop))
    .function("cropRatio", select_overload<Image_data(size_t, size_t) const>(&Image_data::crop))

    .function("rotateClockwise", &Image_data::rotate_clockwise)
    .function("rotateAnticlockwise", &Image_data::rotate_anticlockwise)
    .function("flipVertically", &Image_data::flip_vertically)
    .function("flipHorizontally", &Image_data::flip_horizontally)

    .function("adjustContrastBrightness", &Image_data::adjust_contrast_brightness)
    .function("adjustContrast", &Image_data::adjust_contrast)
    .function("adjustBrightness", &Image_data::adjust_brightness)

    // Export to memory buffer
    .function("encodePng", optional_override([](const Image_data& self) {
      std::vector<uint8_t> buffer = self.encode_png();
      // Safely copy the WASM vector into a JS Uint8Array
      return val::global("Uint8Array").new_(
          typed_memory_view(buffer.size(), buffer.data())
      );
    }))

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
