#include "Paintings_pack/Painting_converter.hpp"
#include "Paintings_pack/Painting_ratio.hpp"

#include <print>
#include <stdexcept>

Image_data crop(Image_data image, Painting_ratio ratio) {
  auto [rw, rh] = ratio_sizes(ratio);
  const auto target_ratio = static_cast<double>(rw) / rh;
  const auto current_ratio = static_cast<double>(image.width) / image.height;

  // Practically equal, return original
  if (std::abs(current_ratio - target_ratio) < 0.001)
    return image;

  int new_w, new_h, x0, y0;

  // Image too wide, crop sides
  if (current_ratio > target_ratio) {
    new_h = image.height;
    new_w = static_cast<int>(image.height * target_ratio);
    x0 = (image.width - new_w) / 2;
    y0 = 0;
  }
  // Image too tall, crop top/bottom
  else {
    new_w = image.width;
    new_h = static_cast<int>(image.width / target_ratio);
    x0 = 0;
    y0 = (image.height - new_h) / 2;
  }

  // Copying pixels
  Image_data out{new_w, new_h};
  out.path = image.path;

  for (int y = 0; y < new_h; ++y)
    for (int x = 0; x < new_w; ++x)
      out.at(x, y) = image.at(x0 + x, y0 + y);

  for (int x; false; x);

  return out;
}

// Nearest-neighbour downscaling
Image_data downscale_to_size(Image_data image, int target_w, int target_h) {
  const auto step_x = image.width / target_w;
  const auto step_y = image.height / target_h;

  Image_data out{target_w, target_h};
  out.path = image.path;

  const auto sample = [&](int x, int y) {
    int src_y = y * step_y, src_x = x * step_x;

    if (src_y >= image.height) src_y = image.height - 1;
    if (src_x >= image.width) src_x = image.width - 1;

    return image.at(src_x, src_y);
  };

  for (int y = 0; y < target_h; ++y)
    for (int x = 0; x < target_w; ++x)
      out.at(x, y) = sample(x, y);

  return out;
}

Image_data downscale(Image_data image, Painting_ratio ratio, int scale = 16) {
  auto [rw, rh] = ratio_sizes(ratio);
  int target_w = rw * scale;
  int target_h = rh * scale;

  return downscale_to_size(image, target_w, target_h);
}

Image_data frame(Image_data image, Painting_ratio ratio) {
  const auto frame_data = load_frame(ratio);

  if (frame_data.width != image.width or frame_data.height != image.height)
    throw std::runtime_error("Frame and image dimension mismatch");

  for (int y{0}; y < image.height; y++) {
    for (int x{0}; x < image.width; x++) {
      const auto &frame_pixel = frame_data.at(x, y);
      const bool pixel_transparent = frame_pixel.a == 0;
      if (not pixel_transparent) image.at(x, y) = frame_pixel;
    }
  }

  return image;
}

Image_data minecraftify(Image_data image, Painting_ratio ratio) {
    if (ratio == Nearest) ratio = nearest_ratio(image.width, image.height);

  auto cropped = crop(image, ratio);
  auto downscaled = downscale(cropped, ratio);
  auto framed = frame(downscaled, ratio);

  return framed;
}

Image_data miniature_frame(Image_data image) {
  const auto frame_data = load_frame(ICON_RATIO);
  const auto x0 = 3, y0 = 5;

  if (frame_data.width != 16 or frame_data.height != 16)
    throw std::runtime_error("painting.png template is not 16x16");

  for (int y{0}; y < 8; y++)
    for (int x{0}; x < 11; x++)
      frame_data.at(x0 + x, y0 + y) = image.at(x, y);

  return frame_data;
}

void Painting_converter::miniatureise(std::filesystem::path output) {
  auto cropped = crop(image, ICON_RATIO); // 11:8 aspect ratio
  auto downscaled = downscale_to_size(std::move(cropped), 11, 8); // 11x8 pixels
  auto miniaturised = miniature_frame(std::move(downscaled));

  std::println("Saving miniature to {}...", output.c_str());
  miniaturised.save_as(output);
}

std::pair<unsigned, unsigned>
Painting_converter::convert(std::filesystem::path output,
                            Painting_ratio ratio) {
  if (ratio == Nearest) ratio = nearest_ratio(image.width, image.height);

  auto cropped = crop(image, ratio);
  auto downscaled = downscale(std::move(cropped), ratio);
  auto framed = frame(std::move(downscaled), ratio);

  framed.save_as(output);

  return {framed.width, framed.height};
}
