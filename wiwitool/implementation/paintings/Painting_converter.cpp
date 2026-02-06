#include "paintings/Painting_converter.hpp"

#include "paintings/Painting_ratio.hpp"

#include <print>
#include <stdexcept>

Image_data crop(Image_data image, Painting_ratio ratio) {
  auto [rw, rh] = ratio_sizes(ratio);
  const auto target_ratio = static_cast<double>(rw) / rh;
  const auto current_ratio = static_cast<double>(image.width()) / image.height();

  // Practically equal, return original
  if (std::abs(current_ratio - target_ratio) < 0.001)
    return image;

  int new_w, new_h, x0, y0;

  // Image too wide, crop sides
  if (current_ratio > target_ratio) {
    new_h = image.height();
    new_w = static_cast<int>(image.height() * target_ratio);
    x0 = (image.width() - new_w) / 2;
    y0 = 0;
  }
  // Image too tall, crop top/bottom
  else {
    new_w = image.width();
    new_h = static_cast<int>(image.width() / target_ratio);
    x0 = 0;
    y0 = (image.height() - new_h) / 2;
  }

  return image.crop(x0, y0, new_w, new_h);
}

Image_data downscale(Image_data image, Painting_ratio ratio, int scale = 16) {
  auto [rw, rh] = ratio_sizes(ratio);
  return image.scale(rw * scale, rh * scale);
}

Image_data frame(Image_data image, Painting_ratio ratio) {
  const auto frame_data = load_frame(ratio);

  if (frame_data.width() != image.width() or
      frame_data.height() != image.height())
    throw std::runtime_error("Frame and image dimension mismatch");

  for (size_t y = 0; y < image.height(); y++) {
    for (size_t x = 0; x < image.width(); x++) {
      const auto &frame_pixel = frame_data.at(x, y);
      const bool pixel_transparent = frame_pixel.a == 0;
      if (not pixel_transparent) image.at(x, y) = frame_pixel;
    }
  }

  return image;
}

Image_data miniature_frame(Image_data image) {
  auto frame_data = load_frame(ICON_RATIO);
  constexpr auto x0 = 3, y0 = 5;

  if (frame_data.width() != 16 or frame_data.height() != 16)
    throw std::runtime_error("painting.png template is not 16x16");

  for (int y{0}; y < 8; y++)
    for (int x{0}; x < 11; x++)
      frame_data.at(x0 + x, y0 + y) = image.at(x, y);

  return frame_data;
}

Image_data Painting_converter::miniatureise(void) {
  auto cropped = crop(image, ICON_RATIO); // 11:8 aspect ratio
  auto downscaled = cropped.scale(11, 8); // 11x8 pixels
  auto miniaturised = miniature_frame(std::move(downscaled));

  return miniaturised;
}

Image_data Painting_converter::convert(Painting_ratio ratio) {
  if (ratio == Nearest) ratio = nearest_ratio(image.width(), image.height());

  auto cropped = crop(image, ratio);
  auto downscaled = downscale(std::move(cropped), ratio);
  auto framed = frame(std::move(downscaled), ratio);

  return framed;
}
