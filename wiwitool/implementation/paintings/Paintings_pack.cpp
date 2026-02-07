#include "paintings/Paintings_pack.hpp"

#include <fstream>

#include "paintings/Painting_converter.hpp"

#include "util/wiwidebug.hpp"

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

  for (const auto &painting : paintings) {
    // Declare a new painting
    generate_painting_variant_json(
        painting, in_data_folder(data_subfolder / paintings_namespace /
                                 "painting_variant"));

    // Stone cutter recipe to obtain the painting
    generate_recipe_json(
        painting,
        in_data_folder(data_subfolder / paintings_namespace / "recipe"));
  }
}

void Paintings_pack::generate_recipe_json(const Painting &painting,
                                          std::filesystem::path in_folder) {
  const auto json_name = painting.string_id() + "_recipe.json";
  std::ofstream jsonfile{in_folder / json_name};

  json data;

  data["type"] = "minecraft:stonecutting";
  data["ingredient"] = "minecraft:painting";
  data["result"] = {
      {"id", "minecraft:painting"},
      {"components",
       {{"minecraft:painting/variant",
         paintings_namespace + ":" + painting.string_id()}}},
      {"count", 1}};

  jsonfile << data.dump(4) << std::endl;
}

void Paintings_pack::generate_painting_resource(
    const Painting &painting, std::filesystem::path directory) {

  const auto output_filename = painting.string_id() + ".png";
  wiwidebug std::println("Generating painting {}...", output_filename);

  auto painting_image = Painting_converter{painting.painting_data()}.convert();
  painting_image.save_png(directory / output_filename);
  wiwidebug std::println("Saved painting to ", (directory / output_filename).string());
}


std::vector<std::string> Paintings_pack::get_painting_ids(void) const {
  auto painting_ids = std::vector<std::string>{};

  for (const auto &painting : paintings)
    painting_ids.push_back(paintings_namespace + ":" + painting.string_id());

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

  for (const auto &painting : paintings) {
    generate_painting_resource(painting, paintings_image_folder); // FOTUT: must run before generating JSON

    // Miniature (aka. item)
    generate_respack_item_json(painting, item_jsons_folder);
    generate_miniature_resource(painting, miniatures_folder);

    // Linking painting to miniature
    generate_painting_json(painting, painting_to_icon_json_folder);
  }
}


void Paintings_pack::generate_painting_json(const Painting &painting,
                                            std::filesystem::path directory) {
  std::ofstream jsonfile{directory / "painting.json"};

  json data;

  data["model"]["type"] = "minecraft:select";
  data["model"]["property"] = "minecraft:component";
  data["model"]["component"] = "minecraft:painting/variant";

  for (const auto &painting : paintings) {
    data["model"]["cases"].push_back({
      {"when", paintings_namespace + ":" + painting.string_id()},
      {"model", {
        {"type", "minecraft:model"},
        {"model", item_id(painting)}
      }}
    });
  }

  data["model"]["fallback"]["type"] = "minecraft:model";
  data["model"]["fallback"]["model"] = "minecraft:item/painting";

  jsonfile << data.dump(4) << std::endl;
}

void Paintings_pack::generate_miniature_resource(
    const Painting &painting, std::filesystem::path directory) {

  const auto output_filename = std::format("{}.png", painting.string_id());
  wiwidebug std::println("Generating painting miniature {}...", output_filename);
  const auto item_image = Painting_converter(painting.original_data()).miniatureise();
  item_image.save_as(directory / output_filename);
}


void Paintings_pack::generate_respack_item_json(
    const Painting &painting, std::filesystem::path directory) {

  const auto json_name = painting.string_id() + ".json";
  std::ofstream jsonfile{directory / json_name};

  json data;

  data["parent"] = "minecraft:item/generated";
  data["textures"] = {{"layer0", item_id(painting)}};

  jsonfile << data.dump(4) << std::endl;
}

void Paintings_pack::generate_painting_variant_json(
    const Painting &painting, std::filesystem::path in_folder) {

  const auto json_name = painting.string_id() + ".json";
  std::ofstream jsonfile{in_folder / json_name};

  wiwidebug std::println("Writing {}", (in_folder / json_name).string());

  json data;

  data["asset_id"] = painting_id(painting);
  // I still don't like for the scale of 16 to be hard-coded
  data["width"] = painting.painting_data().width() / 16;
  data["height"] = painting.painting_data().height() / 16;
  data["title"].push_back({{"text", painting.get_title()}});
  data["author"].push_back({{"text", painting.get_author()}});

  jsonfile << data.dump(4) << std::endl;
}

void Paintings_pack::set_paintings(std::vector<Painting> paintings) {
  this->paintings = std::move(paintings);
}


#ifdef EMSCRIPTEN
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(paintings_pack) {
  register_vector<Painting>("PaintingsVector");

  class_<Paintings_pack, base<Minecraft_pack>>("PaintingsPack")
      .constructor<>()
      .function("setPaintings", &Paintings_pack::set_paintings);
}
#endif
