#include "Minecraft_pack.hpp"

#include "util/wiwidebug.hpp"
#include "util/memstat.hpp"

#include "Serialiser.hpp"

#include <filesystem>
#include <fstream>
#include <print>

#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;

#include "miniz.hpp"

void Minecraft_pack::generate_pack_skeletons(void) {
  json pack;

  pack["pack"]["description"] = description;

  // Datapack skeleton
  {
    in_data_folder(data_subfolder);

    pack["pack"]["min_format"] = data_ver.min;
    // pack["pack"]["max_format"] = data_ver.max;

    std::ofstream{in_data_folder() / "pack.mcmeta"} << pack.dump(4) << std::endl;
  }

  // Resource pack skeleton
  {
    in_resource_folder(assets_subfolder);

    pack["pack"]["min_format"] = res_ver.min;
    // pack["pack"]["max_format"] = res_ver.max;

    std::ofstream{in_resource_folder() / "pack.mcmeta"} << pack.dump(4) << std::endl;
  }
}

void compress_directory(std::filesystem::path directory,
                        std::filesystem::path zipname) {
  using namespace std::filesystem;
  const auto all_entries = recursive_directory_iterator(directory);

  miniz_cpp::zip_file zip;

  for (const directory_entry &entry : all_entries) {
    if (entry.is_regular_file()) {
      const std::string archive_name =
          std::filesystem::relative(entry.path(), directory).string();

      zip.write(entry.path().string(), archive_name);
    }
  }

  print_memory_stats("Zipping: Right before saving " +
                     zipname.filename().string());

  zip.save(zipname.string());
  std::filesystem::remove_all(directory);
}

void Minecraft_pack::generate(bool fresh) {
  // Clean up previous runs of generating packs
  if (fresh) {
    using namespace std::filesystem;

    if (exists(genpath / "datapack")) remove_all(genpath / "datapack");
    if (exists(genpath / "respack")) remove_all(genpath / "respack");
  }

  generate_pack_skeletons();
  generate_resource(); // respack must be generated before datapack
  generate_data();
}

void Minecraft_pack::compress_genpath(std::filesystem::path packs_zip) {
  const auto datapack_zip = genpath / "datapack.zip";
  const auto respack_zip = genpath / "respack.zip";

  // If we have a workspace icon.png, we will copy it to
  // both the data pack and resource pack directories.
  if (std::filesystem::exists(genpath / "pack.png")) {
    std::filesystem::copy(genpath / "pack.png",
                          genpath / "datapack" / "pack.png");

    std::filesystem::copy(genpath / "pack.png",
                          genpath / "respack" / "pack.png");
  }

  compress_directory(genpath / "datapack", datapack_zip);
  compress_directory(genpath / "respack", respack_zip);

  // Combining data and resource packs into a single zip file

  miniz_cpp::zip_file zip;

  zip.write(datapack_zip.string());
  zip.write(respack_zip.string());

  // Add import file when it exists
  if (std::filesystem::exists(genpath / Serialiser::import_filename)) {
    const auto importfile = genpath / Serialiser::import_filename;
    zip.write(importfile.string(), Serialiser::import_filename.string());
    std::filesystem::remove(importfile);
  }

  zip.save(packs_zip.string());

  std::filesystem::remove(datapack_zip);
  std::filesystem::remove(respack_zip);
}

void Minecraft_pack::compress_packs(void) {
  Minecraft_pack::compress_genpath(genpath / "packs.zip");
}

std::filesystem::path Minecraft_pack::in_data_folder(std::filesystem::path name) {
  auto dir = genpath / "datapack" / name;
  std::filesystem::create_directories(dir);
  return dir;
}

std::filesystem::path Minecraft_pack::in_resource_folder(std::filesystem::path name) {
  auto dir = genpath / "respack" / name;
  std::filesystem::create_directories(dir);
  return dir;
}


#ifdef EMSCRIPTEN
#include <emscripten/bind.h>

using namespace emscripten;

EMSCRIPTEN_BINDINGS(minecraft_pack) {
  class_<Minecraft_pack>("MinecraftPack")
      .function("generate", &Minecraft_pack::generate)
      .function("compress", &Minecraft_pack::compress_packs);
}
#endif
