#pragma once

#include "Minecraft_identifiable.hpp"
#include <filesystem>

#include "Image_data.hpp"

struct Transformed_image {
  std::filesystem::path path;
};

class Painting : public Minecraft_identifiable {
private:
  static inline unsigned int next_painting_id{0};
  static inline std::string painting_id_string(int id) {
    return std::format("painting_{}", id);
  }

  unsigned int painting_id;

  Image_data original_image;
  Transformed_image transformed;

  std::string title{"Painting"}, author{""};

  unsigned converted_width{0}, converted_height{0};

public:
  Painting(void) = delete;
  Painting(std::string mc_namespace, std::filesystem::path path);

  // No copy
  Painting(const Painting &) = delete;
  Painting& operator=(const Painting &) = delete;

  // Move-only type
  Painting(Painting&& other) noexcept = default;
  Painting& operator=(Painting&& other) noexcept = default;


  // Getter/setter title
  inline void set_title(std::string the_title) { title = the_title; }
  inline std::string get_title(void) const { return title; }

  // Getter/setter author
  inline void set_author(std::string the_author) { author = the_author; }
  inline std::string get_author(void) const { return author; }

  // Define a new painting type
  void generate_painting_variant_json(std::filesystem::path in_folder);

  // Definition of stonecutter recipes to obtain the custom paintings
  void generate_recipe_json(std::filesystem::path in_folder);

  // Generate the Minecraft asset of the painting
  void generate_painting_resource(std::filesystem::path directory);

  // Miniature aka icon aka item
  void generate_miniature_resource(std::filesystem::path directory);

  void generate_respack_item_json(std::filesystem::path directory);

};
