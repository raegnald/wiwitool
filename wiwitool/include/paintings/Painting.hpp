#pragma once

#include <filesystem>
#include <format>
#include <vector>

#include "Image_data.hpp"
#include "paintings/Painting_ratio.hpp"

class Painting {
public:
  Painting(void) = delete;

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
  inline void set_ratio(Painting_ratio ratio) { conversion_ratio = ratio; refresh(); }
  inline Painting_ratio get_ratio(void) const { return conversion_ratio; }

  // Getters for the painting and icon images
  const Image_data original_data(void) const;
  const Image_data painting_data(void) const;
  const Image_data icon_data(void) const;

  inline std::string string_id(void) const {
    return std::format("painting_{}", painting_id);
  }

  // Transformations on paintings

  void rotate_clockwise(void);
  void rotate_anticlockwise(void);

private:
  static inline unsigned int next_painting_id{0};

  unsigned int painting_id;

  std::string title, author;

  Image_data original_image;
  mutable Image_data painting, icon;

  Painting_ratio conversion_ratio{Nearest};

  // Compute painting and icon based on the original_image
  void refresh(void) const;

  inline void compute_painting_and_icon_if_necessary(void) const {
    if (not original_image.empty() and (painting.empty() or icon.empty()))
      refresh();
  }
};
