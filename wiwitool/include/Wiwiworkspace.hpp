// A wrapper, generator and source of truth for all the Minecraft
// packs.

#pragma once

#include "paintings/Painting.hpp"

#include <filesystem>
#include <memory>
#include <vector>

class Wiwiworkspace {
public:
  Wiwiworkspace(void) = default;

  // Paintings

  void add_painting(std::shared_ptr<Painting>);
  void remove_painting(std::string id);

  std::vector<std::shared_ptr<Painting>> get_paintings(void) const;

  // Invisible item frames

  void set_invisible_item_frames(bool);
  bool get_invisible_item_frames() const;

  // Serialisation and generation

  std::vector<uint8_t> serialise(void) const;
  void deserialise(const std::vector<uint8_t> &binary_msgpack);

  std::filesystem::path generate_zip(void);

private:
  std::vector<std::shared_ptr<Painting>> paintings;
  bool invisible_item_frames{false};
};
