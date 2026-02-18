// Aggregate multiple Minecraft_pack into a single file containing a
// single datapack.zip and respack.zip

#pragma once

#include "Minecraft_pack.hpp"
#include <filesystem>
#include <memory>
#include <vector>
#include <string>

class Minecraft_packer {
public:
  Minecraft_packer(void) = default;

  void add(Minecraft_pack &);

  std::filesystem::path get_zip(void);

private:
  static std::filesystem::path root_genpath;

  std::vector<Minecraft_pack *> packs;
};
