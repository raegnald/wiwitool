#include "music/Music_discs_pack.hpp"

#include <fstream>
#include <memory>
#include <print>

#include "util/strutil.hpp"
#include "util/wiwidebug.hpp"
#include "util/memstat.hpp"

#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;

Music_discs_pack::Music_discs_pack(void)
    : Minecraft_pack{"Minecraft pack containing custom music discs"},
      music_discs_namespace(timestamped(default_music_discs_namespace)) {}

void Music_discs_pack::set_discs(std::vector<std::shared_ptr<Music_disc>> ds) {
  discs = std::move(ds);
}

void Music_discs_pack::generate_data(void) {
  if (discs.empty()) return;

  // Tag containing all vanilla discs
  generate_music_discs_tag_json(
      in_data_folder(data_subfolder / music_discs_namespace / "tags" / "item"));

  const auto jukebox_folder =
      in_data_folder(data_subfolder / music_discs_namespace / "jukebox_song");

  const auto recipe_folder =
      in_data_folder(data_subfolder / music_discs_namespace / "recipe");

  for (auto &disc : discs) {
    generate_jukebox_song_json(*disc, jukebox_folder);
    generate_recipe_json(*disc, recipe_folder);
  }
}

void Music_discs_pack::generate_music_discs_tag_json(
    std::filesystem::path directory) {

  std::ofstream jsonfile{directory / "music_discs.json"};
  json data;

  data["replace"] = false;
  data["values"] = {"minecraft:music_disc_11",
                    "minecraft:music_disc_13",
                    "minecraft:music_disc_5",
                    "minecraft:music_disc_blocks",
                    "minecraft:music_disc_cat",
                    "minecraft:music_disc_chirp",
                    "minecraft:music_disc_creator",
                    "minecraft:music_disc_creator_music_box",
                    "minecraft:music_disc_far",
                    "minecraft:music_disc_lava_chicken",
                    "minecraft:music_disc_mall",
                    "minecraft:music_disc_mellohi",
                    "minecraft:music_disc_otherside",
                    "minecraft:music_disc_pigstep",
                    "minecraft:music_disc_precipice",
                    "minecraft:music_disc_relic",
                    "minecraft:music_disc_stal",
                    "minecraft:music_disc_strad",
                    "minecraft:music_disc_tears",
                    "minecraft:music_disc_wait",
                    "minecraft:music_disc_ward"};

  jsonfile << data.dump(4) << std::endl;
}

void Music_discs_pack::generate_jukebox_song_json(
    Music_disc &disc, std::filesystem::path directory) {

  const auto json_name = disc.string_id() + ".json";
  std::ofstream jsonfile{directory / json_name};

  json data;

  data["comparator_output"] = disc.get_comparator_output();
  data["description"] = disc.description();
  data["length_in_seconds"] =
      disc.get_trimmed_duration() + disc.get_silence_gap();
  data["sound_event"]["sound_id"] =
      music_discs_namespace + ":music_disc." + disc.string_id();

  jsonfile << data.dump(4) << std::endl;
}

void Music_discs_pack::generate_recipe_json(Music_disc &disc,
                                            std::filesystem::path directory) {

  const auto json_name = disc.string_id() + ".json";
  std::ofstream jsonfile{directory / json_name};

  json data;

  data["type"] = "minecraft:stonecutting";
  data["ingredient"] = "#" + music_discs_namespace + ":music_discs";
  data["result"] = {{"count", 1},
                    {"id", base_disc_id},
                    {"components",
                     {{"minecraft:jukebox_playable",
                       music_discs_namespace + ":" + disc.string_id()}}}};

  jsonfile << data.dump(4) << std::endl;
}

void Music_discs_pack::generate_resource(void) {
  if (discs.empty()) return;

  // global sounds.json file
  generate_sounds_json(
      in_resource_folder(assets_subfolder / music_discs_namespace));

  // override for the vanilla base disc
  generate_vanilla_override_json(
      in_resource_folder(assets_subfolder / "minecraft" / "items"));

  const auto models_folder = in_resource_folder(
      assets_subfolder / music_discs_namespace / "models" / "item");

  const auto textures_folder = in_resource_folder(
      assets_subfolder / music_discs_namespace / "textures" / "item");

  const auto sounds_folder = in_resource_folder(
      assets_subfolder / music_discs_namespace / "sounds" / "records");

  size_t count = 0;
  for (auto &disc : discs) {
    generate_item_model_json(*disc, models_folder);
    export_disc_texture(*disc, textures_folder);
    wiwidebug std::println("Exporting ogg audio for {}", disc->string_id());
    export_ogg_audio(*disc, sounds_folder);
    wiwidebug std::println("  done");

    count++;
    print_memory_stats("Music Loop: Processed " + std::to_string(count));
  }
}

void Music_discs_pack::generate_sounds_json(std::filesystem::path directory) {
  std::ofstream jsonfile{directory / "sounds.json"};
  json data;

  for (const auto &disc : discs) {
    std::string sound_event = "music_disc." + disc->string_id();
    std::string sound_path =
        music_discs_namespace + ":records/" + disc->string_id();

    data[sound_event]["sounds"].push_back({{"name", sound_path}});
  }

  jsonfile << data.dump(4) << std::endl;
}

void Music_discs_pack::generate_vanilla_override_json(
    std::filesystem::path directory) {

  // Determine the base disc name without the "minecraft:" prefix
  std::string clean_base_name = base_disc_id;
  if (clean_base_name.starts_with("minecraft:"))
    clean_base_name = clean_base_name.substr(10);

  std::ofstream jsonfile{directory / (clean_base_name + ".json")};
  json data;

  data["model"]["type"] = "minecraft:select";
  data["model"]["property"] = "minecraft:component";
  data["model"]["component"] = "minecraft:jukebox_playable";

  for (const auto &disc : discs) {
    data["model"]["cases"].push_back(
        {{"when", music_discs_namespace + ":" + disc->string_id()},
         {"model",
          {{"type", "minecraft:model"},
           {"model", music_discs_namespace + ":item/" + disc->string_id()}}}});
  }

  data["model"]["fallback"] = {{"type", "minecraft:model"},
                               {"model", "minecraft:item/" + clean_base_name}};

  jsonfile << data.dump(4) << std::endl;
}

void Music_discs_pack::generate_item_model_json(
    Music_disc &disc, std::filesystem::path directory) {

  const auto json_name = disc.string_id() + ".json";
  std::ofstream jsonfile{directory / json_name};

  json data;

  data["parent"] = "minecraft:item/generated";
  data["textures"] = {
      {"layer0", music_discs_namespace + ":item/" + disc.string_id()}};

  jsonfile << data.dump(4) << std::endl;
}

void Music_discs_pack::export_disc_texture(Music_disc &disc,
                                           std::filesystem::path directory) {

  const auto output_filename = disc.string_id() + ".png";
  wiwidebug std::println("Generating disc texture {}...", output_filename);

  auto item_image = disc.get_disc_item_image();
  item_image.save_png(directory / output_filename);
}

void Music_discs_pack::export_ogg_audio(Music_disc &disc,
                                        std::filesystem::path directory) {

  const auto output_filename = disc.string_id() + ".ogg";
  wiwidebug std::println("Encoding & writing audio {}...", output_filename);

  const std::vector<uint8_t>* data_to_write = nullptr;
  std::vector<uint8_t> temp_trimmed_data;

  if (not disc.requires_audio_trim()) {
    data_to_write = &disc.get_ogg_audio_data();
  } else {
    temp_trimmed_data = disc.encode_ogg();
    data_to_write = &temp_trimmed_data;
  }

  if (not data_to_write->empty()) {
    std::ofstream outfile(directory / output_filename, std::ios::binary);
    outfile.write(reinterpret_cast<const char *>(data_to_write->data()),
                  data_to_write->size());
  } else {
    wiwidebug std::println("Warning: Encoded OGG data was empty for {}",
                           disc.string_id());
  }
}
