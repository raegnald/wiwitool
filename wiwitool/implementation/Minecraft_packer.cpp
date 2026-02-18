#include "Minecraft_packer.hpp"

#include "Minecraft_pack.hpp"

#include <cstddef>
#include <filesystem>
#include <memory>
#include <string>

#ifdef EMSCRIPTEN
std::filesystem::path Minecraft_packer::root_genpath = "/packer";
#else
std::filesystem::path Minecraft_packer::root_genpath = std::filesystem::temp_directory_path();
#endif

void Minecraft_packer::add(Minecraft_pack &pack) {
  packs.push_back(&pack);
}

std::filesystem::path Minecraft_packer::get_zip(void) {
  const auto zippath = root_genpath / "wiwipacks.zip";

  std::filesystem::create_directories(root_genpath);

  // We will suppose no two packs produce the same file. Otherwise,
  // this simplistic approach totally breaks apart!

  for (auto pack : packs)
    pack->generate(false);

  Minecraft_pack::compress_genpath(zippath);

  return zippath;
}


#ifdef EMSCRIPTEN
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(minecraft_packer) {
  class_<Minecraft_packer>("MinecraftPacker")
      .constructor<>()
      .function("add", optional_override(
                           [](Minecraft_packer &packer, Minecraft_pack &pack) {
                             packer.add(pack);
                           }))
      .function("getZip", optional_override([](Minecraft_packer &packer) {
                  return std::string{packer.get_zip()};
                }));
}
#endif
