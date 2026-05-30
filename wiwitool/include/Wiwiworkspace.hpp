// A wrapper, generator and source of truth for all the Minecraft
// packs.

#pragma once

#include "Image_data.hpp"
#include "music/Music_disc.hpp"
#include "paintings/Painting.hpp"
#include "util/strutil.hpp"

#include <cstddef>
#include <filesystem>
#include <memory>
#include <vector>

class Wiwiworkspace {
public:
  Wiwiworkspace(void) = default;

  // Workspace name

  inline std::string get_workspace_name(void) const { return workspace_name; }
  inline void set_workspace_name(std::string name) { workspace_name = name; }

  // Workspace image

  Image_data get_workspace_icon(void) const;
  void set_workspace_image(Image_data im);

  inline void reset_workspace_image(void) { workspace_image = Image_data{}; }

  // Paintings

  void add_painting(std::shared_ptr<Painting>);
  void remove_painting(std::string id);

  std::vector<std::shared_ptr<Painting>> get_paintings(void) const;

  // Music discs

  void add_music_disc(std::shared_ptr<Music_disc>);
  void remove_music_disc(std::string id);

  std::vector<std::shared_ptr<Music_disc>> get_music_discs(void) const;

  // Invisible item frames

  void set_invisible_item_frames(bool);
  bool get_invisible_item_frames() const;

  // Is workspace new?
  bool inline is_workspace_new(void) const { return export_count == 0; }

  // Serialisation and generation

  std::vector<uint8_t> serialise(void) const;
  void deserialise(const std::vector<uint8_t> &binary_msgpack);

  std::filesystem::path generate_zip(void);

private:
  std::vector<std::shared_ptr<Painting>> paintings;
  std::vector<std::shared_ptr<Music_disc>> discs;
  bool invisible_item_frames{false};

  std::string workspace_name{get_random_adjective_underscore_noun()};
  Image_data workspace_image;

  // Workspace is newly created (0), or reimported (>0)
  int export_count{0};
};
