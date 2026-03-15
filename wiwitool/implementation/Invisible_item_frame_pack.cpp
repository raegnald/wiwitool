#include "Invisible_item_frame_pack.hpp"

#include "Minecraft_pack.hpp"
#include "util/strutil.hpp"
#include <fstream>

Invisible_item_frame_pack::Invisible_item_frame_pack(std::string mc_namespace)
    : Minecraft_pack{"A recipe to craft invisible item frames"},
      item_frame_namespace_{timestamped(mc_namespace)} {}

constexpr auto invisible_glowing_item_frame_recipe_json = R"({
	"type": "minecraft:crafting_shapeless",
	"ingredients": [
		"minecraft:glow_item_frame",
		"minecraft:glass_pane",
		"minecraft:fermented_spider_eye",
		"minecraft:golden_carrot"
	],
	"result": {
		"id": "minecraft:glow_item_frame",
		"components": {
			"minecraft:item_name": "Invisible Glow Item Frame",
			"minecraft:entity_data": {
				"id": "minecraft:glow_item_frame",
				"Invisible": true
			}
		},
		"count": 1
	}
})";

constexpr auto invisible_item_frame_recipe_json = R"({
	"type": "minecraft:crafting_shapeless",
	"ingredients": [
		"minecraft:item_frame",
		"minecraft:glass_pane",
		"minecraft:fermented_spider_eye",
		"minecraft:golden_carrot"
	],
	"result": {
		"id": "minecraft:item_frame",
		"components": {
			"minecraft:item_name": "Invisible Item Frame",
			"minecraft:entity_data": {
				"id": "minecraft:item_frame",
				"Invisible": true
			}
		},
		"count": 1
	}
})";

void Invisible_item_frame_pack::generate_data(void) {
  const auto folder =
      in_data_folder(data_subfolder / item_frame_namespace_ / "recipe");

  std::ofstream{folder / "invisible_glowing_item_frame.json"}
      << invisible_glowing_item_frame_recipe_json << std::endl;

  std::ofstream{folder / "invisible_item_frame.json"}
      << invisible_item_frame_recipe_json << std::endl;
}

void Invisible_item_frame_pack::generate_resource() { return; }


#ifdef EMSCRIPTEN
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(invisible_item_frame_pack) {
  class_<Invisible_item_frame_pack, base<Minecraft_pack>>(
      "InvisibleItemFramePack")
      .constructor<>();
}
#endif
