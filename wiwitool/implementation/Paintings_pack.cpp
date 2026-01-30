#include "Paintings_pack.hpp"

#include <fstream>

#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;

Paintings_pack::Paintings_pack(void)
    : Minecraft_pack{"A pack with custom paintings"} {}


void Paintings_pack::generate_data(void) {
  // Minecraft namespace (wrinting placeable.json)
  {
    const auto painting_variant_folder = in_data_folder(
        data_subfolder / "minecraft" / "tags" / "painting_variant");

    std::ofstream placeable_json{painting_variant_folder / "placeable.json"};

    const auto painting_ids = get_painting_ids();
    json content;

    content["replace"] = false;
    content["values"] = painting_ids;

    placeable_json << content.dump(4) << std::endl;

  }

  // Custom paintings namespace

  for (auto &painting : paintings) {
    // Declare a new painting
    painting.generate_painting_variant_json(in_data_folder(
        data_subfolder / paintings_namespace / "painting_variant"));

    // Stone cutter recipe to obtain the painting
    painting.generate_recipe_json(
        in_data_folder(data_subfolder / paintings_namespace / "recipe"));
  }
}


std::vector<std::string> Paintings_pack::get_painting_ids(void) const {
  auto painting_ids = std::vector<std::string>{};

  for (const auto &painting : paintings)
    painting_ids.push_back(painting.id());

  return painting_ids;
}


void Paintings_pack::generate_resource(void) {
  // Folder where the Minecraft paintings are exported
  const auto paintings_image_folder = in_resource_folder(
      assets_subfolder / paintings_namespace / "textures" / "painting");

  // Folder that contains a JSON file for every painting
  const auto item_jsons_folder = in_resource_folder(
      assets_subfolder / paintings_namespace / "models" / "item");

  // Folder where the miniature icon is exported (shown for instance
  // in the inventory or user's hand)
  const auto miniatures_folder = in_resource_folder(
      assets_subfolder / paintings_namespace / "textures" / "item");

  const auto painting_to_icon_json_folder =
      in_resource_folder(assets_subfolder / "minecraft" / "items");

  for (auto &painting : paintings) {
    painting.generate_painting_resource(paintings_image_folder); // FOTUT: must run before generating JSON

    // Miniature (aka. item)
    painting.generate_respack_item_json(item_jsons_folder);
    painting.generate_miniature_resource(miniatures_folder);

    // Linking painting to miniature
    generate_painting_json(painting_to_icon_json_folder);
  }
}

void Paintings_pack::generate_painting_json(std::filesystem::path directory) {
  std::ofstream jsonfile{directory / "painting.json"};

  json data;

  data["model"]["type"] = "minecraft:select";
  data["model"]["property"] = "minecraft:component";
  data["model"]["component"] = "minecraft:painting/variant";

  for (const auto &painting : paintings) {
    data["model"]["cases"].push_back({
      {"when", painting.id()},
      {"model", {
        {"type", "minecraft:model"},
        {"model", painting.item_id()}
      }}
    });
  }

  data["model"]["fallback"]["type"] = "minecraft:model";
  data["model"]["fallback"]["model"] = "minecraft:item/painting";

  jsonfile << data.dump(4) << std::endl;
}



Painting &Paintings_pack::add_painting(std::string filename) {
  std::println("Adding painting from file {}", filename);
  paintings.push_back(Painting{paintings_namespace, filename});
  return paintings.back();
}


#ifdef EMSCRIPTEN
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(paintings_pack) {
  class_<Painting>("Painting")
      .function("set_title", &Painting::set_title)
      .function("get_title", &Painting::get_title)
      .function("set_author", &Painting::set_author)
      .function("get_author", &Painting::get_author);
  // FOTUT: binding methods taking std::filesystem::path without
  // writing a wrapper that accepts std::string is hard

  class_<Paintings_pack>("Paintings_pack")
      .constructor<>()
      .function("add_painting", &Paintings_pack::add_painting,
                return_value_policy::reference());
}
#endif
