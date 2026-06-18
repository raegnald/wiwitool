#include "paintings/Painting_converter.hpp"

#include "paintings/Painting_frame_generator.hpp"
#include "paintings/Painting_ratio.hpp"

#include <print>
#include <stdexcept>

static constexpr auto icon_ratio = Painting_ratio{11, 8};

Image_data crop(Image_data image, Painting_ratio ratio) {
  auto [rw, rh] = ratio.sizes();
  return image.crop(rw, rh);
}

Image_data downscale(Image_data image, Painting_ratio ratio, int scale = 16) {
  auto [rw, rh] = ratio.sizes();
  return image.scale(rw * scale, rh * scale);
}

Image_data frame(const Painting_frame_generator &frame_generator,
                 Image_data image, Painting_ratio ratio,
                 size_t pixels_per_block) {
  const auto frame_data = get_frame(frame_generator, ratio, pixels_per_block);

  if (frame_data.empty())
    return image;

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
  auto frame_data = Image_data{frames_directory / "painting.png"};
  constexpr auto x0 = 3, y0 = 5;

  if (frame_data.width() != 16 or frame_data.height() != 16)
    throw std::runtime_error("painting.png template is not 16x16");

  for (int y{0}; y < 8; y++)
    for (int x{0}; x < 11; x++)
      frame_data.at(x0 + x, y0 + y) = image.at(x, y);

  return frame_data;
}

Image_data Painting_converter::miniatureise(void) {
  auto cropped = crop(image, icon_ratio);
  auto downscaled = cropped.scale(11, 8); // 11x8 pixels
  auto miniaturised = miniature_frame(std::move(downscaled));

  return miniaturised;
}

Image_data
Painting_converter::convert(const Painting_frame_generator &frame_generator,
                            size_t pixels_per_block, Painting_ratio ratio,
                            float contrast, int brightness) {
  if (ratio.using_nearest_ratio())
    ratio = Painting_ratio::nearest_to(image.size());

  auto cropped = crop(image, ratio);

  auto downscaled = downscale(std::move(cropped), ratio, pixels_per_block)
                        .adjust_contrast_brightness(contrast, brightness);

  auto framed = frame(frame_generator, std::move(downscaled), ratio, pixels_per_block);

  return framed;
}
