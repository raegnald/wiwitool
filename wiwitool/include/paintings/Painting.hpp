#pragma once

#include <filesystem>
#include <format>
#include <memory>
#include <vector>

#include "Image_data.hpp"
#include "paintings/Painting_frame_generator.hpp"
#include "paintings/Painting_ratio.hpp"

#include "nlohmann/json.hpp"

class Painting {
public:
  Painting(void);

  // Copy
  Painting(const Painting &) = default;
  Painting& operator=(const Painting &) = default;

  // Move
  Painting(Painting&& other) noexcept = default;
  Painting& operator=(Painting&& other) noexcept = default;

  Painting(std::filesystem::path path);
  Painting(std::vector<uint8_t> image_data);
  Painting(Image_data image_data);

  std::shared_ptr<Painting> clone(void) const;

  // Getter/setter title
  inline void set_title(std::string the_title) { title = the_title; }
  inline std::string get_title(void) const { return title; }

  // Getter/setter author
  inline void set_author(std::string the_author) { author = the_author; }
  inline std::string get_author(void) const { return author; }

  // Getter/setter for conversion ratio
  inline void set_ratio(Painting_ratio ratio) {
    conversion_ratio = ratio;
    invalidate_cache();
  }
  inline Painting_ratio get_ratio(void) const { return conversion_ratio; }

  // Getter/setter for crop
  inline void set_crop(double x, double y, double w, double h) {
    crop_x = x;
    crop_y = y;
    crop_w = w;
    crop_h = h;
    invalidate_cache();
  }
  inline double get_crop_x(void) const { return crop_x; }
  inline double get_crop_y(void) const { return crop_y; }
  inline double get_crop_w(void) const { return crop_w; }
  inline double get_crop_h(void) const { return crop_h; }

  // Getters for the painting and icon images
  const Image_data original_data(void) const;
  const Image_data cropped_original_data(void) const;
  const Image_data painting_data(void) const;
  const Image_data icon_data(void) const;

  inline std::string string_id(void) const {
    return std::format("painting_{}", painting_id);
  }

  // Compute painting and icon based on the original_image
  void refresh(void) const;

  // Transformations on paintings

  void rotate_clockwise(void);
  void rotate_anticlockwise(void);

  // Painting frames

  void use_procedural_frame(void);
  void use_no_frame(void);

  Procedural_frame_generator *get_procedural_settings(void);

  bool is_frame_procedural(void) const;
  bool is_frame_nonexistent(void) const;

  // Placeable

  void set_placeable(bool p) { placeable = p; }
  bool is_placeable(void) const { return placeable; }

  // Stonecutter recipe

  void set_has_stonecutter_recipe(bool b) { stonecutter_recipe = b; }
  bool has_stonecutter_recipe(void) const { return stonecutter_recipe; }

  // Pixels per block

  void set_pixels_per_block(size_t ppb) { pixels_per_block = ppb; }
  size_t get_pixels_per_block(void) const { return pixels_per_block; }

  // Serialisation

  friend void to_json(nlohmann::json &j, const Painting &p);
  friend void from_json(const nlohmann::json &j, Painting &p);

private:
  static constexpr inline size_t default_resolution{16}; // in pixels_per_block
  static inline unsigned int next_painting_id{0};

  unsigned int painting_id;

  std::string title, author;

  Image_data original_image;
  mutable Image_data painting, icon;
  size_t pixels_per_block{default_resolution};

  double crop_x{0.0}, crop_y{0.0}, crop_w{1.0}, crop_h{1.0};

  Painting_frame_generator frame_generator = Procedural_frame_generator{};

  Painting_ratio conversion_ratio{Nearest};

  bool placeable{true};
  bool stonecutter_recipe{true};

  inline void invalidate_cache(void) const {
    painting = Image_data{};
    icon = Image_data{};
  }

  inline void compute_painting_and_icon_if_necessary(void) const {
    if (not original_image.empty() and (painting.empty() or icon.empty()))
      refresh();
  }
};

void to_json(nlohmann::json &j, const Painting &p);
void from_json(const nlohmann::json &j, Painting &p);
