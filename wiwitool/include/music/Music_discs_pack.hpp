// A Minecraft pack generator for custom music discs

#pragma once

#include "Minecraft_pack.hpp"
#include "music/Music_disc.hpp"
#include <format>
#include <memory>

static std::string default_music_discs_namespace{"music_discs"};

class Music_discs_pack : public Minecraft_pack {
public:
  Music_discs_pack(void);
  virtual ~Music_discs_pack(void) = default;

  void set_workspace_name(std::string workspace_name, bool add_suffix = true) {
    if (add_suffix)
      music_discs_namespace =
          std::format("{}_{}", workspace_name, default_music_discs_namespace);
    else
      music_discs_namespace = workspace_name;
  }

  void set_discs(std::vector<std::shared_ptr<Music_disc>> discs);

protected:
  virtual void generate_data(void) override;
  virtual void generate_resource(void) override;

private:
  std::string music_discs_namespace;
  std::vector<std::shared_ptr<Music_disc>> discs{};

  const std::string base_disc_id = "minecraft:music_disc_ward";

  // Data helpers
  void generate_music_discs_tag_json(std::filesystem::path directory);
  void generate_jukebox_song_json(Music_disc &disc, std::filesystem::path directory);
  void generate_recipe_json(Music_disc &disc, std::filesystem::path directory);

  // Resource helpers
  void generate_sounds_json(std::filesystem::path directory);
  void generate_item_model_json(Music_disc &disc, std::filesystem::path directory);
  void generate_vanilla_override_json(std::filesystem::path directory);
  void export_disc_texture(Music_disc &disc, std::filesystem::path directory);
  void export_ogg_audio(Music_disc &disc, std::filesystem::path directory);
};
