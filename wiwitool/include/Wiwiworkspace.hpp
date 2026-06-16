// A wrapper, generator and source of truth for all the Minecraft
// packs.

#pragma once

#include "Image_data.hpp"
#include "music/Music_disc.hpp"
#include "paintings/Painting.hpp"
#include "util/strutil.hpp"
#include "util/wiwidebug.hpp"

#include <cstddef>
#include <filesystem>
#include <memory>
#include <optional>
#include <print>
#include <vector>

#include "nlohmann/json.hpp"

class Namespace_override {
public:
  Namespace_override(void) = default;
  Namespace_override(std::string n) { set(n); }

  static std::shared_ptr<Namespace_override> create_shared(void) {
    return std::make_shared<Namespace_override>();
  }

  bool has_value(void) const { return namespace_override.has_value(); }

  // Setter, getter, reset

  void set(std::string n) {
    wiwidebug std::println("Setting namespace override to [{}]", n);

    if (n.empty()) {
      reset();
      return;
    }

    namespace_override = n;
  }

  std::optional<std::string> get(void) const { return namespace_override; }

  void reset(void) { namespace_override = std::nullopt; }

  // Serialisation

  friend void to_json(nlohmann::json &j, const Namespace_override &p);
  friend void from_json(const nlohmann::json &j, Namespace_override &p);

private:
  std::optional<std::string> namespace_override{std::nullopt};
};

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

  std::shared_ptr<Namespace_override> get_paintings_namespace_override(void) const {
    return paintings_namespace_override;
  }

  // Music discs

  void add_music_disc(std::shared_ptr<Music_disc>);
  void remove_music_disc(std::string id);

  std::vector<std::shared_ptr<Music_disc>> get_music_discs(void) const;

  std::shared_ptr<Namespace_override> get_music_discs_namespace_override(void) const {
    return music_discs_namespace_override;
  }

  // Invisible item frames

  void set_invisible_item_frames(bool);
  bool get_invisible_item_frames() const;

  std::shared_ptr<Namespace_override> get_iif_namespace_override(void) const {
    return iif_namespace_override;
  }

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

  // Namespace overrides let the user change the name of the namespace
  // where their packs live (this feature is intented for advanced
  // users, or users who have an old wiwipack and want to keep their
  // pack working in their world)

  std::shared_ptr<Namespace_override> paintings_namespace_override =
      Namespace_override::create_shared();

  std::shared_ptr<Namespace_override> music_discs_namespace_override =
      Namespace_override::create_shared();

  std::shared_ptr<Namespace_override> iif_namespace_override =
      Namespace_override::create_shared();

  // Workspace is newly created (0), or reimported (>0)
  int export_count{0};
};
