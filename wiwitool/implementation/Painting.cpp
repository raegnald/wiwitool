#include "Paintings_pack/Painting.hpp"

#include "Minecraft_identifiable.hpp"

#include <format>
#include <fstream>
#include <print>

#include "Paintings_pack/Painting_converter.hpp"
#include "wiwidebug.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;


Painting::Painting(std::string mc_namespace, std::filesystem::path image_path)
    : Minecraft_identifiable{mc_namespace,
                             painting_id_string(painting_id =
                                                    next_painting_id++)},
      original_image{image_path} {
  std::println("Loaded image (painting id = {})", painting_id);
  wiwidebug std::println("Loaded image (painting id = {})", painting_id);
}


void Painting::generate_painting_variant_json(std::filesystem::path in_folder) {
  const auto json_name = object_name + ".json";
  std::ofstream jsonfile{in_folder / json_name};

  std::println("Writing {}", (in_folder / json_name).string());

  json data;

  data["asset_id"] = id();
  data["width"] = converted_width;
  data["height"] = converted_height;
  data["title"].push_back({{"text", title}});
  data["author"].push_back({{"text", author}});

  jsonfile << data.dump(4) << std::endl;
}


void Painting::generate_recipe_json(std::filesystem::path in_folder) {
  const auto json_name = object_name + "_recipe.json";
  std::ofstream jsonfile{in_folder / json_name};

  json data;

  data["type"] = "minecraft:stonecutting";
  data["ingredient"] = "minecraft:painting";
  data["result"] = {{"id", "minecraft:painting"},
                    {"components", {{"minecraft:painting/variant", id()}}},
                    {"count", 1}};

  jsonfile << data.dump(4) << std::endl;
}


void Painting::generate_painting_resource(std::filesystem::path directory) {
  const auto output_filename = std::format("painting_{}.png", painting_id);
  std::println("Generating painting {}...", output_filename);

  const auto [w, h] =
      Painting_converter(original_image).convert(directory / output_filename);

  converted_width = w / 16;     // FOTUT: hard coded
  converted_height = h / 16;
}

void Painting::generate_miniature_resource(std::filesystem::path directory) {
  const auto output_filename = std::format("painting_{}.png", painting_id);
  std::println("Generating painting miniature {}...", output_filename);
  Painting_converter(original_image).miniatureise(directory / output_filename);
}

void Painting::generate_respack_item_json(std::filesystem::path directory) {
  const auto json_name = object_name + ".json";
  std::ofstream jsonfile{directory / json_name};

  json data;

  data["parent"] = "minecraft:item/generated";
  data["textures"] = {{"layer0", item_id()}};

  jsonfile << data.dump(4) << std::endl;
}
