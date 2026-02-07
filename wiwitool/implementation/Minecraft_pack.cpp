#include "Minecraft_pack.hpp"

#include <fstream>
#include <utility>

#include "nlohmann/json.hpp"
using json = nlohmann::ordered_json;

// #include "zip_file.hpp"

void Minecraft_pack::generate_pack_skeletons(void) {
  json pack;

  pack["pack"]["description"] = description;

  // Datapack skeleton
  {
    in_data_folder(data_subfolder);

    pack["pack"]["min_format"] = data_ver.min;
    pack["pack"]["max_format"] = data_ver.max;

    std::ofstream{in_data_folder() / "pack.mcmeta"} << pack.dump(4) << std::endl;
  }

  // Resource pack skeleton
  {
    in_resource_folder(assets_subfolder);

    pack["pack"]["min_format"] = res_ver.min;
    pack["pack"]["max_format"] = res_ver.max;

    std::ofstream{in_resource_folder() / "pack.mcmeta"} << pack.dump(4) << std::endl;
  }
}

void compress_folder(std::filesystem::path folder,
                     std::filesystem::path zipname) {
  // TODO: Fix the compression feature
  std::println("Compression unimplemented");
  std::unreachable();

  // using namespace miniz_cpp;
  // using namespace std::filesystem;

  // zip_file zip;

  // const auto all_entries = recursive_directory_iterator(folder);
  // for (const directory_entry &entry : all_entries) {
  //   wiwidebug std::println("Compressing {} in {}...", entry.path().string(),
  //                zipname.string());
  //   if (entry.is_regular_file())
  //     zip.write(entry.path());
  // }

  // zip.save(zipname);
}

void Minecraft_pack::generate(void) {
  generate_pack_skeletons();
  generate_resource(); // FOTUT: resuce must be generated beforehand
  generate_data();
}

void Minecraft_pack::compress(void) {
  compress_folder(genpath / "datapack", genpath / "datapack.zip");
  compress_folder(genpath / "respack", genpath / "respack.zip");
}

std::filesystem::path Minecraft_pack::in_data_folder(std::string name) {
  auto dir = genpath / "datapack" / name;
  std::filesystem::create_directories(dir);
  return dir;
}

std::filesystem::path Minecraft_pack::in_resource_folder(std::string name) {
  auto dir = genpath / "respack" / name;
  std::filesystem::create_directories(dir);
  return dir;
}
