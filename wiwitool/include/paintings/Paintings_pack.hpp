#pragma once

#include <string>
#include <vector>

#include "Minecraft_identifiable.hpp"
#include "Minecraft_pack.hpp"
#include "paintings/Painting.hpp"

static std::string default_paintings_namespace{"custompaintings"};

class Paintings_pack : public Minecraft_pack {
public:
  Paintings_pack(void);
  virtual ~Paintings_pack() = default;

  Painting &add_painting(std::string filename);

protected:
  virtual void generate_data(void) override;
  virtual void generate_resource(void) override;

private:
  std::string paintings_namespace{default_paintings_namespace};
  std::vector<Painting> paintings{};

  std::vector<std::string> get_painting_ids(void) const;

  // IDEA: Show tree file structure and assign every file the method
  // that generates it.

  inline std::string item_id(const Painting &painting) const {
    return Minecraft_identifiable::adhoc_item(paintings_namespace,
                                              painting.string_id());
  }

  inline std::string painting_id(const Painting &painting) const {
    return Minecraft_identifiable::adhoc(paintings_namespace,
                                         painting.string_id());
  }

  void generate_painting_json(const Painting &painting,
                              std::filesystem::path directory);

  // Define a new painting type
  void generate_painting_variant_json(const Painting &painting,
                                      std::filesystem::path in_folder);

  // Definition of stonecutter recipes to obtain the custom paintings
  void generate_recipe_json(const Painting &painting,
                            std::filesystem::path in_folder);

  // Generate the Minecraft asset of the painting
  void generate_painting_resource(const Painting &painting,
                                  std::filesystem::path directory);

  // Miniature aka icon aka item
  void generate_miniature_resource(const Painting &painting,
                                   std::filesystem::path directory);

  void generate_respack_item_json(const Painting &painting,
                                  std::filesystem::path directory);
};
